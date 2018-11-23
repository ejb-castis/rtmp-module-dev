chunk:
  chunk_header:
    basic_header:
      chunk_type
      chunk_stream_id
    message_header
      timestamp_delta
      message_length
      message_type_id
      message_stream_id
    extented_timestamp
    timestamp
  chunk_data:

message :
  timestamp
  legnth
  type_id
  received_length
  []chunk_data

message_stream :
  id
  []message

chunk_stream:
  id
  []message_stream   

session:
  chunk_size
  chunk_stream_id
  chunk_stream

chunk.basic_header = receive_basic_header()
chunk.message_header = receive_message_header()

session = get_session(chunk.chunk_stream_id)
chunk_stream = session.chunk_stream

if chunk.chunk_type == 0 :
  if chunk.timestamp_delta == 0xFFFFFF :
    chunk.timestamp_delta = chunk.extended_timestamp
  chunk.timestamp = chunk.timestamp_delta

  message_stream = chunk_stream.get_message_stream(chunk.message_stream_id)
  message = message_stream.new_message()
  message.length = chunk.message_length
  message.type_id = chunk.message_type_id
  message.timestamp = chunk.timestamp

if chunk.chunk_type == 1 :
  if chunk.timestamp_delta == 0xFFFFFF
    chunk.timestamp_delta = chunk.extended_timestamp
  chunk.timestamp = chunk_stream.get_preceding_chunk().timestamp + chunk.timestamp_delta
  chunk.message_stream_id = chunk_stream.get_preceding_chunk().message_stream_id
  
  message_stream = chunk_stream.get_message_stream(chunk.message_stream_id)
  message = message_stream.new_message()
  message.length = chunk.message_length
  message.type_id = chunk.message_type_id
  message.timestamp = chunk.timestamp

if chunk.chunk_type == 2 :
  if chunk.timestamp_delta == 0xFFFFFF
    chunk.timestamp_delta = chunk.extended_timestamp
  chunk.timestamp = chunk_stream.get_preceding_chunk().timestamp + chunk.timestamp_delta  
  chunk.message_length = chunk_stream.get_preceding_chunk().message_length
  chunk.message_type_id = chunk_stream.get_preceding_chunk().message_type_id
  chunk.message_stream_id = chunk_stream.get_preceding_chunk().message_stream_id

  message_stream = chunk_stream.get_message_stream(chunk.message_stream_id)
  
  if chunk.message_length < session.chunk_size :
    message = message_stream.new_mesaage()
    message.length = chunk.message_length
    message.type_id = chunk.message_type_id
    message.timestamp = chunk.timestamp
  else:
    message = message_stream.get_current_mesaage()

if chunk.chunk_type == 3 :
  chunk.timestamp_delta = chunk_stream.get_preceding_chunk().timestamp_delta
  chunk.timestamp = chunk_stream.get_preceding_chunk().timestamp + chunk.timestamp_delta
  chunk.message_length = chunk_stream.get_preceding_chunk().message_length
  chunk.message_type_id = chunk_stream.get_preceding_chunk().message_type_id
  chunk.message_stream_id = chunk_stream.get_preceding_chunk().message_stream_id

  message_stream = chunk_stream.get_message_stream(chunk.message_stream_id)

  if chunk.message_length < session.chunk_size :
    message = message_stream.new_mesaage()
    message.length = chunk.message_length
    message.type_id = chunk.message_type_id
    message.timestamp = chunk.timestamp
  else:
    message = message_stream.get_current_mesaage()

receiving_length = message.legnth - message.received_length
chunk.chunk_data = receive_chunk_data(receiving_length)

message.received_length = message.legnth
message.chunk_data.add(chunk.chunk_data)

















