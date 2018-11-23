#include "AMF0Serializer.hpp"
#include "../common/StreamUtil.hpp"
#include "../../../src/rtmpmodulelogger.h"

using namespace rtmp_protocol;

std::vector<std::string> AMF0Serializer::keys_;
unsigned char AMF0Serializer::end_of_object_[] = {0, 0, 9};

AMF0Serializer::AMF0Serializer() {
  if (keys_.size() <= 0) {
    keys_.push_back("app");
    keys_.push_back("flashVer");
    keys_.push_back("fmsVer");
    keys_.push_back("swfUrl");
    keys_.push_back("tcUrl");
    keys_.push_back("fpad");
    keys_.push_back("capabilities");
    keys_.push_back("audioCodecs");
    keys_.push_back("videoCodecs");
    keys_.push_back("videoFunction");
    keys_.push_back("pageUrl");

    keys_.push_back("level");
    keys_.push_back("code");
    keys_.push_back("description");
    keys_.push_back("details");
    keys_.push_back("clientid");
    keys_.push_back("duration");
    keys_.push_back("width");
    keys_.push_back("height");
    keys_.push_back("videorate");
    keys_.push_back("framerate");
    keys_.push_back("videocodecid");
    keys_.push_back("audiorate");
    keys_.push_back("audiodelay");
    keys_.push_back("audiocodecid");
    keys_.push_back("canSeekToEnd");
    keys_.push_back("creationdate");
  }
}

AMF0Serializer::~AMF0Serializer() {}

bool AMF0Serializer::read_type(std::istream &stream, /*output*/
                               AMF0Type::type &type_value) {
  if (!stream_util::check_redable_length(stream, 1)) return false;

  unsigned char readed;
  stream.read(reinterpret_cast<char *>(&readed), 1);
  type_value = static_cast<AMF0Type::type>(readed);
  return true;
}

bool AMF0Serializer::write_type(std::ostream &stream, AMF0Type::type type_value,
                                size_t &writed) {
  char char_value = static_cast<char>(type_value);
  stream.write(reinterpret_cast<char *>(&char_value), 1);
  ++writed;
  return true;
}

bool AMF0Serializer::read_short_string(std::istream &stream, /*output*/
                                       std::string &value, bool reads_type) {
  if (reads_type) {
    if (!stream_util::check_redable_length(stream, 1)) return false;

    AMF0Type::type data_type;
    if (!read_type(stream, data_type)) {
      return false;
    }

    if (data_type != AMF0Type::AMF0_SHORT_STRING) return false;
  }

  if (!stream_util::check_redable_length(stream, 2)) return false;

  short str_len;
  stream_util::read_short(stream, str_len, true);

  if (!stream_util::check_redable_length(stream, str_len)) return false;

  char buf[str_len];
  stream.read(buf, str_len);

  std::string read_str(buf, str_len);
  value = read_str;

  return true;
}

bool AMF0Serializer::write_short_string(std::ostream &stream,
                                        std::string &value, bool writes_type,
                                        size_t &writed) {
  if (writes_type) {
    write_type(stream, AMF0Type::AMF0_SHORT_STRING, writed);
  }
  stream_util::write_short(stream, value.length(), true, writed);
  stream.write(value.c_str(), value.length());

  writed = writed + value.length();
  return true;
}

bool AMF0Serializer::read_number_4byte(std::istream &stream, unsigned int &value) {
  if (!stream_util::check_redable_length(stream, 4)) return false;
  stream_util::read_uint(stream, value, true);
  return true;
}

bool AMF0Serializer::read_number(std::istream &stream, /*output*/
                                 double &value, bool reads_type) {
  if (reads_type) {
    if (!stream_util::check_redable_length(stream, 1)) return false;

    AMF0Type::type data_type;
    if (!read_type(stream, data_type)) {
      return false;
    }

    if (data_type != AMF0Type::AMF0_NUMBER) return false;
  }

  if (!stream_util::check_redable_length(stream, 8)) return false;

  stream_util::read_double(stream, value, true);
  return true;
}

bool AMF0Serializer::write_number(std::ostream &stream, double value,
                                  bool writes_type, size_t &writed) {
  if (writes_type) {
    write_type(stream, AMF0Type::AMF0_NUMBER, writed);
  }
  stream_util::write_double(stream, value, true, writed);
  return true;
}

bool AMF0Serializer::read_boolean(std::istream &stream, /*output*/
                                  bool &value, bool reads_type) {
  if (reads_type) {
    if (!stream_util::check_redable_length(stream, 1)) return false;

    AMF0Type::type data_type;
    if (!read_type(stream, data_type)) {
      return false;
    }

    if (data_type != AMF0Type::AMF0_BOOLEAN) return false;
  }

  if (!stream_util::check_redable_length(stream, 1)) return false;

  char readed;
  stream_util::read_char(stream, readed);

  if (!(readed == 1 || readed == 0)) {
    return false;
  }

  value = (readed == 1) ? true : false;
  return true;
}

bool AMF0Serializer::write_boolean(std::ostream &stream, bool value,
                                   bool writes_type, size_t &writed) {
  if (writes_type) {
    write_type(stream, AMF0Type::AMF0_BOOLEAN, writed);
  }
  stream.write(reinterpret_cast<char *>(&value), 1);
  ++writed;
  return true;
}

bool AMF0Serializer::is_end_of_object(std::istream &stream,
                                      bool &is_not_sufficent_stream_length) {
  is_not_sufficent_stream_length = false;

  if (!stream_util::check_redable_length(stream, 3)) {
    is_not_sufficent_stream_length = true;
    return false;
  }

  char buf[3];
  stream.read(buf, 3);

  if (buf[0] == 0 && buf[1] == 0 && buf[2] == 9) return true;

  stream_util::change_relative_position_istream(stream, -3);
  return false;
}

bool AMF0Serializer::write_end_of_object(std::ostream &stream, size_t &writed) {
  stream.write(reinterpret_cast<const char *>(AMF0Serializer::end_of_object_),
               sizeof(AMF0Serializer::end_of_object_));

  writed = writed + sizeof(AMF0Serializer::end_of_object_);
  return true;
}

bool AMF0Serializer::write_object_key(std::ostream &stream, std::string &value,
                                      size_t &writed) {
  return write_short_string(stream, value, false, writed);
}

bool AMF0Serializer::read_null(std::istream &stream) {
  if (!stream_util::check_redable_length(stream, 1)) return false;

  AMF0Type::type data_type;
  if (!read_type(stream, data_type)) {
    return false;
  }

  if (data_type != AMF0Type::AMF0_NULL) return false;

  return true;
}

bool AMF0Serializer::write_null(std::ostream &stream, size_t &writed) {
  write_type(stream, AMF0Type::AMF0_NULL, writed);
  return true;
}

// FIXME: 
// AMF0_MIXED_ARRAY

bool AMF0Serializer::consume_any(std::istream &stream) {
  AMF0Type::type data_type;
  if (!read_type(stream, data_type)) {
    RTMPLOG(error) << "amf:object parsing fails" <<",amf:"<<data_type;
    return false;
  }
  RTMPLOG(debug) << "amf_type:" << data_type;
  switch (data_type) {
    case AMF0Type::AMF0_BOOLEAN: {
      bool temp;
      bool ret = read_boolean(stream, temp, false);
      RTMPLOG(debug) << "amf_bool_value:" << temp;
      return ret;
    }
    case AMF0Type::AMF0_NULL: {
      RTMPLOG(debug) << "amf_null_value:";
      return true;
    }
    case AMF0Type::AMF0_NUMBER: {
      double temp;
      bool ret = read_number(stream, temp, false);
      RTMPLOG(debug) << "amf_number_value:" << temp;
      return ret;
    }
    case AMF0Type::AMF0_SHORT_STRING: {
      std::string temp;
      bool ret = read_short_string(stream, temp, false);
      RTMPLOG(debug) << "amf_string_value:" << temp;
      return ret;
    }
    case AMF0Type::AMF0_UNDEFINED: {
      RTMPLOG(debug) << "amf_undefined:";
      return true;
    }
    case AMF0Type::AMF0_OBJECT: {
      while (true) {
        bool is_not_sufficent_stream_length;
        bool result = is_end_of_object(stream, is_not_sufficent_stream_length);

        if (is_not_sufficent_stream_length) {
          RTMPLOG(error) << "amf_object parsing fails";
          return false;
        }
        if (result) {
          RTMPLOG(debug) << "amf_end_of_object:" << end_of_object_;
          return true;
        }  
        consume_any_property(stream);
      }
      break;
    }
    case AMF0Type::AMF0_MIXED_ARRAY: {
      unsigned int array_size;
      bool ret = read_number_4byte(stream, array_size);
      RTMPLOG(debug) << "amf_mixed_array_size:" << array_size;

      // read AMF0_OBJECT      
      while (true) {
        bool is_not_sufficent_stream_length;
        bool result = is_end_of_object(stream, is_not_sufficent_stream_length);

        if (is_not_sufficent_stream_length) {
          RTMPLOG(error) << "amf_object parsing fails";
          return false;
        }
        if (result) {
          RTMPLOG(debug) << "amf_end_of_object:" << end_of_object_;
          return true;
        }  
        consume_any_property(stream);
      }
      return true;
      break;
    }
    case AMF0Type::AMF0_ARRAY: {
      unsigned int array_size;
      bool ret = read_number_4byte(stream, array_size);
      RTMPLOG(debug) << "amf_array_size:" << array_size;
      
      
      for (double i = 0; i < array_size; ++i) {
        if (!consume_any(stream)) { 
          return false;    
        }
      }
      return true;
      break;
    }

    case AMF0Type::AMF0_OBJECT_END: {
      RTMPLOG(debug) << "amf_object_end:";
      return true;
      break;
    }

    default: {
      RTMPLOG(debug) << "amf_type not supported. consume all";
      consume_until_end_of_object(stream);
      return true;

      return false;
    }
  }

  return true;
}

bool AMF0Serializer::consume_any_property(std::istream &stream) {
  std::string key;
  if (!read_short_string(stream, key, false)) return false;

  RTMPLOG(debug)<< "amf_object_property_key:" << key;

  if (!consume_any(stream)) return false;

  return true;
}

bool AMF0Serializer::consume_full(std::istream &stream) {
  while (true) {
    if (stream_util::get_readable_length(stream) == 0) return true;

    if (!consume_any(stream)) return false;
  }
  return true;
}

void AMF0Serializer::consume_until_end_of_object(std::istream &stream){
  bool error = false;
  while (is_end_of_object(stream, error) == false && error == false) {
    stream_util::change_relative_position_istream(stream, 1);
  }
}

bool AMF0Serializer::read_mixed_array(std::istream &stream, AMFObject &obj) {
  bool error = false;
  std::string amfobj_name;

  double array_size;
  if (read_number(stream, array_size, false) == false) { return false; }
  RTMPLOG(debug) << "array_size: " << array_size;
  return not error;
}



