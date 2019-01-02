/*
 * HandshakeManager.cpp
 *
 *  Created on: 2012. 12. 26.
 *      Author: alexyun
 */

#include "HandshakeManager.hpp"
#include "../../../src/rtmpmodulelogger.h"
#include "../common/EncryptUtil.hpp"
#include "../common/NetworkUtil.hpp"
#include "../common/StringUtil.hpp"

namespace rtmp_network {

unsigned char HandshakeManager::FMS_key_in_sha256[] = {
    0x47, 0x65, 0x6e, 0x75, 0x69, 0x6e, 0x65, 0x20, 0x41, 0x64, 0x6f, 0x62,
    0x65, 0x20, 0x46, 0x6c, 0x61, 0x73, 0x68, 0x20, 0x4d, 0x65, 0x64, 0x69,
    0x61, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x30, 0x30,
    0x31,  // Genuine Adobe Flash Media Server 001
    0xf0, 0xee, 0xc2, 0x4a, 0x80, 0x68, 0xbe, 0xe8, 0x2e, 0x00, 0xd0, 0xd1,
    0x02, 0x9e, 0x7e, 0x57, 0x6e, 0xec, 0x5d, 0x2d, 0x29, 0x80, 0x6f, 0xab,
    0x93, 0xb8, 0xe6, 0x36, 0xcf, 0xeb, 0x31, 0xae};  // 68

unsigned char HandshakeManager::FP_key_in_sha256[] = {
    0x47, 0x65, 0x6E, 0x75, 0x69, 0x6E, 0x65, 0x20, 0x41, 0x64, 0x6F,
    0x62, 0x65, 0x20, 0x46, 0x6C, 0x61, 0x73, 0x68, 0x20, 0x50, 0x6C,
    0x61, 0x79, 0x65, 0x72, 0x20, 0x30, 0x30,
    0x31,  // Genuine Adobe Flash Player 001
    0xF0, 0xEE, 0xC2, 0x4A, 0x80, 0x68, 0xBE, 0xE8, 0x2E, 0x00, 0xD0,
    0xD1, 0x02, 0x9E, 0x7E, 0x57, 0x6E, 0xEC, 0x5D, 0x2D, 0x29, 0x80,
    0x6F, 0xAB, 0x93, 0xB8, 0xE6, 0x36, 0xCF, 0xEB, 0x31, 0xAE};  // 62

HandshakeManager::HandshakeManager() {}

bool HandshakeManager::validate_C0_C1(std::istream& stream) {
  stream.read(reinterpret_cast<char*>(&client_rtmp_version_),
              HANDSHAKE_MSG_VERSION_LEN);

  switch (client_rtmp_version_) {
    case 3: {
      RTMPLOGF(debug, "handshake C0. rtmp_version[%1%],connection[%2%]",
               static_cast<unsigned>(client_rtmp_version_), connection_id_);
      stream.read(reinterpret_cast<char*>(handshake_C1_), HANDSHAKE_MSG_LENGTH);
      return validate_C1_ver3(handshake_C1_);
    }

    // case 6: // RTMPE
    // case another
    default: {
      RTMPLOGF(debug,
               "invalid handshake C0 value. RTMP version SHOULD be 3. "
               "rtmp_version[%1%],connection[%2%]",
               static_cast<unsigned>(client_rtmp_version_), connection_id_);
      return false;
    }
  }
  return false;
}

// RTMP spec(1.0)에서는 timestamp 값(4byte) 다음 값(4 byte )은 0 이어야 한다.
// 하지만 이 함수에서는 handshake version 값이 들어있다고 되어있다.
// 이 값이 0 인 경우는 RTMP spec에 나온 것과 같고 아무일도 하지 않으므로 그냥
// 두어도 될 것 같고,, 이 값이 0 이 아닌 경우는  RTMP spec(1.0) 에 없는
// 내용이다.
bool HandshakeManager::validate_C1_ver3(const unsigned char* C1_buf) {
  RTMPLOGF(debug, "C1_data[%1%], C1_random_data[%2%],connection[%3%]",
           string_util::to_hex(&(C1_buf[0]), 16),
           string_util::to_hex(&(C1_buf[8]), 16), connection_id_);

  // implicitly big endian to little endian
  memcpy(&client_epoch_timestamp_, C1_buf, 4);
  // implicitly big endian to little endian
  memcpy(&client_handshake_version_, C1_buf + 4, 4);
  // revert it. because version is not a number
  client_handshake_version_ = network_util::hton_4(client_handshake_version_);

  server_client_epoch_read_timestamp_ = (uint32_t)time(NULL);

  // check C1 validity
  if (client_handshake_version_ == 0) {
    RTMPLOGF(
        debug,
        "no need C1 validation. "
        "client_epoch_timestamp[%1%],client_version[%2%],connection[%3%]",
        client_epoch_timestamp_, string_util::to_hex(client_handshake_version_),
        connection_id_);

    return true;
  } else {
    RTMPLOGF(
        debug,
        "validate C1. "
        "client_epoch_timestamp[%1%],client_version[%2%],connection[%3%]",
        client_epoch_timestamp_, string_util::to_hex(client_handshake_version_),
        connection_id_);
  }

  if (validate_client_scheme(C1_buf, DIGEST_MODE_0))
    digest_scheme_ = DIGEST_MODE_0;
  else if (validate_client_scheme(C1_buf, DIGEST_MODE_1))
    digest_scheme_ = DIGEST_MODE_1;
  else {
    RTMPLOGF(
        debug,
        "process no digest mode. "
        "client_epoch_timestamp[%1%],handshake_version[%2%],connection[%3%]",
        client_epoch_timestamp_, string_util::to_hex(client_handshake_version_),
        connection_id_);
    digest_scheme_ = NO_DIGEST_MODE;
    return true;
  }

  return true;
}

bool HandshakeManager::validate_client_scheme(const unsigned char* buf,
                                              unsigned char scheme) {
  unsigned int digest_offset = get_digest_offset(buf, scheme);

  unsigned char* pTempBuffer = new unsigned char[HANDSHAKE_MSG_LENGTH - 32];
  memcpy(pTempBuffer, buf, digest_offset);
  memcpy(pTempBuffer + digest_offset, buf + digest_offset + 32,
         HANDSHAKE_MSG_LENGTH - digest_offset - 32);

  unsigned char* pTempHash = new unsigned char[512];
  encrypt_util::HMACSha256_util sha256_util;
  sha256_util.encrypt(pTempBuffer, HANDSHAKE_MSG_LENGTH - 32, FP_key_in_sha256,
                      30, pTempHash);

  bool result = true;
  for (unsigned int i = 0; i < 32; i++) {
    if (buf[digest_offset + i] != pTempHash[i]) {
      result = false;
      break;
    }
  }

  delete[] pTempBuffer;
  delete[] pTempHash;

  return result;
}

unsigned int HandshakeManager::get_digest_offset_0(const unsigned char* buf) {
  unsigned int offset = buf[8] + buf[9] + buf[10] + buf[11];
  offset = offset % 728;
  offset = offset + 12;
  if (offset + 32 >= HANDSHAKE_MSG_LENGTH) {
    RTMPLOGF(debug,
             "while validating C1, digest offset is invalid. "
             "handshakeversion[%1%],connection[%2%]",
             client_handshake_version_, connection_id_);
  }
  return offset;
}

unsigned int HandshakeManager::get_digest_offset_1(const unsigned char* buf) {
  unsigned int offset = buf[772] + buf[773] + buf[774] + buf[775];
  offset = offset % 728;
  offset = offset + 776;
  if (offset + 32 >= HANDSHAKE_MSG_LENGTH) {
    RTMPLOGF(debug,
             "while validating C1, digest offset is invalid. "
             "handshakeversion[%1%],connection[%2%]",
             client_handshake_version_, connection_id_);
  }
  return offset;
}

unsigned int HandshakeManager::get_digest_offset(const unsigned char* buf,
                                                 unsigned char scheme) {
  switch (scheme) {
    case DIGEST_MODE_0: {
      return get_digest_offset_0(buf);
    }
    case DIGEST_MODE_1: {
      return get_digest_offset_1(buf);
    }
    default: {
      RTMPLOGF(debug,
               "while validating C1, invalid scheme[%1%]. process as if scheme "
               "is 0.  handshakeversion[%2%],connection[%3%]",
               scheme, client_handshake_version_, connection_id_);
      return get_digest_offset_0(buf);
    }
  }
}

// S1 : Time1 Zero RandomData1
// S2 : Time2 Time3 RandomData2
void HandshakeManager::get_S0_S1_S2(std::ostream& stream) {
  unsigned int timestamp =
      network_util::ntoh_4((unsigned int)time(NULL));  // Time1 field
  unsigned int server_version = 0;                     // Zero field

  memcpy(handshake_S1_S2_, &timestamp, sizeof(unsigned int));
  memcpy(handshake_S1_S2_ + 4, &server_version, sizeof(unsigned int));

  for (int i = 8; i < HANDSHAKE_MSG_LENGTH * 2; i++) {
    handshake_S1_S2_[i] = rand() % 256;
  }

  // generate S1
  // generate the digest
  unsigned int server_digest_offset =
      get_digest_offset(handshake_S1_S2_, digest_scheme_);

  unsigned char* pTempBuffer = new unsigned char[HANDSHAKE_MSG_LENGTH - 32];
  memcpy(pTempBuffer, handshake_S1_S2_, server_digest_offset);
  memcpy(pTempBuffer + server_digest_offset,
         handshake_S1_S2_ + server_digest_offset + 32,
         HANDSHAKE_MSG_LENGTH - server_digest_offset - 32);

  unsigned char* pTempHash = new unsigned char[512];
  encrypt_util::HMACSha256_util sha256_util;
  sha256_util.encrypt(pTempBuffer, HANDSHAKE_MSG_LENGTH - 32, FMS_key_in_sha256,
                      36, pTempHash);

  // put the digest in place
  memcpy(handshake_S1_S2_ + server_digest_offset, pTempHash, 32);

  // cleanup
  delete[] pTempBuffer;
  delete[] pTempHash;

  // generate S2
  // Compute the chalange index from the initial client request
  unsigned int keyChallengeIndex =
      get_digest_offset(handshake_C1_, digest_scheme_);

  // compute the key
  pTempHash = new unsigned char[512];
  sha256_util.encrypt(handshake_C1_ + keyChallengeIndex, 32, FMS_key_in_sha256,
                      68, pTempHash);

  // generate the hash
  unsigned char* pLastHash = new unsigned char[512];
  sha256_util.encrypt(handshake_S1_S2_ + HANDSHAKE_MSG_LENGTH,
                      HANDSHAKE_MSG_LENGTH - 32, pTempHash, 32, pLastHash);

  // put the hash where it belongs
  memcpy(handshake_S1_S2_ + HANDSHAKE_MSG_LENGTH * 2 - 32, pLastHash, 32);

  // cleanup
  delete[] pTempHash;
  delete[] pLastHash;

  unsigned char rtmp_protocol_version = 3;
  stream.write(reinterpret_cast<const char*>(&rtmp_protocol_version),
               sizeof(rtmp_protocol_version));
  stream.write((const char*)(handshake_S1_S2_), HANDSHAKE_MSG_LENGTH * 2);
}


// S1의 time 과 S2 의 time2 는 서버 epoch 를 보내면 되지만,
// client 의 C1 time 을 기준으로 바꾸어서 보내주게 구현함
// FIXME:
// use stream directly
// handshake_s1_s2_ 는 없애는 방법도 있을 것 같음.
void HandshakeManager::write_S0_S1_S2(std::ostream& stream) {
  server_epoch_timestamp_ = (uint32_t)time(NULL);
  // S1  
  //uint32_t s1_time = server_epoch_timestamp_;
  uint32_t s1_time = client_epoch_timestamp_;
  // memcopy do implicitly little endian to big endian
  memcpy(handshake_S1_S2_, &s1_time, 4);
  memcpy(handshake_S1_S2_ + 4, &server_handshake_version_, 4);
  for (int i = 8; i < HANDSHAKE_MSG_LENGTH; ++i) {
    handshake_S1_S2_[i] = rand() % 256;
  }

  // S2
  uint32_t s2_time = client_epoch_timestamp_;
  //uint32_t s2_stime2 = server_client_epoch_read_timestamp_;
  uint32_t s2_time2 = client_epoch_timestamp_ + (server_epoch_timestamp_ - server_client_epoch_read_timestamp_);

  // memcopy do implicitly little endian to big endian
  memcpy(handshake_S1_S2_ + HANDSHAKE_MSG_LENGTH, &s2_time, 4);
  memcpy(handshake_S1_S2_ + HANDSHAKE_MSG_LENGTH + 4, &s2_time2, 4);
  for (int i = 8; i < HANDSHAKE_MSG_LENGTH; ++i) {
    handshake_S1_S2_[HANDSHAKE_MSG_LENGTH + i] = handshake_C1_[i];
  }

  RTMPLOGF(debug, "write S0. server_rtmp_version[%1%],connection[%2%]",
           static_cast<unsigned>(server_rtmp_version_), connection_id_);
  RTMPLOGF(debug,
           "write S1. "
           "server_epoch_timestamp[%1%],handshake_version[%2%],connection[%3%]",
           string_util::to_hex(&(handshake_S1_S2_[0]), 4),
           string_util::to_hex(&(handshake_S1_S2_[4]), 4), connection_id_);
  RTMPLOGF(
      debug,
      "write S2. "
      "client_epoch_timestamp[%1%],server_client_epoch_read_timestamp[%2%],"
      "connection[%3%]",
      string_util::to_hex(&(handshake_S1_S2_[HANDSHAKE_MSG_LENGTH]), 4),
      string_util::to_hex(&(handshake_S1_S2_[HANDSHAKE_MSG_LENGTH + 4]), 4),
      connection_id_);
  RTMPLOGF(
      debug, "write S2. random_echo[%1%],C1_random_data[%2%],connection[%3%]",
      string_util::to_hex(&(handshake_S1_S2_[HANDSHAKE_MSG_LENGTH + 8]), 16),
      string_util::to_hex(&(handshake_C1_[8]), 16), connection_id_);

  stream.write(reinterpret_cast<const char*>(&server_rtmp_version_),
               HANDSHAKE_MSG_VERSION_LEN);
  stream.write((const char*)(handshake_S1_S2_), HANDSHAKE_MSG_LENGTH * 2);
}

}  // namespace rtmp_network
