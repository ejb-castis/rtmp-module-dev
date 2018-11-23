#include "ControlMessage.hpp"
#include "../../common/StreamUtil.hpp"

namespace rtmp_protocol {

size_t SetChunkSize::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_uint(stream, chunk_size_, true, writed);
  //writed += 4;
  return writed;
}
size_t AbortMessage::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_uint(stream, chunk_stream_id_, true, writed);
  //writed += 4;
  return writed;
}
size_t Acknowledgement::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_uint(stream, seq_num_, true, writed);
  //writed += 4;
  return writed;
}
size_t WindowAcknowledgementSize::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_uint(stream, window_size_, true, writed);
  //writed += 4;
  return writed;
}
size_t SetPeerBandwidth::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_uint(stream, window_size_, true, writed);
  stream_util::write_uchar(stream, limit_type_, writed);
  //writed += 5;
  return writed;
}

/****************************
 *  children of UserControlMessage
 */
size_t StreamBegin::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_ushort(stream, event_type_, true, writed);
  stream_util::write_uint(stream, stream_id_, true, writed);
  //writed += 6;
  return writed;
}
size_t StreamEof::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_ushort(stream, event_type_, true, writed);
  stream_util::write_uint(stream, stream_id_, true, writed);
  //writed += 6;
  return writed;
}
size_t StreamDry::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_ushort(stream, event_type_, true, writed);
  stream_util::write_uint(stream, stream_id_, true, writed);
  //writed += 6;
  return writed;
}
size_t SetBufferLength::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_ushort(stream, event_type_, true, writed);
  stream_util::write_uint(stream, stream_id_, true, writed);
  stream_util::write_uint(stream, buffer_length_, true, writed);
  //writed += 10;
  return writed;
}
size_t StreamIsRecorded::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_ushort(stream, event_type_, true, writed);
  stream_util::write_uint(stream, stream_id_, true, writed);
  //writed += 6;
  return writed;
}
size_t PingRequest::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_ushort(stream, event_type_, true, writed);
  stream_util::write_uint(stream, timestamp_, true, writed);
  //writed += 6;
  return writed;
}
size_t PingResponse::write_payload(std::ostream& stream) {
  size_t writed = get_header()->write_payload(stream);
  stream_util::write_ushort(stream, event_type_, true, writed);
  stream_util::write_uint(stream, timestamp_, true, writed);
  //writed += 6;
  return writed;
}

}  // namespace rtmp_protocol

