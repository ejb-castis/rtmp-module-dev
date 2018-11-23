#ifndef HANDSHAKEMANAGER_HPP_
#define HANDSHAKEMANAGER_HPP_

#include <boost/asio.hpp>

namespace rtmp_network {

#define HANDSHAKE_MSG_VERSION_LEN 1
#define HANDSHAKE_MSG_LENGTH 1536

class HandshakeManager {
 private:
  static unsigned char FMS_key_in_sha256[];
  static unsigned char FP_key_in_sha256[];

  unsigned int connection_id_;

  unsigned char handshake_C1_[HANDSHAKE_MSG_LENGTH];
  unsigned char handshake_S1_S2_[HANDSHAKE_MSG_LENGTH * 2];

  unsigned char digest_scheme_;

  bool validate_C1_ver3(const unsigned char* C1_buf);
  bool validate_client_scheme(const unsigned char* buf, unsigned char scheme);
  unsigned int get_digest_offset(const unsigned char* buf,
                                 unsigned char scheme);
  unsigned int get_digest_offset_0(const unsigned char* buf);
  unsigned int get_digest_offset_1(const unsigned char* buf);

 public:
  HandshakeManager();
  bool validate_C0_C1(std::istream& C0_C1_stream);
  void get_S0_S1_S2(std::ostream& S0_S1_S2_stream);
  void set_connection_id(unsigned int id) {
    connection_id_ = id;
  }
};

}  // namespace rtmp_network

#endif /* HANDSHAKEMANAGER_HPP_ */
