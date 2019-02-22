#include "../../../src/rtmpmodulelogger.h"
#include "RtmpParser.hpp"
#include "RtmpMessage.hpp"
#include "control_message/ControlMessage.hpp"

namespace rtmp_protocol {
RtmpParser::RtmpParser()
    : connection_id_(0),
      parsing_state_(HEADER_PARSING),
      readed_bytes_count_(0),
      win_ack_size_(DEFAULT_WIN_ACK_SIZE),
      next_ack_send_size_(DEFAULT_WIN_ACK_SIZE),
      read_bytes_count_in_window_(0) {
  payload_parser_.set_connection_id(connection_id_);
  header_parser_.set_connection_id(connection_id_);
}

RtmpParser::RtmpParser(rtmp_network::MessageSender_wptr sender,
                       unsigned int connection_id)
    : rtmp_network::RequestParser(sender),
      connection_id_(connection_id),
      parsing_state_(HEADER_PARSING),
      readed_bytes_count_(0),
      win_ack_size_(DEFAULT_WIN_ACK_SIZE),
      next_ack_send_size_(DEFAULT_WIN_ACK_SIZE),
      read_bytes_count_in_window_(0) {
  payload_parser_.set_connection_id(connection_id_);
  header_parser_.set_connection_id(connection_id_);
}

boost::tribool RtmpParser::parse(std::istream& stream, size_t buf_size,
                                 size_t& total_readed_size) {

  boost::tribool result = parse_private(stream, buf_size, total_readed_size);
  if (total_readed_size > 0) {
    readed_bytes_count_ += total_readed_size;

    // send ack
    // 여태까지 받은 byte 의 경우 4byte 를 넘어갈 수 있어서,
    // windows size 내에서 받은 byte 를 보내는 것으로 변경
    if (readed_bytes_count_ >= next_ack_send_size_) {
      next_ack_send_size_ = readed_bytes_count_ + win_ack_size_;
      push_to_send_queue(RtmpMessage_ptr(new Acknowledgement(win_ack_size_)));
      signal_send_message();

      RTMPLOGF(info,
               "connection_id[%1%] "
               "total_read_bytes[%2%],actual_read_bytes_window_size[%3%],"
               "window_size[%4%]",
               connection_id_, readed_bytes_count_, read_bytes_count_in_window_,
               win_ack_size_);
      read_bytes_count_in_window_ = 0;
    } else {
      read_bytes_count_in_window_ += total_readed_size;
    }
  }
  return result;
}

boost::tribool RtmpParser::parse_private(std::istream& stream, size_t buf_size,
                                         size_t& total_readed_size) {
  total_readed_size = 0;

  while (true) {
    switch (parsing_state_) {
      case HEADER_PARSING: {

        if (buf_size == 0) { return boost::indeterminate; }

        size_t readed_size = 0;
        boost::tribool result = header_parser_.parse_header(stream, buf_size,
                                                     readed_size);
        if (result) {
          buf_size -= readed_size;
          total_readed_size += readed_size;

          set_parsed_header(header_parser_.get_parsed_msg());

          RTMPLOG(debug) << "connection_id:" << connection_id_
                              << ",parsed header:"
                              << parsed_header_->to_string();

          parsing_state_ = PAYLOAD_PARSING;
        } else if (result == false) {
          RTMPLOG(error) << "parsing header fail";
          return result;
        } else {
          RTMPLOG(debug) << "continue parsing header: current buf size:" << buf_size ;
          return result;
        }
        
        break;
      }

      case PAYLOAD_PARSING: {

        if (buf_size == 0) { return boost::indeterminate; }

        size_t readed_size = 0;
        RtmpPayloadParseResult::type result = payload_parser_.parse_payload(
            parsed_header_, stream, buf_size, readed_size);

        if (result == RtmpPayloadParseResult::COMPLETE) {
          buf_size -= readed_size;
          total_readed_size += readed_size;

          set_parsed_msg(payload_parser_.get_parsed_msg());

          RTMPLOG(debug) << "connection_id:" << connection_id_
                              << ",parsed payload:"
                              << parsed_msg_->to_string();

          if (parsed_msg_->get_class_name().compare("SetChunkSize") == 0) {
            unsigned int chunk_size = boost::dynamic_pointer_cast<SetChunkSize>(
                parsed_msg_)->chunk_size_;
            payload_parser_.set_recv_chunk_size(chunk_size);
            RTMPLOG(debug) << "connection_id:" << connection_id_
                              << ",change recv chunk size:"
                              << chunk_size;
          } else if (parsed_msg_->get_class_name().compare(
              "WindowAcknowledgementSize") == 0) {

            unsigned int size = boost::dynamic_pointer_cast<
                WindowAcknowledgementSize>(parsed_msg_)->window_size_;
            set_win_ack_size(size);
            RTMPLOG(debug) << "connection_id:" << connection_id_
                              << ",change windows ack size:"
                              << win_ack_size_;
          }
          parsing_state_ = COMPLETE;
        } else if (result == RtmpPayloadParseResult::WAITING_NEXT_CHUNK) {
          buf_size -= readed_size;
          total_readed_size += readed_size;

          //RTMPLOG(debug) << "result : WAITING_NEXT_CHUNK -> parsing state HEADER_PARSING after parsing payload"<< ",read size: "<< readed_size << ",buf_size: " << buf_size << ",total read size: " << total_readed_size;

          parsing_state_ = HEADER_PARSING;
          break;
        } else if (result == RtmpPayloadParseResult::FAIL) {

          RTMPLOG(error) << "connection_id:" << connection_id_ << ",error occurred in parsing payload.";
          return false;
        } else if (result
            == RtmpPayloadParseResult::WAITING_MORE_PAYLOAD_DATA) {

          //RTMPLOG(debug) << "result : RtmpPayloadParseResult::WAITING_MORE_PAYLOAD_DATA -> return indeterminate after parsing payload";

          return boost::indeterminate;
        }
        break;
      }
      case COMPLETE: {
        parsing_state_ = HEADER_PARSING;
        return true;
      }
      default:
        RTMPLOG(error) << "connection_id:" << connection_id_
                           << ",invalid parse state.";
        break;
    }
  }
  return false;
}

void RtmpParser::set_win_ack_size(unsigned int size) {
  next_ack_send_size_ -= win_ack_size_;
  win_ack_size_ = size;
  next_ack_send_size_ += win_ack_size_;
}

rtmp_network::Message_ptr RtmpParser::get_parsed_message() {
  return parsed_msg_;
}

void RtmpParser::reset() {
  parsed_header_.reset();
}

void RtmpParser::set_parsed_msg(RtmpMessage_ptr msg) {
  parsed_msg_.reset();
  parsed_msg_ = msg;
}

void RtmpParser::set_parsed_header(RtmpHeader_ptr header) {
  parsed_header_.reset();
  parsed_header_ = header;
}

void RtmpParser::set_context(castis::streamer::media_publish_es_context_ptr ctx) {
  context_ = ctx;
  payload_parser_.context_ = context_;
}

}  // namespace rtmp_protocol

