### plan in 2018-09-19
1. boost bind 는 될 수 있으면 lamda function 으로 수정 예정
2. 변수 이름을 될 수 있으면 스펙의 이름으로 수정 예정

### modified in 2018-09-18

1.
in StreamUil.cpp 
in StreamUil.hpp
```cpp
void change_relative_position_istream(std::istream& stream,int relative_position) 
-->
void change_relative_position_istream(std::istream& stream,std::streampos relative_position)
```
2.
in OnStatus.hpp
```cpp
oss << oss << ", stream_name:" << stream_name_;
-->
oss << ", stream_name:" << stream_name_;
```
3.
in NetworkUtil.cpp
```cpp
register unsigned long long rstamp = 0x0000000000000000;
-->
unsigned long long rstamp = 0x0000000000000000;
```
4.
in RtmpStream.cpp
vector 에 들어있는 OutRtmpStream 마다, OutRtmpStream 의 member function 인 write_message() 를 param으로 boost::ref(msg)로 각각 호출한다. _1 때문에 error 가 났음
v1 처럼 compiler의 제안에 따라서 _1 을 boost::placeholders::_1 으로 수정
C++11 규격에 따라서 v2 처럼 수정할 수도 있을 것 같다.
```cpp
// std::list<OutRtmpStream_ptr>  linked_out_streams_ 

void InRtmpStream::add_data(rtmp_network::Message_ptr msg) {
  std::for_each(
      linked_out_streams_.begin(), linked_out_streams_.end(),
      boost::bind(&OutRtmpStream::write_message, _1, boost::ref(msg)));
}
--> v1
void InRtmpStream::add_data(rtmp_network::Message_ptr msg) {
  std::for_each(
      linked_out_streams_.begin(), linked_out_streams_.end(),
      boost::bind(&OutRtmpStream::write_message, boost::placeholders::_1, boost::ref(msg)));
}
--> v2
void InRtmpStream::add_data(rtmp_network::Message_ptr msg) {
  std::for_each(
      linked_out_streams_.begin(), linked_out_streams_.end(),
      [&msg](OutRtmpStream_ptr& ortmpstream) { ortmpstream->write_message(msg); } );
}

```