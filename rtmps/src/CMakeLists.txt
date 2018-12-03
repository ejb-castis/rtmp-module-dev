
# rtmpserver_lib

cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME rtmps)
project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS
common/EncryptUtil.cpp
common/NetworkUtil.cpp
common/StreamUtil.cpp
common/StringUtil.cpp
common/TimeUtil.cpp
logic/RtmpHandler.cpp
logic/RtmpStream.cpp
logic/StreamManager.cpp
network/Connection.cpp
network/HandshakeManager.cpp
network/Server.cpp
protocol/cmd_message/CallError.cpp
protocol/cmd_message/CommandMsgParser.cpp
protocol/cmd_message/ConnectResult.cpp
protocol/cmd_message/OnBWDone.cpp
protocol/cmd_message/OnFCPublish.cpp
protocol/cmd_message/OnStatus.cpp
protocol/cmd_message/SimpleResult.cpp
protocol/control_message/ControlMessage.cpp
protocol/control_message/ControlMsgParser.cpp
protocol/data_message/DataMsgParser.cpp
protocol/data_message/OnMetaData.cpp
protocol/data_message/RtmpSampleAccess.cpp
protocol/media_message/MediaMsgParser.cpp
protocol/AMF0Serializer.cpp
protocol/RtmpHeader.cpp
protocol/RtmpHeaderParser.cpp
protocol/RtmpMessage.cpp
protocol/RtmpParser.cpp
protocol/RtmpPayloadParser.cpp
)

set(HEADERS
common/ConcurrentQueue.hpp
common/DataType.hpp
common/EncryptUtil.hpp
common/NetworkUtil.hpp
common/StreamUtil.hpp
common/StringUtil.hpp
common/TimeUtil.hpp
logic/RtmpHandler.hpp
logic/RtmpStream.hpp
logic/StreamManager.hpp
network/Connection.hpp
network/HandshakeManager.hpp
network/Message.hpp
network/MessageSender.hpp
network/RequestHandler.hpp
network/RequestParser.hpp
network/Server.hpp
protocol/cmd_message/CallError.hpp
protocol/cmd_message/CommandMessage.hpp
protocol/cmd_message/CommandMsgParser.hpp
protocol/cmd_message/ConnectMessage.hpp
protocol/cmd_message/ConnectResult.hpp
protocol/cmd_message/CreateStream.hpp
protocol/cmd_message/FCPublish.hpp
protocol/cmd_message/OnBWDone.hpp
protocol/cmd_message/OnFCPublish.hpp
protocol/cmd_message/OnStatus.hpp
protocol/cmd_message/Play.hpp
protocol/cmd_message/Publish.hpp
protocol/cmd_message/ReleaseStream.hpp
protocol/cmd_message/SimpleResult.hpp
protocol/control_message/ControlMessage.hpp
protocol/control_message/ControlMsgParser.hpp
protocol/data_message/DataMessage.hpp
protocol/data_message/DataMsgParser.hpp
protocol/data_message/OnMetaData.hpp
protocol/data_message/RtmpSampleAccess.hpp
protocol/media_message/MediaMessage.hpp
protocol/media_message/MediaMsgParser.hpp
protocol/AMF0Serializer.hpp
protocol/RtmpHeader.hpp
protocol/RtmpHeaderParser.hpp
protocol/RtmpMessage.hpp
protocol/RtmpParser.hpp
protocol/RtmpPayloadParser.hpp
)

add_library(${PROJECT_NAME} ${SRCS} ${HEADERS})
