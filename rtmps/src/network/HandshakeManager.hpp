#ifndef HANDSHAKEMANAGER_HPP_
#define HANDSHAKEMANAGER_HPP_

#include <boost/asio.hpp>

namespace rtmp_network {

const unsigned HANDSHAKE_MSG_VERSION_LEN =1;
const unsigned HANDSHAKE_MSG_LENGTH =1536;
const unsigned HANDSHAKE_VERSION_LENGTH=4;
enum { 
  DIGEST_MODE_0 = 0,
  DIGEST_MODE_1 = 1,
  NO_DIGEST_MODE = 99
};

class HandshakeManager {
 public:
  static unsigned char FMS_key_in_sha256[];
  static unsigned char FP_key_in_sha256[];

  unsigned int connection_id_;
  uint8_t client_rtmp_version_{0};
  uint32_t client_epoch_timestamp_{0};
  unsigned char client_handshake_version_[HANDSHAKE_VERSION_LENGTH];
  
  uint8_t server_rtmp_version_{3};
  uint32_t server_epoch_timestamp_{0};
  uint32_t server_client_epoch_read_timestamp_{0};
  uint32_t server_handshake_version_{0};

  unsigned char handshake_C1_[HANDSHAKE_MSG_LENGTH];
  unsigned char handshake_S1_S2_[HANDSHAKE_MSG_LENGTH * 2];

  unsigned char digest_scheme_{NO_DIGEST_MODE};

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
  void write_S0_S1_S2(std::ostream& S0_S1_S2_stream);

};

}  // namespace rtmp_network

#endif /* HANDSHAKEMANAGER_HPP_ */
