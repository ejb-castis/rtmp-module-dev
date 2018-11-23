#ifndef ENCRYPTUTIL_HPP_
#define ENCRYPTUTIL_HPP_

namespace encrypt_util {
class HMACSha256_util {
 public:
  void encrypt(const unsigned char *text, /* pointer to data stream        */
               int text_len,                                      /* length of data stream         */
               const unsigned char *key,            /* pointer to authentication key */
               int key_len,                                       /* length of authentication key  */
               unsigned char *digest                    /* caller digest to be filled in */
               );
};

}  // namespace encrypt_util

#endif /* ENCRYPTUTIL_HPP_ */
