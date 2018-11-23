#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "Connection.hpp"
#include "RequestHandler.hpp"
#include "RequestParser.hpp"

namespace rtmp_network {

class server : private boost::noncopyable {
 public:
  /// Construct the server to listen on the specified TCP address and port
  explicit server(const std::string& address, const std::string& port,
                  std::size_t thread_pool_size,
                  RequestHandlerFactory_ptr handler_factory,
                  RequestParserFactory_ptr parser_factory);

  /// Run the server's io_service loop.
  void run();

 private:
  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  /// The number of threads that will call io_service::run().
  std::size_t thread_pool_size_;

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  Connection_ptr new_connection_;

  RequestHandlerFactory_ptr handler_factory_;
  RequestParserFactory_ptr parser_factory_;

  unsigned int new_connection_id_;
};

}  // namespace rtmp_network

#endif // SERVER_SERVER_HPP
