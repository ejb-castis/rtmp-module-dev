#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <queue>

#include "HandshakeManager.hpp"
#include "MessageSender.hpp"
#include "Message.hpp"
#include "RequestParser.hpp"
#include "RequestHandler.hpp"
#include "../common/ConcurrentQueue.hpp"

namespace rtmp_network {

/// Represents a single connection from a client.
class Connection : public boost::enable_shared_from_this<Connection>,
    private boost::noncopyable, MessageSender {
 public:
  /// Construct a connection with the given io_service.
  explicit Connection(boost::asio::io_service& io_service,
                      RequestHandlerFactory_ptr handler_factory,
                      RequestParserFactory_ptr parser_factory,
                      unsigned int id);
  virtual ~Connection();

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

  unsigned int get_message_sender_id();

 private:
  void handle_read_handshake_C0_C1(const boost::system::error_code& e);
  void handle_S0_S1_S2_write(const boost::system::error_code& e);
  void handle_read_handshake_C2(const boost::system::error_code& e);

  void handle_read(const boost::system::error_code& e,
                   std::size_t bytes_transferred);
  void disconnect();
  void push_to_send_queue(Message_ptr message);
  void signal_send_message();
  void write_message();

  void handle_write(const boost::system::error_code& e);
  void change_continuous_send_state(bool state);
  void handle_send_timer(const boost::system::error_code& e);

  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// streambuf for request data.
  boost::asio::streambuf request_streambuf_;

  /// streambuf for reply data.
  boost::asio::streambuf reply_streambuf_;

  common::concurrent_queue<Message_ptr> write_queue_;

  bool is_continuous_send_state_;
  boost::asio::deadline_timer continuous_send_timer_;

  HandshakeManager handshake_manager_;
  RequestHandler_ptr handler_;
  RequestParser_ptr parser_;

  unsigned int id_;
};

typedef boost::shared_ptr<Connection> Connection_ptr;

}  // namespace rtmp_network

#endif // SERVER_CONNECTION_HPP
