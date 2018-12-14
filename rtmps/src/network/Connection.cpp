//
// Connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//#define PRINT_CLOCK
//#define PRINT_DUMP

#include "Connection.hpp"

#include <vector>
#include <boost/bind.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "rtmpmodulelogger.h"

#define READ_BUF_SIZE 8192
#define CONTINUOUS_SEND_INTERVAL_MS 5

namespace rtmp_network {

Connection::Connection(boost::asio::io_service& io_service,
                       RequestHandlerFactory_ptr handler_factory,
                       RequestParserFactory_ptr parser_factory,
                       unsigned int id)
    : strand_(io_service),
      socket_(io_service),
      id_(id),
      is_continuous_send_state_(false),
      continuous_send_timer_(
          io_service,
          boost::posix_time::milliseconds(CONTINUOUS_SEND_INTERVAL_MS)) {
  
  context_ = std::make_shared<castis::streamer::MediaPublishEsContext>();
  handler_ = handler_factory->get_request_handler(this, id);
  parser_ = parser_factory->get_request_parser(this, id);
  handshake_manager_.set_connection_id(id);
  handler_->context_ = context_;
}

Connection::~Connection() {
}

boost::asio::ip::tcp::socket& Connection::socket() {
  return socket_;
}

unsigned int Connection::get_message_sender_id() {
  return id_;
}

void Connection::start() {
  RTMPLOGF(info, "start connection[%1%]", id_ );
  unsigned int C0_C1_len = HANDSHAKE_MSG_VERSION_LEN + HANDSHAKE_MSG_LENGTH;

  boost::asio::async_read(
      socket_,
      request_streambuf_,
      boost::asio::transfer_exactly(C0_C1_len),
      strand_.wrap(
          boost::bind(&Connection::handle_read_handshake_C0_C1,
                      shared_from_this(), boost::asio::placeholders::error)));
}

void Connection::handle_read_handshake_C0_C1(
    const boost::system::error_code& e) {
  RTMPLOGF(debug, "read handshake C0_C1. connection[%1%]", id_ );

  if (e) {
    RTMPLOGF(error, "handshake C0_C1 error. connection[%1%],error_message[%2%]", id_, e.message());
    disconnect();
    return;
  }

  std::istream request_stream(&request_streambuf_);
  if (handshake_manager_.validate_C0_C1(request_stream)) {
    std::ostream reply_stream(&reply_streambuf_);
    handshake_manager_.get_S0_S1_S2(reply_stream);

    request_streambuf_.consume(HANDSHAKE_MSG_LENGTH * 2 + 1);

    boost::asio::async_write(
        socket_,
        reply_streambuf_,
        strand_.wrap(
            boost::bind(&Connection::handle_S0_S1_S2_write, shared_from_this(),
                        boost::asio::placeholders::error)));
  }
}
void Connection::handle_S0_S1_S2_write(const boost::system::error_code& e) {
  RTMPLOGF(debug, "write handshake S0_S1_S2. connection[%1%]", id_ );

  if (e) {
    RTMPLOGF(error,"write handshake S0_S1_S1 error. connection[%1%],error_message[%2%]", id_, e.message());
    disconnect();
    return;
  }

  boost::asio::async_read(
      socket_,
      request_streambuf_,
      boost::asio::transfer_exactly(HANDSHAKE_MSG_LENGTH),
      strand_.wrap(
          boost::bind(&Connection::handle_read_handshake_C2, shared_from_this(),
                      boost::asio::placeholders::error)));
}

void Connection::handle_read_handshake_C2(const boost::system::error_code& e) {
  RTMPLOGF(debug, "read handshake C2. connection[%1%]", id_ );

  if (e) {
    RTMPLOGF(error,"read handshake C2 error. connection[%1%],error_message[%2%]", id_, e.message());
    disconnect();
    return;
  }

  request_streambuf_.consume(HANDSHAKE_MSG_LENGTH);
  boost::asio::streambuf::mutable_buffers_type buf = request_streambuf_.prepare(
      READ_BUF_SIZE);
  socket_.async_read_some(
      buf,
      strand_.wrap(
          boost::bind(&Connection::handle_read, shared_from_this(),
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred)));
}

void Connection::handle_read(const boost::system::error_code& e,
                             std::size_t bytes_transferred) {
  if (e) {
    RTMPLOGF(error,"read error. connection[%1%],error_message[%2%]", id_, e.message());
    disconnect();
    return;
  }

  if (bytes_transferred > 0) {
    request_streambuf_.commit(bytes_transferred);
    std::size_t buffer_size = request_streambuf_.size();

    //RTMPLOGF(debug,"read. connection[%1%],bytes_transferred[%2%],buffer_size[%3%]", id_,bytes_transferred,buffer_size);

    std::istream request_stream(&request_streambuf_);

    std::size_t total_read_in_parse = 0;
    while (true) {
      size_t read_in_parse = 0;

      if (buffer_size == total_read_in_parse ) {
        boost::asio::streambuf::mutable_buffers_type buf = request_streambuf_
            .prepare(READ_BUF_SIZE);
        socket_.async_read_some(
            buf,
            strand_.wrap(
                boost::bind(&Connection::handle_read, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));
        return;   
      }

      boost::tribool result = parser_->parse(request_stream,
                                             buffer_size - total_read_in_parse,
                                             read_in_parse);
      total_read_in_parse += read_in_parse;
     
      if (result) {
        handler_->handle_request(parser_->get_parsed_message());
      } else if (!result) {
        RTMPLOGF(error,"parsing message error. connection[%1%]", id_ );
        disconnect();
        return;
      }
      else {
        boost::asio::streambuf::mutable_buffers_type buf = request_streambuf_
            .prepare(READ_BUF_SIZE);
        socket_.async_read_some(
            buf,
            strand_.wrap(
                boost::bind(&Connection::handle_read, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));
        return;
      }
    }
  }
}

void Connection::disconnect() {
  handler_->notify_disconnect();
  boost::system::error_code ignored_ec;
  socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
}

void Connection::push_to_send_queue(Message_ptr message) {
  write_queue_.push(message);
}

void Connection::signal_send_message() {
  if (!is_continuous_send_state_)
    write_message();
}

void Connection::write_message() {
  if (!write_queue_.empty()) {

    std::ostream reply_stream(&reply_streambuf_);
    size_t total_writed_bytes = 0;

    while (!write_queue_.empty()) {
      Message_ptr write_msg = write_queue_.front();
      write_queue_.pop();

      RTMPLOGF(info,"write message. connection[%1%],write_message[%2%]", id_, write_msg->to_string());

      size_t writed_bytes = write_msg->write_payload(reply_stream);
      total_writed_bytes += writed_bytes;
    }
    boost::asio::async_write(
        socket_,
        reply_streambuf_,
        strand_.wrap(
            boost::bind(&Connection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error)));

    reply_streambuf_.consume(total_writed_bytes);
  }
}

void Connection::handle_send_timer(const boost::system::error_code& e) {
  if (!e) {
    write_message();

    if (is_continuous_send_state_) {
      continuous_send_timer_.expires_from_now(
          boost::posix_time::milliseconds(CONTINUOUS_SEND_INTERVAL_MS));
      continuous_send_timer_.async_wait(
          strand_.wrap(
              boost::bind(&Connection::handle_send_timer, shared_from_this(),
                          boost::asio::placeholders::error)));
    }
  }
}

void Connection::handle_write(const boost::system::error_code& e) {
  if (e) {
    RTMPLOGF(error,"write error. connection[%1%],error_message[%2%]", id_, e.message());
    disconnect();
    return;
  }
}

void Connection::change_continuous_send_state(bool state) {
  const bool change_state_to_true = state && !is_continuous_send_state_;
  if (change_state_to_true) {
    RTMPLOGF(debug,"write status changed. connection[%1%]", id_);

    continuous_send_timer_.expires_from_now(
        boost::posix_time::milliseconds(CONTINUOUS_SEND_INTERVAL_MS));
    continuous_send_timer_.async_wait(
        strand_.wrap(
            boost::bind(&Connection::handle_send_timer, shared_from_this(),
                        boost::asio::placeholders::error)));
  }

  is_continuous_send_state_ = state;
}

}  // namespace rtmp_network
