/*
 * HandshakeManager.cpp
 *
 *  Created on: 2012. 12. 26.
 *      Author: alexyun
 */

#include "HandshakeManager.hpp"
#include "../common/EncryptUtil.hpp"
#include "../common/NetworkUtil.hpp"
#include "../../../src/rtmpmodulelogger.h"

namespace rtmp_network {

unsigned char HandshakeManager::FMS_key_in_sha256[] = { 0x47, 0x65, 0x6e, 0x75,
    0x69, 0x6e, 0x65, 0x20, 0x41, 0x64, 0x6f, 0x62, 0x65, 0x20, 0x46, 0x6c,
    0x61, 0x73, 0x68, 0x20, 0x4d, 0x65, 0x64, 0x69, 0x61, 0x20, 0x53, 0x65,
    0x72, 0x76, 0x65, 0x72, 0x20, 0x30, 0x30,
    0x31,  // Genuine Adobe Flash Media Server 001
    0xf0, 0xee, 0xc2, 0x4a, 0x80, 0x68, 0xbe, 0xe8, 0x2e, 0x00, 0xd0, 0xd1,
    0x02, 0x9e, 0x7e, 0x57, 0x6e, 0xec, 0x5d, 0x2d, 0x29, 0x80, 0x6f, 0xab,
    0x93, 0xb8, 0xe6, 0x36, 0xcf, 0xeb, 0x31, 0xae };  // 68

unsigned char HandshakeManager::FP_key_in_sha256[] = { 0x47, 0x65, 0x6E, 0x75,
    0x69, 0x6E, 0x65, 0x20, 0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x46, 0x6C,
    0x61, 0x73, 0x68, 0x20, 0x50, 0x6C, 0x61, 0x79, 0x65, 0x72, 0x20, 0x30,
    0x30,
    0x31,  // Genuine Adobe Flash Player 001
    0xF0, 0xEE, 0xC2, 0x4A, 0x80, 0x68, 0xBE, 0xE8, 0x2E, 0x00, 0xD0, 0xD1,
    0x02, 0x9E, 0x7E, 0x57, 0x6E, 0xEC, 0x5D, 0x2D, 0x29, 0x80, 0x6F, 0xAB,
    0x93, 0xB8, 0xE6, 0x36, 0xCF, 0xEB, 0x31, 0xAE };  // 62

HandshakeManager::HandshakeManager()
    : digest_scheme_(0) {
}

bool HandshakeManager::validate_C0_C1(std::istream& stream) {
  unsigned char protocol_version = 0;
  stream.read(reinterpret_cast<char*>(&protocol_version), sizeof(protocol_version));

  switch (protocol_version) {
    case 3:
      stream.read(reinterpret_cast<char*>(handshake_C1_), HANDSHAKE_MSG_LENGTH);

      return validate_C1_ver3(handshake_C1_);
      // case 6: // RTMPE
      // case another 
    default:
      RTMPLOG(error) << "HandshakeManager[" << connection_id_ << "]:invalid handshake C1 protocl version ("
                         << (unsigned int) protocol_version << ")";
      return false;
  }
  return false;
}
bool HandshakeManager::validate_C1_ver3(const unsigned char* C1_buf) {
  unsigned int flashPlayerVersion = 0;
  memcpy(&flashPlayerVersion, C1_buf + 4, sizeof(unsigned int));
  flashPlayerVersion = network_util::ntoh_4(flashPlayerVersion);

  // check C1 validity
  if (flashPlayerVersion == 0) {
    RTMPLOG(error)
        << "HandshakeManager[" << connection_id_ << "]:no need C1 validity (flash player version is 0)";
    return true;
  }

  if (validate_client_scheme(C1_buf, 0))
    digest_scheme_ = 0;
  else if (validate_client_scheme(C1_buf, 1))
    digest_scheme_ = 1;
  else {
    RTMPLOG(error) << "HandshakeManager[" << connection_id_ << "]:handshake C1 is not valid.";
    return false;
  }

  return true;
}

bool HandshakeManager::validate_client_scheme(const unsigned char* buf,
                                              unsigned char scheme) {
  unsigned int digest_offset = get_digest_offset(buf, scheme);

  unsigned char *pTempBuffer = new unsigned char[HANDSHAKE_MSG_LENGTH - 32];
  memcpy(pTempBuffer, buf, digest_offset);
  memcpy(pTempBuffer + digest_offset, buf + digest_offset + 32,
         HANDSHAKE_MSG_LENGTH - digest_offset - 32);

  unsigned char *pTempHash = new unsigned char[512];
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
    RTMPLOG(error) << "HandshakeManager[" << connection_id_ << "]:invalid digest offset.";
  }
  return offset;
}

unsigned int HandshakeManager::get_digest_offset_1(const unsigned char* buf) {
  unsigned int offset = buf[772] + buf[773] + buf[774] + buf[775];
  offset = offset % 728;
  offset = offset + 776;
  if (offset + 32 >= HANDSHAKE_MSG_LENGTH) {
    RTMPLOG(error) << "HandshakeManager[" << connection_id_ << "]:invalid digest offset.";
  }
  return offset;
}

unsigned int HandshakeManager::get_digest_offset(const unsigned char* buf,
                                                 unsigned char scheme) {
  switch (scheme) {
    case 0: {
      return get_digest_offset_0(buf);
    }
    case 1: {
      return get_digest_offset_1(buf);
    }
    default: {
      RTMPLOG(error) << "HandshakeManager[" << connection_id_ << "]:invalid scheme number: " << scheme
                         << ". Defaulting to 0";
      return get_digest_offset_0(buf);
    }
  }
}

// S1 : Time1 Zero RandomData1
// S2 : Time2 Time3 RandomData2
// FIXME: 
// ? MUST S2 field be contain the randomData of C1 in Spec
// ? SHOLD function change name get_xxxx -> put_ or write_ 
void HandshakeManager::get_S0_S1_S2(std::ostream& stream) {
  unsigned int timestamp = network_util::ntoh_4((unsigned int) time(NULL)); // Time1 field
  unsigned int server_version = 0; // Zero field

  memcpy(handshake_S1_S2_, &timestamp, sizeof(unsigned int));
  memcpy(handshake_S1_S2_ + 4, &server_version, sizeof(unsigned int));

  for (int i = 8; i < 3072; i++) {
    handshake_S1_S2_[i] = rand() % 256;
  }

  // generate S1
  //generate the digest
  unsigned int server_digest_offset = get_digest_offset(handshake_S1_S2_,
                                                        digest_scheme_);

  unsigned char *pTempBuffer = new unsigned char[HANDSHAKE_MSG_LENGTH - 32];
  memcpy(pTempBuffer, handshake_S1_S2_, server_digest_offset);
  memcpy(pTempBuffer + server_digest_offset,
         handshake_S1_S2_ + server_digest_offset + 32,
         HANDSHAKE_MSG_LENGTH - server_digest_offset - 32);

  unsigned char *pTempHash = new unsigned char[512];
  encrypt_util::HMACSha256_util sha256_util;
  sha256_util.encrypt(pTempBuffer, HANDSHAKE_MSG_LENGTH - 32, FMS_key_in_sha256,
                      36, pTempHash);

  //put the digest in place
  memcpy(handshake_S1_S2_ + server_digest_offset, pTempHash, 32);

  //cleanup
  delete[] pTempBuffer;
  delete[] pTempHash;

  // generate S2
  //Compute the chalange index from the initial client request
  unsigned int keyChallengeIndex = get_digest_offset(handshake_C1_,
                                                     digest_scheme_);

  //compute the key
  pTempHash = new unsigned char[512];
  sha256_util.encrypt(handshake_C1_ + keyChallengeIndex, 32, FMS_key_in_sha256,
                      68, pTempHash);

  //generate the hash
  unsigned char *pLastHash = new unsigned char[512];
  sha256_util.encrypt(handshake_S1_S2_ + HANDSHAKE_MSG_LENGTH,
                      HANDSHAKE_MSG_LENGTH - 32, pTempHash, 32, pLastHash);

  //put the hash where it belongs
  memcpy(handshake_S1_S2_ + HANDSHAKE_MSG_LENGTH * 2 - 32, pLastHash, 32);

  //cleanup
  delete[] pTempHash;
  delete[] pLastHash;

  unsigned char rtmp_protocol_version = 3;
  stream.write(reinterpret_cast<const char*>(&rtmp_protocol_version),
               sizeof(rtmp_protocol_version));
  stream.write((const char*) (handshake_S1_S2_), HANDSHAKE_MSG_LENGTH * 2);
}

}  // namespace rtmp_network
