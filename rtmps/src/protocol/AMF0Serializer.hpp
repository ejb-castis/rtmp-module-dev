#ifndef AMF0SERIALIZER_HPP_
#define AMF0SERIALIZER_HPP_

#include <vector>
#include <string>
#include <iostream>

namespace rtmp_protocol {
class AMF0Type {
 public:
  typedef enum type {
    AMF0_NUMBER = 0x00, // number (double precision)
    AMF0_BOOLEAN = 0x01, // boolean value
    AMF0_SHORT_STRING = 0x02, // AMF_STRING : Pascal-style string with length < 65536
    AMF0_OBJECT = 0x03, // AMF object, contains property names and values
    AMF0_MOVIE, // Flash object, reserved
    AMF0_NULL = 0x05, // NULL value
    AMF0_UNDEFINED = 0x06, // undefined (return?) value
    AMF0_REFERENCE, // reference
    AMF0_MIXED_ARRAY = 0x08, // or AMF_ECMA_ARRAY : ECMA array, almost like AMF object but has number of entries
    AMF0_OBJECT_END, // marker for end of AMF object or ECMA array
    AMF0_ARRAY = 0x0a, // or AMF_STRICT_ARRAY : strict array
    AMF0_TIMESTAMP = 0x0b, // or AMF_DATE : date
    AMF0_LONG_STRING = 0x0c, // Pascal-style string with possible length up to 4GB
    AMF0_UNSUPPORTED, // unsipported feature indicator
    AMF0_RECORD_SET, // record set, reserved
    AMF_XML_OBJECT, // XML object
    AMF0_TYPED_OBJECT = 0x10,
    AMF0_AMF3_OBJECT = 0x11 // avmplus-obect for switching to AMF3
  } type;
};

struct AMFObjectProperty;
struct AMFObject
{
  int num;
  std::vector<AMFObjectProperty> object_props;
};
struct AMFObjectProperty
{
  std::string name;
  AMF0Type::type type;
  union
  {
    double number;
    std::string aval;
    AMFObject object;
  } value;
  int16_t p_UTCoffset;
};


class AMF0Serializer {
 private:
  static std::vector<std::string> keys_;
  static unsigned char end_of_object_[];

 public:
  AMF0Serializer();
  virtual ~AMF0Serializer();

  bool read_type(std::istream &stream, /*output*/AMF0Type::type &type_value);
  bool write_type(std::ostream &stream, AMF0Type::type type_value, size_t& writed);

  bool read_short_string(std::istream &stream, /*output*/
                         std::string &value, bool reads_type);
  bool write_short_string(std::ostream &stream, std::string &value,
                          bool writes_type, size_t& writed);

  bool read_number_4byte(std::istream &stream, unsigned int &value);

  bool read_number(std::istream &stream, /*output*/
                   double &value, bool reads_type);

  bool write_number(std::ostream &stream, double value, bool writes_type, size_t& writed);

  bool read_boolean(std::istream &stream, /*output*/
                    bool &value, bool reads_type);
  bool write_boolean(std::ostream &stream, bool value, bool writes_type, size_t& writed);

  bool is_end_of_object(std::istream &stream, /*output*/
                        bool& is_not_sufficent_stream_length);
  bool write_end_of_object(std::ostream &stream, size_t& writed);

  bool write_object_key(std::ostream &stream, std::string &value, size_t& writed);

  bool read_null(std::istream &stream);
  bool write_null(std::ostream &stream, size_t& writed);

  bool consume_any(std::istream &stream);
  bool consume_any_property(std::istream &stream);
  bool consume_full(std::istream &stream);

  bool read_object(std::istream &stream);
  bool read_mixed_array(std::istream &stream, AMFObject& object);
  void consume_until_end_of_object(std::istream &stream);
};

}

#endif /* AMF0SERIALIZER_HPP_ */
