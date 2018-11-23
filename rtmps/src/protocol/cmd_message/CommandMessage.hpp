#ifndef COMMANDMESSAGE_HPP_
#define COMMANDMESSAGE_HPP_

#include "../RtmpMessage.hpp"
#include <sstream>

namespace rtmp_protocol {

#define CMD_MSG_TYPE_STR_CHECKBANDWIDTH "checkBandwidth"
#define CMD_MSG_TYPE_STR_CLOSE "close"
#define CMD_MSG_TYPE_STR_CLOSESTREAM "closeStream"
#define CMD_MSG_TYPE_STR_CONNECT "connect"
#define CMD_MSG_TYPE_STR_CREATESTREAM "createStream"
#define CMD_MSG_TYPE_STR_DELETESTREAM "deleteStream"
#define CMD_MSG_TYPE_STR_ERROR "_error"
#define CMD_MSG_TYPE_STR_FCPUBLISH "FCPublish"
#define CMD_MSG_TYPE_STR_FCUNPUBLISH "FCUnpublish"
#define CMD_MSG_TYPE_STR_FCSUBSCRIBE "FCSubscribe"
#define CMD_MSG_TYPE_STR_GETSTREAMLENGTH "getStreamLength"
#define CMD_MSG_TYPE_STR_ONBWCHECK "onBWCheck"
#define CMD_MSG_TYPE_STR_ONBWDONE "onBWDone"
#define CMD_MSG_TYPE_STR_ONFCPUBLISH "onFCPublish"
#define CMD_MSG_TYPE_STR_ONSTATUS "onStatus"
#define CMD_MSG_TYPE_STR_PAUSE "pause"
#define CMD_MSG_TYPE_STR_PAUSERAW "pauseRaw"
#define CMD_MSG_TYPE_STR_PLAY "play"
#define CMD_MSG_TYPE_STR_PUBLISH "publish"
#define CMD_MSG_TYPE_STR_RELEASESTREAM "releaseStream"
#define CMD_MSG_TYPE_STR_RESULT "_result"
#define CMD_MSG_TYPE_STR_SEEK "seek"
#define CMD_MSG_TYPE_STR_RTMPSAMPLEACCESS "|RtmpSampleAccess"
#define CMD_MSG_TYPE_STR_ONMETADATA "OnMetaData"


class CommandMsgType {
 public:
  typedef enum {
    CHECKBANDWIDTH,
    CLOSE,
    CLOSESTREAM,
    CONNECT,
    CREATESTREAM,
    DELETESTREAM,
    ERROR,
    FCPUBLISH,
    FCUNPUBLISH,
    FCSUBSCRIBE,
    GETSTREAMLENGTH,
    ONBWCHECK,
    ONBWDONE,
    ONSTATUS,
    PAUSE,
    PAUSERAW,
    PLAY,
    PUBLISH,
    RELEASESTREAM,
    RESULT,
    SEEK,
    ONMETADATA,
    UNKNOWN,
  } type;

  static type get_type(std::string& str) {
    if (str.compare(CMD_MSG_TYPE_STR_CHECKBANDWIDTH) == 0)
      return CHECKBANDWIDTH;
    if (str.compare(CMD_MSG_TYPE_STR_CLOSE) == 0)
      return CLOSE;
    if (str.compare(CMD_MSG_TYPE_STR_CLOSESTREAM) == 0)
      return CLOSESTREAM;
    if (str.compare(CMD_MSG_TYPE_STR_CONNECT) == 0)
      return CONNECT;
    if (str.compare(CMD_MSG_TYPE_STR_CREATESTREAM) == 0)
      return CREATESTREAM;
    if (str.compare(CMD_MSG_TYPE_STR_DELETESTREAM) == 0)
      return DELETESTREAM;
    if (str.compare(CMD_MSG_TYPE_STR_ERROR) == 0)
      return ERROR;
    if (str.compare(CMD_MSG_TYPE_STR_FCPUBLISH) == 0)
      return FCPUBLISH;
    if (str.compare(CMD_MSG_TYPE_STR_FCUNPUBLISH) == 0)
      return FCUNPUBLISH;      
    if (str.compare(CMD_MSG_TYPE_STR_FCSUBSCRIBE) == 0)
      return FCSUBSCRIBE;
    if (str.compare(CMD_MSG_TYPE_STR_GETSTREAMLENGTH) == 0)
      return GETSTREAMLENGTH;
    if (str.compare(CMD_MSG_TYPE_STR_ONBWCHECK) == 0)
      return ONBWCHECK;
    if (str.compare(CMD_MSG_TYPE_STR_ONBWDONE) == 0)
      return ONBWDONE;
    if (str.compare(CMD_MSG_TYPE_STR_ONSTATUS) == 0)
      return ONSTATUS;
    if (str.compare(CMD_MSG_TYPE_STR_PAUSE) == 0)
      return PAUSE;
    if (str.compare(CMD_MSG_TYPE_STR_PAUSERAW) == 0)
      return PAUSERAW;
    if (str.compare(CMD_MSG_TYPE_STR_PLAY) == 0)
      return PLAY;
    if (str.compare(CMD_MSG_TYPE_STR_PUBLISH) == 0)
      return PUBLISH;
    if (str.compare(CMD_MSG_TYPE_STR_RELEASESTREAM) == 0)
      return RELEASESTREAM;
    if (str.compare(CMD_MSG_TYPE_STR_RESULT) == 0)
      return RESULT;
    if (str.compare(CMD_MSG_TYPE_STR_SEEK) == 0)
      return SEEK;
    if (str.compare(CMD_MSG_TYPE_STR_ONMETADATA) == 0)
      return ONMETADATA;
    return UNKNOWN;
  }
  static std::string get_type_str(type value) {
    switch (value) {
      case CHECKBANDWIDTH:
        return CMD_MSG_TYPE_STR_CHECKBANDWIDTH;
      case CLOSE:
        return CMD_MSG_TYPE_STR_CLOSE;
      case CLOSESTREAM:
        return CMD_MSG_TYPE_STR_CLOSESTREAM;
      case CONNECT:
        return CMD_MSG_TYPE_STR_CONNECT;
      case CREATESTREAM:
        return CMD_MSG_TYPE_STR_CREATESTREAM;
      case DELETESTREAM:
        return CMD_MSG_TYPE_STR_DELETESTREAM;
      case ERROR:
        return CMD_MSG_TYPE_STR_ERROR;
      case FCPUBLISH:
        return CMD_MSG_TYPE_STR_FCPUBLISH;
      case FCSUBSCRIBE:
        return CMD_MSG_TYPE_STR_FCSUBSCRIBE;
      case ONBWCHECK:
        return CMD_MSG_TYPE_STR_ONBWCHECK;
      case ONBWDONE:
        return CMD_MSG_TYPE_STR_ONBWDONE;
      case ONSTATUS:
        return CMD_MSG_TYPE_STR_ONSTATUS;
      case PAUSE:
        return CMD_MSG_TYPE_STR_PAUSE;
      case PAUSERAW:
        return CMD_MSG_TYPE_STR_PAUSERAW;
      case PLAY:
        return CMD_MSG_TYPE_STR_PLAY;
      case PUBLISH:
        return CMD_MSG_TYPE_STR_PUBLISH;
      case RELEASESTREAM:
        return CMD_MSG_TYPE_STR_RELEASESTREAM;
      case RESULT:
        return CMD_MSG_TYPE_STR_RESULT;
      case SEEK:
        return CMD_MSG_TYPE_STR_SEEK;
      case ONMETADATA:
        return CMD_MSG_TYPE_STR_ONMETADATA;        
    }
    return "Unknown";
  }
};

class CommandMessage : public RtmpMessage {
 private:
  double transaction_id_;

 public:
  CommandMessage() {
    set_msg_type_id(RtmpHeaderMsgTypeId::COMMAND_AMF0);
  }
  virtual ~CommandMessage() {
  }
  virtual std::string get_class_name() {
    return "CommandMessage";
  }

  double get_transaction_id() {
    return transaction_id_;
  }
  void set_transaction_id(double id) {
    transaction_id_ = id;
  }
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << RtmpMessage::to_string();
    oss << ", transaction_id:" << transaction_id_;
    return oss.str();
  }
  virtual size_t write_payload(std::ostream& stream) {
    std::ostringstream body_stream;
    size_t body_len = write_body_payload(body_stream);

    get_header()->msg_length_ = body_len;

    size_t writed_header = get_header()->write_payload(stream);
    size_t writed_body = RtmpMessage::write_payload_with_body(body_stream,
                                                              stream);

    return writed_header + writed_body;
  }

  virtual size_t write_body_payload(std::ostream& stream) = 0;

};

class UnknownCommand : public CommandMessage {
 public:
  UnknownCommand() {
    set_type(RtmpMessageType::UNKNOWN);
  }
  virtual ~UnknownCommand() {
  }
  // not implemented
  virtual size_t write_body_payload(std::ostream& stream) {
    return 0;
  }
  virtual std::string to_string() {
    std::ostringstream oss;
    oss << "UnknownCommand:(" << command_name_ << "):";
    oss << CommandMessage::to_string();
    return oss.str();
  }

  std::string command_name_;
};
typedef boost::shared_ptr<UnknownCommand> UnknownCommand_ptr;
}  // namespace rtmp_protocol

#endif /* COMMANDMESSAGE_HPP_ */
