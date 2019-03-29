#2019-03-21

rtmpcamera app 은 아직 연동 안됨 : 
  - 이유 밝히지 못함 
  - nginx 와는 연동이 되는 것으로 보아, nginx 등의 소스코드를 살펴보아야 할 것 같음

#2019-03-20

RtmpHeaderParser::parse_basic_header_3byte() 함수에 버그:
- pointer 를 넘겨야하는데, 그냥 변수를 넘겨서 죽는 문제가 있어서 수정함

chunkStream_id 가 319 밖에 안될 거라는 가정을 하고 만든 소스가 있었는데, 
그 가정에 벗어나는 client app 이 있었다. 
코드 refactoring 겸, structure 를 바꾸어야 할 것 같음

channel_info channels_[MAX_CHANNEL_COUNT];
array 대신 map 으로 변경 



#2019-03-18

추가된 로그:

rtmp-module,0.2.7,2019-03-18,11:03:55.133328,Report,RtmpPayloadParser.cpp::parse_payload:283,,message[AudioMessage:id[18],type[8],length[7],timestamp[0],abs_timestamp[0],header[RtmpHeader:format_type:0,chunk_stream_id:4,timestamp:0,msg_length:7,msg_type_id:8,msg_stream_id:3555,timestamp:0,abs_timestamp:0,message_id:0],data_len:7,data:af 00 11 90 56 e5 00 ]
rtmp-module,0.2.7,2019-03-18,11:03:55.133411,Information,RtmpHandler.cpp::handle_request:55,,handler_id:0,message_name:AudioMessage,message_info:AudioMessage:id[18],type[8],length[7],timestamp[0],abs_timestamp[0],header[RtmpHeader:format_type:0,chunk_stream_id:4,timestamp:0,msg_length:7,msg_type_id:8,msg_stream_id:3555,timestamp:0,abs_timestamp:0,message_id:0],data_len:7,data:af 00 11 90 56 e5 00 
rtmp-module,0.2.7,2019-03-18,11:03:55.138054,Information,flv_message.cpp::process_flv_es_message:126,,audiospecificconfig[AF 00 11 90 56 E5 00]
rtmp-module,0.2.7,2019-03-18,11:03:55.138152,Debug,flv_message.cpp::process_flv_es_message:136,,flv_audio_data[sound_format[AAC:10],sountd_rate[44khz:3],sound_size[16bit:1],sound_type[Stereo:1],aac_packet_type[AACSequenceHeader:0]],flv_audio_spec_config[audio_object_type[AACLC:2],sampling_frequency[48000Hz:3],channel_count[2channel(s):2]]
rtmp-module,0.2.7,2019-03-18,11:03:55.138291,Debug,flv_message.cpp::process_flv_es_message:144,,audiospecificconfig ,object_type[2], sample_rate[48000],channel_count[2]
rtmp-module,0.2.7,2019-03-18,11:03:55.138336,Debug,publish_to_streamer.cpp::publish_to_streamer:282,,process es. context[publish_id[],publish_state[begin],frame_number[0],video_frame_number[0],audio_frame_number[0],no es_frame_data],stream_type[live],stream_name[ffmpeg],client_id[9_1_38567520]




rtmp-module,0.2.7,2019-03-18,11:03:55.151692,Report,RtmpPayloadParser.cpp::parse_payload:283,,message[VideoMessage:id[20],type[9],length[53628],timestamp[34],abs_timestamp[34],header[RtmpHeader:format_type:1,chunk_stream_id:6,timestamp_delta:34,msg_length:53628,msg_type_id:9,msg_stream_id:3555,timestamp:34,abs_timestamp:34,message_id:0],data_len:53628,data:27 01 00 00 a6 00 00 00 02 09 f0 00 00 d1 6d 41 9a 24 6c 41 4f fe b5 2a 80 00 04 01 7e 23 ]
rtmp-module,0.2.7,2019-03-18,11:03:55.151756,Information,RtmpHandler.cpp::handle_request:55,,handler_id:0,message_name:VideoMessage,message_info:VideoMessage:id[20],type[9],length[53628],timestamp[34],abs_timestamp[34],header[RtmpHeader:format_type:1,chunk_stream_id:6,timestamp_delta:34,msg_length:53628,msg_type_id:9,msg_stream_id:3555,timestamp:34,abs_timestamp:34,message_id:0],data_len:53628,data:27 01 00 00 a6 00 00 00 02 09 f0 00 00 d1 6d 41 9a 24 6c 41 4f fe b5 2a 80 00 04 01 7e 23 
rtmp-module,0.2.7,2019-03-18,11:03:55.158306,Debug,flv_message.cpp::process_flv_es_message:276,,video data ,timestamp[34], cts[166],keyframe[2]
rtmp-module,0.2.7,2019-03-18,11:03:55.158355,Debug,flv_message.cpp::process_flv_es_message:284,,video data ,dts[3060], cts[14940], pts[18000], key[2]
rtmp-module,0.2.7,2019-03-18,11:03:55.158379,Debug,publish_to_streamer.cpp::publish_to_streamer:282,,process es. context[publish_id[],publish_state[begin],frame_number[2],video_frame_number[2],audio_frame_number[0],es_frame_data exist],stream_type[live],stream_name[ffmpeg],client_id[9_1_38567520]
rtmp-module,0.2.7,2019-03-18,11:03:55.158405,Debug,publish_to_streamer.cpp::publish_to_streamer:294,,publish es. context[publish_id[],publish_state[begin],frame_number[2],video_frame_number[2],audio_frame_number[0],es_frame_data exist],stream_type[live],stream_name[ffmpeg],client_id[9_1_38567520]



#2019-03-15
log ... 다시 정리해보기

AS-IS:

streamer,1.0.1,2019-03-15,10:17:35.438285,Report,RtmpPayloadParser.cpp::parse_payload:286,,message[AudioMessage:id[75],type[8],length[279],timestamp[0],abs_timestamp[0],header[chunk_header_format[0:FULL],chunk_stream_id[4],timestamp[0],timestamp_delta[0],msg_length[279],msg_type_id[8:AUDIODATA],msg_stream_id[3555],abs_timestamp[0],message_id[0]],data_len:279,data:af 01 de 02 00 4c 61 76 63 35 37 2e 34 38 2e 31 30 31 00 02 2c 6b 5b 6a 0c 5d 0a 07 4e 9b ]
streamer,1.0.1,2019-03-15,10:17:35.438457,Debug,RtmpHandler.cpp::handle_request:58,,handler_id:1,message_name:AudioMessage,message_info:AudioMessage:id[75],type[8],length[279],timestamp[0],abs_timestamp[0],header[chunk_header_format[0:FULL],chunk_stream_id[4],timestamp[0],timestamp_delta[0],msg_length[279],msg_type_id[8:AUDIODATA],msg_stream_id[3555],abs_timestamp[0],message_id[0]],data_len:279,data:af 01 de 02 00 4c 61 76 63 35 37 2e 34 38 2e 31 30 31 00 02 2c 6b 5b 6a 0c 5d 0a 07 4e 9b 
streamer,1.0.1,2019-03-15,11:26:31.495770,Debug,ingest.cpp::OnEsChunkImpl:760,,frame[track:2][av:audio][key:1][pts:2962800][dts:2962800][size:350][buf:21 1A 8F 91 80 F7 FF F1 4C D1 4C 91 95 AE 6D C3 32 D5 2B 89 BB AB 66 A6 5D 55 EA 5D C9 F7 FD C2]

streamer,1.0.1,2019-03-15,11:26:00.806604,Debug,packager.cpp::makeSegment:954,,[myapp/ffmpeg] success to make segment, type[audio], pts[139735508224140,32741:39:56.757479], dur[176580,00:00:01.962000], sample-cnt[92], size[31323], llsegno[776308379]
streamer,1.0.1,2019-03-15,11:26:00.856916,Debug,packager.cpp::makeSegment:946,,[myapp/ffmpeg] success to make fragment, type[video], pts[139735508397210], dur[5940], sample-cnt[2], size[156708]

#2019-03-07
flv audio 정보 구하는 함수에 버그 수정
  : audio 가 aac 가 아닌 경우에 생기는 버그여서, 그동안 발견되진 않았을 것 같음
  

app test :

live push :
  resolution : 720p
  cbr
  20fps
  450~600
  chrome-extension hls : ok 
  chrome-extension dash : ok 
  dsah.js (site) : not ok
  vls : ok
  
nanostream :
  resolution : 720p
  frame rate : 20fps, 25fps, 30fps
  video bitrate : 500 kbps
  keyframe distance : 1sec

  resolution : 1080p
  frame rate : 30fps
  video bitrate : 7500 kbps
  keyframe distance : 5sec

  chrome-extension hls : ok
  chrome-extension dash : not ok 
  dsah.js (site) : not ok
  vls : ok

nginx 설정 :
application myapp {
            live on;

            dash on;
            dash_path /data/dash;
            dash_fragment 3s;
            dash_playlist_length 60s;

            hls on;
            hls_path /data/hls;
            hls_fragment 3s;
            hls_playlist_length 60s;

--------------------------------------------------------------
rtmp app test :

live push :
  resolution : 720p
  cbr
  20fps
  450~600
  chrome-extension hls : ok 
  chrome-extension dash : ok 
  dsah.js (site) : not ok
  vls : ok
  
nanostream :
  resolution : 720p
  frame rate : 20fps, 25fps, 30fps
  video bitrate : 500 kbps
  keyframe distance : 1sec

  resolution : 1080p
  frame rate : 30fps
  video bitrate : 7500 kbps
  keyframe distance : 5sec

  chrome-extension hls : ok
  chrome-extension dash : not ok 
  dsah.js (site) : not ok
  vls : ok

nginx 설정 :

rtmp {
    server {
        listen 1935;
        #chunk_size 4000;
        chunk_size 8192;
        ping 30s;
        notify_method get;

        application myapp {
            live on;

            dash on;
            dash_path /data/dash;
            dash_fragment 5s;
            dash_playlist_length 30s;

            hls on;
            hls_path /data/hls;
            hls_fragment 3;
            hls_playlist_length 60;
            #hls_sync 100ms;




#2019-03-04
video 쪽까지 다시 확인한 후에는
nginx 의 audio timestamp log 확인을 해보아야 할 것 같음
1. audio message 를 drop 시켰을 때 보정? 
2. ngix 에서 timestamp 값을 보정해주는 루틴을 streamer 에 적용해보았을 때, 
확인 사항: 
audio 묵음 현상이 개선되는 지? 
ngix 의 log 와 비슷한 로그가 만들어지는지? 

flv AVCPacketType 이 AVC sequence header 일 때 (0, 1, 2 중 1일때)
이 경우, composition time 처리 
nginx 의 flv 의 avc 처리 부분 코드 확인 :
* 소스 부분은 예전에도 확인 한 적이 있지만, 다시 확인 하기로 함
* 지난  번에는 nginx 의 log 확인은 하지 않았기에 이번에는 nginx 의 log 도 확인해보기로 함

* sps, pps 가 multi 일 때의 버그는 structure 를 고쳐서 처리하든지, 
  sps, pps 마다 구분자로 0001 을 넣어주는 방식으로 구현을 고쳐야 할 것 같다. 
  현재는 제일 앞에만 0001 을 넣어주는 방식 

#2019-02-28
flv AVCPacketType 이 AVC sequence header 일 때 (0, 1, 2 중 0일때)
AVCDecoderConfigurationRecord parsing 부분 점검

AVCDecoderConfigurationRecord 형식 (ISO 14496-15)
https://gist.github.com/uupaa/8493378ec15f644a3d2b

```cpp
aligned(8) class AVCDecoderConfigurationRecord {
    unsigned int(8) configurationVersion = 1;
    unsigned int(8) AVCProfileIndication;
    unsigned int(8) profile_compatibility;
    unsigned int(8) AVCLevelIndication;
    bit(6) reserved = ‘111111’b;
    unsigned int(2) lengthSizeMinusOne;
    bit(3) reserved = ‘111’b;
    unsigned int(5) numOfSequenceParameterSets;
    for (i=0; i< numOfSequenceParameterSets; i++) {
      unsigned int(16) sequenceParameterSetLength ;
      bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;
    }
    unsigned int(8) numOfPictureParameterSets;
    for (i=0; i< numOfPictureParameterSets; i++) {
      unsigned int(16) pictureParameterSetLength;
      bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
    }
}
```

ggogle shaka packager 코드 살펴보기:
  data길이, 값에 대한 fail 처리가 있음
  spec 대로 구현되어있음
    첫 번째 sps 에서 h264 nalu sps paring 해서,
    pixel_width, pixel_height 구해냄    

nginx 의 코드 살펴보기 
  data길이, 값에 대한 fail 처리가 있음  
  spec 대로 구현되어있음
    hls 에 사용할 때는 모든 sps, pps 의 앞에 0001 을 추가하는 코드가 있음
  h264 nalu sps paring 하진 않음
      
streamer 의 코드 살펴보기:
  sps, pps 가 한 개씩인 경우는 spec 대로 구현되어있음  
  sps, pps 가 복수 개일 경우 버그 있음
    첫 번째 sps, pps 만 codec info 로 구하는 버그
    sps 가 복수 개일 경우, pps 를 잘못구하는 버그    
    - sps, pps 가 복수 개가 아닌 경우가 아직 없어서 이 문제가 발생하지 않았을 수도 있음    
  h264 nalu sps paring 하진 않음

#2019-02-27
audio, video frame 이 하나 이상 만들어졌을 때, es upload 시작하게 수정해서 test 
  - audio 문제 해결 안됨

straemer log 에 video pts 가 역전되는 로그가 보임. 문제 원인 파악 하기 
연속한 video frame 이 
dts 는 같고, cts 가 커졌다가 , 작아지면 pts 가 역전되는 현상이 생김

#2019-02-26
* 예외 처리는 추가해서 죽는 문제는 해결되었지만, 여전히 소리가 안나오는 문제가 있음
* flv 로 저장한 파일에서는 소리가 잘 나오기 때문에, es upload 처리할 때, 문제가 있다고 볼 수 있음.
  - 잘못 전달된 message 까지 저장한 상태에서도 flv 파일은 소기가 잘 나오는 듯 하다. 
  - 잘못된 message 로 인한 시간 정보 오류 때문일 가능성이 높지 않을까. 

* ffmpeg 으로 rtmp 전송 시, AudioSpecificConfig 정보가 없는 audio message 를 보내주는 문제가 있었다. 
nginx 의 경우, 이 message를 drop 시키는 반면, 
streamer 는 그냥 처리하다가, 죽거나, 잘못된 audio 정보로 처리하는 문제가 있었음

* ffmpeg 으로 out_8M.ts 을 source 로 하고  -acodec copy -bsf:a aac_adtstoasc option 을 주고, rtmp 전송을 했을 때 
asc 정보가 없는 audio message 를 보내는 문제가 있었음
 
test 버전은 bitReader 를 사용해서  parsing 하다가 죽고, 
이전 버전의 경우는 parsing 을 잘못하게 됨. 
audio data 가 2 byte만 들어와도 handler 쪽으로 전달되기 때문에, 문제가 발생함.
  - header parsing 을 하려면, 적어도 4byte 는 들어와야 함.

문제 발생 시 로그 메시지...  ( 실제 서버에는 없는 로그를 추가했음)

rtmp-module,0.2.7,2019-02-26,11:40:49.062822,Information,flv_message.cpp::flv_message::process_flv_es_message:125,,audiospecificconfig[AF 00]
rtmp-module,0.2.7,2019-02-26,11:40:49.062867,Information,flv_message.cpp::flv_message::process_flv_es_message:142,,audiospecificconfig ,object_type[0], sample_rate[88200],channel_count[0]
rtmp-module,0.2.7,2019-02-26,11:40:49.129531,Information,flv_message.cpp::flv_message::process_flv_es_message:125,,audiospecificconfig[AF 00 11 90]
rtmp-module,0.2.7,2019-02-26,11:40:49.129555,Information,flv_message.cpp::flv_message::process_flv_es_message:142,,audiospecificconfig ,object_type[2], sample_rate[48000],channel_count[2]

audio data 가 완성되지 않은 상태에서 audio parsing 을 하는 바람에 object_type, sample_rate, channel_count 에 버그가 있음.
이 때 어떤 문제가 발생하는 지 코드 review 가 필요함. 


기대 data 크기보다 data 전송이 되지 않는 경우, return 하는 코드가 필요함
사실 get_aac_header 함수에서 return false 를 했어야 했는데, 부족한 data가 있던 상황에서도 
그냥 pointer 를 연산을 하는 바람에 잘못된 data 를 구했음

* 로그 상으로는 ffmpeg 이 message를 잘못 주는 것 같음. 첫 번째는 2byte 짜리를 주고, 두 번째에 4byte 를 주는 것 같음
  만일 그렇다면 nginx 는 어떻게 처리될까?

  nginx 의 로그를 살펴보니 ffmpeg 에서 보낸 AudioSpecificConfig 정보가 없는 length 2 짜리 message 를 AAC header 가 없다는 이유로 skip 해버린다는 걸 알 수 있다.
  
2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 3
2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 4
2019/02/26 13:42:40 [debug] 30172#0: *2 reusing formerly read data: 14
2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP bheader fmt=0 csid=4
2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP mheader fmt=0 audio (8) time=0+0 mlen=2 len=0 msid=1
2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP recv audio (8) csid=4 timestamp=0 mlen=2 msid=1 nbufs=1
2019/02/26 13:42:40 [debug] 30172#0: *2 nhandlers: 5
2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 0
2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 1
2019/02/26 13:42:40 [debug] 30172#0: *2 record:  skipping until AAC header
2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 2
2019/02/26 13:42:40 [debug] 30172#0: *2 live: audio packet timestamp=0
2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP prep audio (8) fmt=1 csid=6 timestamp=0 mlen=2 msid=1 nbufs=1

한 참 후에 audio data message 가 다시 오는데, 이 때 AudioSpecificConfig 을 처리 한다. 

28992829 2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP bheader fmt=0 csid=4
28992830 2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP mheader fmt=0 audio (8) time=0+0 mlen=4 len=0 msid=1
28992831 2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP recv audio (8) csid=4 timestamp=0 mlen=4 msid=1 nbufs=1
28992832 2019/02/26 13:42:40 [debug] 30172#0: *2 nhandlers: 5
28992833 2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 0
28992834 2019/02/26 13:42:40 [debug] 30172#0: *2 codec: aac header af001190
28992835 2019/02/26 13:42:40 [debug] 30172#0: *2 codec: aac header profile=2, sample_rate=48000, chan_conf=2
28992836 2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 1
28992837 2019/02/26 13:42:40 [debug] 30172#0: *2 record:  writing AAC header
28992838 2019/02/26 13:42:40 [debug] 30172#0: *2 record:  frame: mlen=4
28992839 2019/02/26 13:42:40 [debug] 30172#0: *2 write: 12, 00007FFD9690343D, 11, 172606
28992840 2019/02/26 13:42:40 [debug] 30172#0: *2 write: 12, 000055F181F69CC6, 4, 172617
28992841 2019/02/26 13:42:40 [debug] 30172#0: *2 write: 12, 00007FFD9690343D, 4, 172621
28992842 2019/02/26 13:42:40 [debug] 30172#0: *2 record:  frame: mlen=4
28992843 2019/02/26 13:42:40 [debug] 30172#0: *2 write: 12, 00007FFD9690343D, 11, 172625
28992844 2019/02/26 13:42:40 [debug] 30172#0: *2 write: 12, 000055F181EFE89C, 4, 172636
28992845 2019/02/26 13:42:40 [debug] 30172#0: *2 write: 12, 00007FFD9690343D, 4, 172640
28992846 2019/02/26 13:42:40 [debug] 30172#0: *2 calling handler 2
28992847 2019/02/26 13:42:40 [debug] 30172#0: *2 live: audio packet timestamp=0
28992848 2019/02/26 13:42:40 [debug] 30172#0: *2 RTMP prep audio (8) fmt=1 csid=6   timestamp=0 mlen=4 msid=1 nbufs=1

ffmpeg 의 aac_adtstoasc option 의 의미 :
Convert MPEG-2/4 AAC ADTS to an MPEG-4 Audio Specific Configuration bitstream.
This filter creates an MPEG-4 AudioSpecificConfig from an MPEG-2/4 ADTS header and removes the ADTS header.
This filter is required for example when copying an AAC stream from a raw ADTS AAC or an MPEG-TS container to MP4A-LATM, to an FLV file, or to MOV/MP4 files and related formats such as 3GP or M4A. Please note that it is auto-inserted for MP4A-LATM and MOV/MP4 and related formats.

MPEG-TS 에서는 ADTS 를 시용하고, MPEG4 에서는 ASC 를 사용하는 것 같음
소스가 MPEG-TS 나 raw ADTS AAC 를 MP4 나 FLV 로 바꿀 때 이 option 을 사용해야 하는 것 같음
마지막 문장은 MP4 로 변환할때는 자동으로 이 option 이 들어간다는 이야기 같음

RTMP 에서는 FLV format 을 사용하고 있어서 이 option 을 사용해야함


ADTS : AudioDataTransportStFLV format 을 이 sample frequency index, channel configuratio함

 ISO 13818-7 문서의 6.2.1 Fixed Header of ADTS의 Table 8
Ahttps://wiki.multimedia.cx/index.php/ADTS


*streamer 와 nginx 의 차이가 있는 부분 : 
 nginx 의 rtmp-module 에는 audio timestamp 보정 하는 코드가 있음
 이전 frame과 현재 frame 간의 timestamp 값과의 차이가 hls_sync 설정 값 범위 내에 오는 경우, 
 보정값을 사용하고, 그렇지 않은 경우, timstamp 값을 사용하는 기능이 있음

nginx 의 hls_sync 설정에 대한 설명:
https://github.com/arut/nginx-rtmp-module/wiki/Directives#hls_sync

hls_sync
Syntax: hls_sync time
Context: rtmp, server, application
Sets HLS timestamp synchronization threshold. Default is 2ms. This feature prevents crackling noises after conversion from low-resolution RTMP (1KHz) to high-resolution MPEG-TS (90KHz)



#2019-02-25
ISO/IEC_14496-3 2005,2006 버전을 보았을 때는 
audio specific config parsing 구현이 완전하진 않지만,  channel number 버그 말고 다른 부분에서 문제가 될 것 같지는 않음
channel config 가 0 인 경우, channel number 구현이 안되어있음
  
MP4-3  Audio Specific Config parsing 구현 :
google shaka package : 
  * 미구현 :
    - audio object type 0, 31 이상 구현없음
  * 구현 : 
    - channel_configuration 이 15 인 경우 구현있음
    - channel_configuration 이 0 인 경우 구현있음

nginx-rtmp-module :
*  미구현 :
  - audio object type 0, 31 구현없음
  - audio object type 4 보다 큰 경우(31보다 큰 경우도 마찬가지 )  2로 치환함
  - channel_configuration 이 15 인 경우 구현없음
  - channel_configuration 이 0 인 경우 구현없음
 * 구현 : 
   - rtmp audio message header 의 time 값 x 90 을 pts 로 사용함. dts 는 따로 사용하지 않음 
   - AAC frame size is 1024 라고 가정함
  - ADTS header 를 만드는 구현이 있음
  - 

* nginx 와 streamer 의 차이 :
  - AudioSpecificConfiguration parsing 부분에서는 크게 차이가 없어보임
  
  hls 만드는 부분에 
  - nginx 에는 ADTS header 를 만드는 구현이 있고, 
  - pts 를 바꾸어주는 구현이 있는데, 
  - 이 부분은 steamer 의 hsl ingest 와 차이가 있는 지 살펴보아야 할 것 같음


```cpp
  est_pts = ctx->aframe_base + ctx->aframe_num * 90000 * 1024 / codec_ctx->sample_rate;
  dpts = (int64_t) (est_pts - pts);
  if (dpts <= (int64_t) hacf->sync * 90 && dpts >= (int64_t) hacf->sync * -90)
  {
    ctx->aframe_num++;
    ctx->aframe_pts = est_pts;
    return NGX_OK;
  }
  ctx->aframe_base = pts;
```





#2019-02-22

 AAC AudioSpecificConfig  (ISO/IEC 14496-1 (MPEG-4 Systems)) 파싱 버그
- 규격 버전에 따라서 달라지는 것 같기도 한데, 2 byte 인 경우(예전 버전?)도 있는 것 같았음

parsingflv.cpp 
get_aac_header

- audio object type 31 의 경우, 제대로 된 spec 을 가지고 구현한 게 아니라 확실치 않음
- audio object type 31 이상인 경우,  구현 없음
- channel configuration 구현에는 8 channel 인데 7 channel 로 parsing 하는 버그
- channel configuration 0 인 경우, channel number parsing 구현 안되어있음
  - 규격에 따르면, program_config_element 를 parsing 해야하는 것 같음
  - google shaka-package, 2018 에서 아직 지원하지 않는 형상인 것 같았음
    https://github.com/google/shaka-packager/issues/387


rtmp 다시 점검

rtmp 의 payload 는flv file format 의 data 인데  flv header 는 제외된 형태입니다.
flv file format 에 대한 설명은

참조:
Adobe Flash Video File Format Specification 10.1.2.01

Adobe Flash Video File Format Specification 10
https://www.adobe.com/content/dam/acom/en/devnet/flv/video_file_format_spec_v10.pdf

https://www.jianshu.com/p/e1e417eee2e7
https://www.jianshu.com/p/0bff0fc2bf28

ISO 14496-3:
ftp://ftp.tnt.uni-hannover.de/pub/MPEG/audio/mpeg4/documents/w2803/w2803_n.pdf
https://github.com/google/shaka-packager/blob/master/packager/media/codecs/aac_audio_specific_config.h
https://wiki.multimedia.cx/index.php/MPEG-4_Audio
http://read.pudn.com/downloads98/doc/comm/401153/14496/ISO_IEC_14496-3%20Part%203%20Audio/C036083E_SUB1.PDF


AUDIODATA : 1byte
SoundData : Nbyte

AAC SoundData :
  AACAUDIODATA :
    AACPacketType : 1byte
    Data : Nbyte :
      if AACPacketType == 0 :
        AudioSpecificConfig
      else if AACPacketType == 1:
        Raw AAC frame data

The AudioSpecificConfig is explained in ISO 14496-3. Note that it is not the same as the
contents of the esds box from an MP4/F4V file. This structure is more deeply embedded.

* mp4 aac audio specific config
  - 5 bits: object type
  // if (object type == 31)
  //     6 bits + 32: object type
  // 4 bits: frequency(=sample rate) index
  // if (frequency index == 15)
  //     24 bits: frequency
  // 4 bits: channel configuration
  // var bits: AOT Specific Config

ISO 14496-3
``` cpp
AudioSpecificConfig ()
{
  audioObjectType; // 5 blbf
  samplingFrequencyIndex; //4 bslbf
  if ( samplingFrequencyIndex==0xf )
    samplingFrequency; //24 uimsbf
    channelConfiguration; //4 bslbf
  if ( audioObjectType == 1 || audioObjectType == 2 ||
      audioObjectType == 3 || audioObjectType == 4 ||
      audioObjectType == 6 || audioObjectType == 7 )
    GASpecificConfig();
  if ( audioObjectType == 8 )
    CelpSpecificConfig();
  if ( audioObjectType == 9 )
    HvxcSpecificConfig();
  if ( audioObjectType == 12 )
    TTSSpecificConfig();
  if ( audioObjectType == 13 || audioObjectType == 14 ||
        audioObjectType == 15 || audioObjectType==16)
    StructuredAudioSpecificConfig();
  if ( audioObjectType == 17 || audioObjectType == 19 ||
        audioObjectType == 20 || audioObjectType == 21 ||
        audioObjectType == 22 || audioObjectType == 23 )
    GASpecificConfig();
  if ( audioObjectType == 24)
    ErrorResilientCelpSpecificConfig();
  if ( audioObjectType == 25)
    ErrorResilientHvxcSpecificConfig();
  if ( audioObjectType == 26 || audioObjectType == 27)
    ParametricSpecificConfig();
  if ( audioObjectType == 17 || audioObjectType == 19 ||
        audioObjectType == 20 || audioObjectType == 21 ||
        audioObjectType == 22 || audioObjectType == 23 ||
        audioObjectType == 24 || audioObjectType == 25 ||
        audioObjectType == 26 || audioObjectType == 27 ) {
    epConfig; //2 bslbf
    if ( epConfig == 2 || epConfig == 3 ) {
      ErrorProtectionSpecificConfig();
    }
    if ( epConfig == 3 ) {
      directMapping; //1 bslbf
      if ( ! directMapping ) {
        /* tbd */
      }
    }
  }
}
```


NOTE:
TagType 8, 9 이외의 tag 는 drop 시켜야함


#2019-02-21

ngx_rtmp_hls_audio
-> ngx_rtmp_hls_parse_aac_header

#2019-02-20

rtmp module 에서 audio 가 안나오는 경우가 있음

nginx review :
  - ngx_rtmp_hls_audio 를 다시 한 번 review 하기 
  - ngx_rtmp_hls_parse_aac_header

#2019-01-24

* code review:

async_read 시에 boost::asio::transfer_exactly(C0_C1_len) 의 용도 : 
원하는 만큼 사이즈 만큼 socket 에서 read 해서 처리할 수 있음


void Connection::start() {
  RTMPLOGF(info, "start connection[%1%]", id_ );
  unsigned int C0_C1_len = HANDSHAKE_MSG_VERSION_LEN + HANDSHAKE_MSG_LENGTH;

  boost::asio::async_read(
      socket_,
      request_streambuf_,
      boost::asio::transfer_exactly(C0_C1_len),
      strand_.wrap(
          boost::bind(&Connection::handle_read_handshake_C0_C1,
                      shared_from_this(), boost::asio::placeholders::error)));
}



#2019-01-23

* handshake 시에 time 값을 잘못주고 잘못 받고 있는 것 같음
  - 더 확인 필요 

* RTMP spec 의 window size review: 필요
  - client 한테 요청받은 window size 에 맞추어, message를 보냄
  - 관련 내부 변수 64bit 로 변경 
  - 여태까지 받은 byte 의 경우 4byte 를 넘어갈 수 있어서,
    windows size 를 보내는 것으로 변경
  - 나머지 내부 로직은 유지
  - 규격은 windows size 내에서 받은 byte 인지, 전체 byte 인지 정확하게 표현되어있지는 않다.
  - client 와의 연동에는 변동이 없었음
    

* is_continuous_send_state_ 의 용도 review
- 현재는 사용하지 않는 것과 마찬가지여서 삭제해도 될 것 같다.
- 나중에 삭제 예정

is_continuous_send_state_ : message 를 send 하고 있는 상태일 경우 true가 되게 하고   
이 값이 false 인 경우, 바로 write 하고
이 값이 true 인 경우, timer 를 등록해서 다음 번에 다시 write 를 하기 위해 만든 것 같음

그런데 현재는 이 값이 true 가 되는 경우가 없이 실행되고 있음

원래는 handle_play 함수에서 is_continuous_send_state_ 값을 true 로 set 하는 경우가 있었고
true 로 setting 하면서 timer 도 등록되는데, 
play 함수는 사용하지 않기 때문에, publish 에서는 영향을 주지 않았음
  - handle_play 함수가 나중에 필요해지면 다시 살펴보는게 좋겠지만, 
  - 필요성이 이해안되어 handle_play 함수에 있던 set 함수도 지웠음. 
  - 필요가 없기 때문에 handle_play 함수 자체를 지울 수도 있음



#2019-01-18

nginx 실행 & 재실행

sudo /usr/local/nginx/sbin/nginx -s stop
sudo /usr/local/nginx/sbin/nginx

app
rtmp://172.16.33.52:1935/myapp/mystream

browser
http://localhost:8080/hls/mystream.m3u8


#2019-01-11

32 bit timestamp -> 64 bit 처리
rtmp timestamp 는 32 bit 값이어서,
49일 17시간 2분 47.296초 마다 timestamp 값이 0 으로 rollover 된다.

내부적으로 64bit timestamp 값을 사용하고 
32 bit timestamp 값이 roll over 되는 것을 고려해서 처리 하는 코드를 추가


	수정함:        doc/rtmp_dev.md
	수정함:        rtmps/src/flv_message.cpp
	수정함:        rtmps/src/flv_message.h
	수정함:        rtmps/src/logic/RtmpHandler.cpp
	수정함:        rtmps/src/logic/RtmpHandler.hpp
	수정함:        rtmps/src/network/Connection.cpp
	수정함:        rtmps/src/network/Connection.hpp
	수정함:        rtmps/src/network/RequestHandler.hpp
	수정함:        rtmps/src/network/RequestParser.hpp
	수정함:        rtmps/src/protocol/RtmpHeader.hpp
	수정함:        rtmps/src/protocol/RtmpHeaderParser.cpp
	수정함:        rtmps/src/protocol/RtmpMessage.cpp
	수정함:        rtmps/src/protocol/RtmpMessage.hpp
	수정함:        rtmps/src/protocol/RtmpParser.cpp
	수정함:        rtmps/src/protocol/RtmpParser.hpp
	수정함:        rtmps/src/protocol/RtmpPayloadParser.cpp
	수정함:        rtmps/src/protocol/RtmpPayloadParser.hpp
	수정함:        rtmps/src/publish_to_streamer.cpp
	새 파일:       rtmps/src/sna.h
	수정함:        src/version.h

#2019-01-10

rtmp timestamp 는 32 bit 값이어서,
49일 17시간 2분 47.296초 마다 timestamp 값이 0 으로 rollover 된다.

내부적으로 64bit timestamp 값을 사용하고 
32 bit timestamp 값이 roll over 되는 것을 고려해서 
처리 하는 코드를 추가 하는 중

TODO:
1.
지난 번에 못한
nginx 코드 검토도 필요할 것 같디.
  ngx_msec_t              epoch;
  ngx_msec_t              peer_epoch;
  ngx_msec_t              base_time;
  uint32_t                current_time;

nginx 는 roll over 를 어떻게 하는 지 알아보아야할 것 같다.

2.
현재 연동에서는 문제 없어보이지만,
message stream 과 chunksream 간의 관계를 다시 고려해서,
코드를 review 해보아야할 것 같다. 
현재는 하나의 chunk stream 에 
하나의 message stream 이 들어있다고 가정한 부분이 있다.

3.
pseudo code 도 다시 review 하고 
refactoring 해야하지 않을까 ? 
지금 다시 보니 code 읽기 힘듬


#2018-12-26

wowza goCoder 는 wowza server 와만 연동되는 것으로 예상되서, 연동 하지 않기로 했음.


goCoder 연동 :

1. handshake validataion 코드 제외
  handshakeManager 에서 수정했으나, 
  connetion.cpp 에서 수정할 수도 있을 것 같음
2. handshake s2,s3 부분 검토


3. nginx 코드 검토:
  ngx_msec_t              epoch;
  ngx_msec_t              peer_epoch;
  ngx_msec_t              base_time;
  uint32_t                current_time;

  의 용도는?


#2018-12-24

RTMPmessage 에 초기화가 안된 변수들이 있어서
test 하다가 죽는 현상이 있었음
- 그냥 test 하다가는 발생한 적이 없는 데, streamer 에 합쳐서 test 하다가 발견했다.

thread 수가  달라진 다거나 하면 발생하기 때문인 것 같음
초기화 안해서 발생하는 문제는 평소 하던대로만 테스트해서는 발견되지 않을 수 있다.

처음에 코딩할 때부터 초기화는 하는 게 맞고,
만일 하지 않았다면, 
refactoring 과정에서 해주어야 한다. 

더 이상사용하지 않는 변수가 있는데, 
이런 변수도 refactoring 과정에서 없애주어야 할 것 같다. 

int flv_message.cpp 
FIXME: nalu start code length is fixed to 4 


#2018-12-20

stearmer 와 연동 시에 파일이 10초? 이하인 것은 잘 안되는 것 같음
또는 지연 시간이 10초 정도여서 그러는 것일 수도 있음

wowza 에서 만든 goCoder app 과 연동 test 해보았습니다.
1. handshake C1 invalid error : server 쪽에서 handsake C1 의 validataion 하는 부분을 제거하면 넘어갑니다.
  - spec 에는 없는 부분인데, 구현되어있었습니다. 
2. client 가 c2 를 보내지 않습니다. 
  - 서버가 s0, s1, s2 를 잘못보내서 그런 것인지 확인을 해보아야겠습니다.
  - 현재 RTMP spec 대로 구현이 되어있지는 않은 상황이지만, ffmpeg 등의 client 와 통신에서 성공해서
   FIXME: 상태로 그냥 두었었는데, spec 대로 구현을 해보아야겠네요.

  
#2018-12-19

 어제 test 에서 play 시 끊기는 문제는 steamer 에 저장된 파일을 local play 해도 생기는 현상이어서,
 dts, pts 계산하는 코드를 review 해서,
 
 audio, 
 video 쪽에 dts, pts 계산하는 코드를 수정해서 해결했음

 끊기는 현상은 없어지고
 지연만 남은 상황임
 
- rtmp-module 과 Streamer 의 연동 중에 
 rtmp-module 버그로 인해서 es upload api 에 pts, dts 등의 값을 잘못 주면 streamer 가 죽는 문제가 발생하기도 했다.

#2018-12-18

ffmpeg 으로 publish 한 동영상을 flv 형태로 record 해서 
ffplay 로 play 해보면 끊기지 않고 play 된다. 

audio, video frame 을 dump 를 남겨서 timestamp 값을 검사했을 때에도
timestamp 값이 거의 일정한 것을 알 수 있다. 

streamer 와 연동했을 때, 끊기는 현상은 
timestamp 보다는 

1. rtmp-module 과 streamer 의 연동 시에 지연 문제가 있지 않나 싶다. 
2. flv 를 ts es 로 바꿀 때, dts, pts 변환 시에 에러가 있을 수는 있을 것 같다. 


ffmpeg 파일 자르기
ffmpeg -i /data/bigbuckbunny_1500.mp4 -ss 0 -t 10 -vcodec copy -acodec copy 10s_1500k.mp4



#2018-12-14

http://172.16.33.52:8080/brokering/live/baseball.mpg/master.m3u8
http://172.16.33.52:8080/brokering/live/750k.mp4/master.m3u8
http://172.16.33.52:8080/brokering/live/750k/master.m3u8

http://172.16.33.52:8080/brokering/live/750k.mp4


video frame 
avc_seq_header - nal - nal - ...  - avc_end_of_sequence

audio frame
aac_specific_config - frame - frame ...


#2018-12-07

server 가 발급하는 id 로 chunkstream id 가 정해진다.
NEW_STREAM_ID


#2018-12-04


```
   frame 
       -----> decoder -----> display device -----> screen
                ^                   ^  
                |                   |
               DTS                 PTS
                |                   |
                |<----------------->|
               
                Compsition time offset

flv.timestamp = DTS / 90 in millisec
flv.composition_time_offset = ( PTS - DTS ) / 90 in millisec

b-frame needs time because b-frame requires more following frames 

? :
  PTS and CTS (Composition Time Stamp) are the same ? 



ref:
https://stackoverflow.com/questions/7054954/the-composition-timects-when-wrapping-h-264-nalus
```


#2018-12-04


nginx 의 코드 상으로는 

ngx_rtmp_codec_module.c의
ngx_rtmp_codec_av() 
ngx_rtmp_codec_parse_avc_header()
static ngx_int_t
ngx_rtmp_codec_postconfiguration(ngx_conf_t *cf)
{
    ngx_rtmp_core_main_conf_t          *cmcf;
    ngx_rtmp_handler_pt                *h;
    ngx_rtmp_amf_handler_t             *ch;

    cmcf = ngx_rtmp_conf_get_module_main_conf(cf, ngx_rtmp_core_module);

    h = ngx_array_push(&cmcf->events[NGX_RTMP_MSG_AUDIO]);
    *h = ngx_rtmp_codec_av;

    h = ngx_array_push(&cmcf->events[NGX_RTMP_MSG_VIDEO]);
    *h = ngx_rtmp_codec_av;

    h = ngx_array_push(&cmcf->events[NGX_RTMP_DISCONNECT]);
    *h = ngx_rtmp_codec_disconnect;

    /* register metadata handler */
    ch = ngx_array_push(&cmcf->amf);
    if (ch == NULL) {
        return NGX_ERROR;
    }
    ngx_str_set(&ch->name, "@setDataFrame");
    ch->handler = ngx_rtmp_codec_meta_data;

    ch = ngx_array_push(&cmcf->amf);
    if (ch == NULL) {
        return NGX_ERROR;
    }
    ngx_str_set(&ch->name, "onMetaData");
    ch->handler = ngx_rtmp_codec_meta_data;


    return NGX_OK;
}



등이 관련이 있는 듯



https://www.adobe.com/content/dam/acom/en/devnet/flv/video_file_format_spec_v10_1.pdf

audio dump : aac 
audio header : 2 byte
aac type 인 경우 AF 00 or AF 01

xxd -b audio.dump.1
00000000: 10101111 00000000 00010001 10010000 01010110 11100101  ....V.
00000006: 00000000                                               .

hd audio.dump.1
00000000  af 00 11 90 56 e5 00                              |....V..|
00000007

video dump: h.264
video header : 5byte (17 00 00 00 00) 

frameType : 1 : key frame for AVC (1:key, 2:inter, etc)
codecId : 7 : AVC  
AVCPacketType : 0 : AVC sequence header (0:header of seq , 1: NANU, 2:end of seq )
compositionTime : 0 0 0 : timeoffset (3byte big endian)

hd -n 100 video.dump.1 
00000000  17 00 00 00 00 01 64 00  28 ff e1 00 29 67 64 00  |......d.(...)gd.|
00000010  28 ac d9 40 78 04 4f de  03 6a 02 02 02 80 00 01  |(..@x.O..j......|
00000020  f4 80 00 75 30 71 10 00  09 89 68 00 05 16 17 af  |...u0q....h.....|
00000030  7c 07 16 86 0a 96 01 00  05 68 ff b5 25 00        ||........h..%.|
0000003e

xxd -l 100 -b video.dump.1
00000000: 00010111 00000000 00000000 00000000 00000000 00000001  ......
00000006: 01100100 00000000 00101000 11111111 11100001 00000000  d.(...
0000000c: 00101001 01100111 01100100 00000000 00101000 10101100  )gd.(.
00000012: 11011001 01000000 01111000 00000100 01001111 11011110  .@x.O.
00000018: 00000011 01101010 00000010 00000010 00000010 10000000  .j....
0000001e: 00000000 00000001 11110100 10000000 00000000 01110101  .....u
00000024: 00110000 01110001 00010000 00000000 00001001 10001001  0q....
0000002a: 01101000 00000000 00000101 00010110 00010111 10101111  h.....
00000030: 01111100 00000111 00010110 10000110 00001010 10010110  |.....
00000036: 00000001 00000000 00000101 01101000 11111111 10110101  ...h..
0000003c: 00100101 00000000                                      %.


mp3, flv1 의경우

audio header 1 byte 로 2f 인 것으로 보이고
video header 1 byte 는 12 또는 22 인 것으로 보입니다.


ffmpeg option 조정

option 을 정하지 않으면, 

audio 는 mp3 로 video 는 flv1 으로 re-encoding 되는 것 같습니다.
audio codec 를 ac3 로 설정해보니, flv는 지원안되다는 메시지가 나옵니다.

찾아보니 option 을 조정해서, rtmp 로 보낼 수 있었습니다.

ffmpeg -re -i /data/movie/20181130104010.mp4 -acodec aac -vcodec copy -f flv rtmp://127.0.0.1:1935/live/stream


https://trac.ffmpeg.org/wiki/StreamingGuide

-re : 
  read input at native frame rate. mainly used to simulate a grab device. 
  i.e. if you wnated to stream a video file, then you would want to use this, otherwise it might stream it too fast ( it attemps to stream at line spped by default). you typically don't want to use this flag when streaming from a live device.

-acodec aac :
  sets the audio codec

-vcodec copy :
  set the video codec as it is as of the input file. stream is not to be re-encoded.

-f flv : 
  deliver the output stream in an flv wrapper

참고 : 
  > ffmpeg -re -i /data/movie/20181130104010.mp4 -acodec aac -vcodec copy -f flv rtmp://127.0.0.1:1935/live/stream

Input #0, mov,mp4,m4a,3gp,3g2,mj2, from '/data/movie/20181130104010.mp4':
  Metadata:
    major_brand     : isom
    minor_version   : 512
    compatible_brands: isomiso2avc1mp41
    encoder         : Lavf58.22.100
  Duration: 00:00:03.00, start: 0.000000, bitrate: 10482 kb/s
    Stream #0:0(und): Video: h264 (High) (avc1 / 0x31637661), yuv420p(tv, bt709), 1920x1080 [SAR 1:1 DAR 16:9], 10095 kb/s, 29.97 fps, 29.97 tbr, 90k tbn, 59.94 tbc (default)
    Metadata:
      handler_name    : VideoHandler
    Stream #0:1(kor): Audio: ac3 (ac-3 / 0x332D6361), 48000 Hz, stereo, fltp, 384 kb/s (default)
    Metadata:
      handler_name    : SoundHandler
    Side data:
      audio service type: main
Stream mapping:
  Stream #0:0 -> #0:0 (copy)
  Stream #0:1 -> #0:1 (ac3 (native) -> aac (native))
Press [q] to stop, [?] for help
Output #0, flv, to 'rtmp://127.0.0.1:1935/live/stream':
  Metadata:
    major_brand     : isom
    minor_version   : 512
    compatible_brands: isomiso2avc1mp41
    encoder         : Lavf57.83.100
    Stream #0:0(und): Video: h264 (High) ([7][0][0][0] / 0x0007), yuv420p(tv, bt709), 1920x1080 [SAR 1:1 DAR 16:9], q=2-31, 10095 kb/s, 29.97 fps, 29.97 tbr, 1k tbn, 90k tbc (default)
    Metadata:
      handler_name    : VideoHandler
    Stream #0:1(kor): Audio: aac (LC) ([10][0][0][0] / 0x000A), 48000 Hz, stereo, fltp, 128 kb/s (default)
    Metadata:
      handler_name    : SoundHandler
      encoder         : Lavc57.107.100 aac
    Side data:
      audio service type: main

  >ffplay dump.flv

  Input #0, flv, from 'dump.flv':    0KB vq=    0KB sq=    0B f=0/0   
  Duration: 00:00:02.95, start: 0.000000, bitrate: 10289 kb/s
    Stream #0:0: Video: h264 (High), yuv420p(tv, bt709, top first), 1920x1080 [SAR 1:1 DAR 16:9], 30.30 fps, 29.97 tbr, 1k tbn, 59.94 tbc
    Stream #0:1: Audio: aac (LC), 48000 Hz, stereo, fltp

참고 : 
  > ffmpeg -re -i /data/movie/20181130104010.mp4 -f flv rtmp://127.0.0.1:1935/live/stream

  Input #0, mov,mp4,m4a,3gp,3g2,mj2, from '/data/movie/20181130104010.mp4':
  Metadata:
    major_brand     : isom
    minor_version   : 512
    compatible_brands: isomiso2avc1mp41
    encoder         : Lavf58.22.100
  Duration: 00:00:03.00, start: 0.000000, bitrate: 10482 kb/s
    Stream #0:0(und): Video: h264 (High) (avc1 / 0x31637661), yuv420p(tv, bt709), 1920x1080 [SAR 1:1 DAR 16:9], 10095 kb/s, 29.97 fps, 29.97 tbr, 90k tbn, 59.94 tbc (default)
    Metadata:
      handler_name    : VideoHandler
    Stream #0:1(kor): Audio: ac3 (ac-3 / 0x332D6361), 48000 Hz, stereo, fltp, 384 kb/s (default)
    Metadata:
      handler_name    : SoundHandler
    Side data:
      audio service type: main
  Stream mapping:
    Stream #0:0 -> #0:0 (h264 (native) -> flv1 (flv))
    Stream #0:1 -> #0:1 (ac3 (native) -> mp3 (libmp3lame))
  Press [q] to stop, [?] for help
  
  Output #0, flv, to 'rtmp://127.0.0.1:1935/live/stream':
  Metadata:
    major_brand     : isom
    minor_version   : 512
    compatible_brands: isomiso2avc1mp41
    encoder         : Lavf57.83.100
    Stream #0:0(und): Video: flv1 (flv) ([2][0][0][0] / 0x0002), yuv420p, 1920x1080 [SAR 1:1 DAR 16:9], q=2-31, 200 kb/s, 29.97 fps, 1k tbn, 29.97 tbc (default)
    Metadata:
      handler_name    : VideoHandler
      encoder         : Lavc57.107.100 flv
    Side data:
      cpb: bitrate max/min/avg: 0/0/200000 buffer size: 0 vbv_delay: -1
    Stream #0:1(kor): Audio: mp3 (libmp3lame) ([2][0][0][0] / 0x0002), 48000 Hz, stereo, fltp (default)
    Metadata:
      handler_name    : SoundHandler
      encoder         : Lavc57.107.100 libmp3lame
    Side data:
      audio service type: main

  >ffplay dump.flv
  
  Input #0, flv, from 'dump.flv':    0KB vq=    0KB sq=    0B f=0/0   
  Duration: 00:00:02.98, start: 0.000000, bitrate: 5440 kb/s
    Stream #0:0: Video: flv1, yuv420p, 1920x1080, 29.97 fps, 29.97 tbr, 1k tbn
    Stream #0:1: Audio: mp3, 48000 Hz, stereo, s16p, 128 kb/s








# 2018-11-23

## basic header, 1,2,3 byte
chunk type 과 chunk stream id 

## message header , 11, 7, 3, 0 byte and extended timestamp, 4 byte

### chunk type : 0 의 message header 11 byte
**timestamp 값이 absolute 값임**
스펙을 보면 절대 값이기 때문에 과거시간이 들어있을 수도 있는 것 같음
단 기준 시간이 언제인지는 모르겠음
 
### chunk type : 1 의 message header 7 byte
**message stream id 4byte 가 생략됨.** 
대신 앞의 chunk 의 message stream id 를 사용함.
timestamp 값은 3 byte의 delta 값이 사용됨

### chunk type : 2 의 message header 3 byte
stream id 4 byte 생략, message length 3 byte 생략, message type 1 byte 생략
모두 이전 chunk 의 값을 사용함
**timestamp delta 3byte 만 사용함**

### chunk type : 3 의 **message header 0 byte**
stream id 4 byte 생략, message length 3 byte 생략, message type 1 byte 생략, timestamp 값도 생략
모두 이전 chunk 의 값을 사용함

**timestamp 값:**
type 0 만 timestamp delta 값으로 timestamp 의 absolute 값이 사용되고,
type 1, 2 은 timestamp delta 가 사용됨
type 3 은 timestamp delta 값도 생략됨 

**unix epoch 가 4byte (32bit)인데 비해, timestamp delta 는 3byte로  자리수가 더 작으므로,**

chunk type 0 의 timestamp (delta)값의 3byte max 값인 0xFFFFFF 보다 큰 경우에는 
이 3byte를 0xFFFFFF 로 채우고, extended message header 4 byte 를 timestamp 값으로 사용한다.

chunk type 1, 2 의 timestamp delta 값 역시 마찬가지로 3 byte 값을 넘을 때는
앞의 3byte 를 0xFFFFFF 로 채우고, extended message header 4 byte 를 사용하여 timestamp delta 값을 나타낸다.

스펙에서는 두 가지 예가 나옴

예1:
chunk 크기보다 작은 크기의 data 를 여러 개 보내는 예
이 data 들은 timestamp 값이 모두 다르고, 크기는 모두 같으며 chunk max 크기 이하임

첫 번째 chunk 의 크기와 시간만 명시하고 나머지 chunk 들은 크기가 같으므로 time delta 값만 명시해줄 수 있음.
time delta 값도 같은 경우, time delta 값도 생략할 수 있음

type0 - type2 - type3 - type3 

예2: 
보내려는 data 가 chunk size 보다 커서 세 개의 chunk 에 나누어 보낼 때의 예이다. 
이 경우 timestamp 값이 당연히 하나 밖에 없음

첫 번째 chunk 의 크기가 전체 data 의 크기가 되고,
timestamp 역시 첫 번째 chunk 의 timestamp 만 의미가 있게 보냈다.
두 번째, 세 번째 chunk 는 header 없이 보냄.

규격 상으로는 두 번째, 세 번째 chunk 의 정보는 앞의 chunk 정보를 따르기 때문애
보내는 data 의 크기는 첫 번째 chunk 의 크기의 세 배라고 생각할 수도 있을 것 같은데
chunk size 가 data size 보다 작기 때문에, 이치에 맞지는 않음

즉,
message length 가 chunk 크기보다 큰 경우에는 
그 뒤에 오는 chunk 는 첫 번째 chunk 의 부분으로써 의미를 갖는 것으로 보인다.
chunk id, message id 가 같다면 
timestamp delta, message length는 무시되어도 됨

message 의 length 가 chunk 크기 보다 작을 때는 
그 뒤의 오는 chunk 도 각각의 chunk 크기와 timestamp delta 값이 의미를 가질 수 있음

type0 - type3 - type3 


header min 값은
+ min 1 basic header
+ min 0 message header
+ min 0 extended header (timestamp)
= 1 byte 

header max 값은
 + max 3  basic header
 + max 11 message header
 + max 4  extended header (timestamp)
= #define NGX_RTMP_MAX_CHUNK_HEADER       18

nginx 에서는 이 값이 어디에서 사용될까? 
header 정보를 받는 buffer 크기로 사용되는 것 같음

nginx 코드 살펴 보기

#keword,  //TODO:
epoch
ngx_rtmp_live_av,
static ngx_int_t
ngx_msec_t 는 ngx 의 main module 에 있는 걸까? 

typedef struct {
    uint32_t                csid;       /* chunk stream id */
    uint32_t                timestamp;  /* timestamp (delta) */
    uint32_t                mlen;       /* message length */
    uint8_t                 type;       /* message type id */
    uint32_t                msid;       /* message stream id */
} ngx_rtmp_header_t;


typedef struct {
    ngx_rtmp_header_t       hdr;
    uint32_t                dtime;
    uint32_t                len;        /* current fragment length */
    uint8_t                 ext;
    ngx_chain_t            *in;
} ngx_rtmp_stream_t;

typedef struct {
    uint32_t                signature;  /* "RTMP" */ /* <-- FIXME wtf */


    ngx_event_t             close;

    void                  **ctx;
    void                  **main_conf;
    void                  **srv_conf;
    void                  **app_conf;

    ngx_str_t              *addr_text;
    int                     connected;

#if (nginx_version >= 1007005)
    ngx_queue_t             posted_dry_events;
#else
    ngx_event_t            *posted_dry_events;
#endif

    /* client buffer time in msec */
    uint32_t                buflen;
    uint32_t                ack_size;

    /* connection parameters */
    ngx_str_t               app;
    ngx_str_t               args;
    ngx_str_t               flashver;
    ngx_str_t               swf_url;
    ngx_str_t               tc_url;
    uint32_t                acodecs;
    uint32_t                vcodecs;
    ngx_str_t               page_url;

    /* handshake data */
    ngx_buf_t              *hs_buf;
    u_char                 *hs_digest;
    unsigned                hs_old:1;
    ngx_uint_t              hs_stage;

    /* connection timestamps */
    ngx_msec_t              epoch;
    ngx_msec_t              peer_epoch;
    ngx_msec_t              base_time;
    uint32_t                current_time;

    /* ping */
    ngx_event_t             ping_evt;
    unsigned                ping_active:1;
    unsigned                ping_reset:1;

    /* auto-pushed? */
    unsigned                auto_pushed:1;
    unsigned                relay:1;
    unsigned                static_relay:1;

    /* input stream 0 (reserved by RTMP spec)
     * is used as free chain link */

    ngx_rtmp_stream_t      *in_streams;
    uint32_t                in_csid;
    ngx_uint_t              in_chunk_size;
    ngx_pool_t             *in_pool;
    uint32_t                in_bytes;
    uint32_t                in_last_ack;

    ngx_pool_t             *in_old_pool;
    ngx_int_t               in_chunk_size_changing;

    ngx_connection_t       *connection;

    /* circular buffer of RTMP message pointers */
    ngx_msec_t              timeout;
    uint32_t                out_bytes;
    size_t                  out_pos, out_last;
    ngx_chain_t            *out_chain;
    u_char                 *out_bpos;
    unsigned                out_buffer:1;
    size_t                  out_queue;
    size_t                  out_cork;
    ngx_chain_t            *out[0];
} ngx_rtmp_session_t;



ngx_rtmp_record_write_header(ngx_file_t *file)

flv?
ngx_rtmp_flv_module.c

command 처리
ngx_rtmp_cmd_module.c

녹화관련 모듈일까?
ngx_rtmp_record_module.c

rtmp 메시지 처리의 시작
ngx_rtmp_receive.c
ngx_rtmp_protocol_message_handler()

rtmp 메시지 값 정의 등...
ngx_rtmp.h

live protocol 처리용:? 
ngx_rtmp_live_moduel.h



# 2018-10-11

### TODO:
1. nginx 의 rtmp module 코드에서 hls 지원하는지 찾아보기
 - 지원함
2. timestamp 정보 관리하기 
3. video , audio codec 정보 관리하기
4. es data 찾아내기
5. streamer 코드와 연동하기
6. nginx 의 코드에서 어떻게 rtmp data를 ts 로 바꾸는 지 알아보기 또는 pts, dts, track id 등을 만드는 코드가 있는 지 알아보기

1. streamer 코드 읽어보기
 - es stream 을 어떻게 hls chunk 로 만드는 지 찾아보기
 
EsFrameMsg 
struct EsFrameMsg {
  bool is_video_; // audio, video 여부
  std::uint8_t track_id_; // ???
  bool is_key_frame_; // key frame 여부
  std::uint64_t pts_; // pts
  std::uint64_t dts_; // dts
  std::uint32_t size_; // payload 길이 ??
  std::string buf_; // payload : h264, aac data 
}

1. 
플래시 비디오 : 어도비 동영상 전송을 위한 콘테이너 파일 규격
확장자 : flv, f4v, f4a, f4b
비디오 : Sorenson-H263, On2 TrueMotion VP6, H264
오디오 : MP3, CM, ADPCM, AAC

RTMP 프로토콜 : flv 를 스트리밍하는 데 사용하는 스트리밍 프로토콜 

2.
ngx_rtmp_mpegts_write_frame(...) 에서 특이점

1.1 코덱 정보 사용
- codec_ctx = ngx_rtmp_get_module_ctx(s, ngx_rtmp_codec_module);
1.2 video_codec_id 파싱 결과 사용
- if (codec_ctx->video_codec_id != NGX_RTMP_VIDEO_H264) {...}
1.3

3.
nginx 의 코드 

flv record 의 경우:
ngx_rtmp_record_write_frame(...) heck 코드를 넣어서 chunk의 payload 를 파일로 만들어볼 수 있었음. in(:ngx_chain_t)->buf->pos 을 src 로 하여 file write 를 했음
  |--ngx_rtmp_record_node_av(...)
    |--ngx_rtmp_record_av(...) audio, video data message 에 대한 handler 함수 
      |--ngx_rtmp_record_postconfiguration(...) 에서 setting    

hls video record 의 경우:
ngx_rtmp_mpegts_write_frame(...) ngx_rtmp_hls_video 에서 사용하는 ts 를 file에 저장 함수
ngx_rtmp_hls_copy(...) ngx_rtmp_hls_video 에서 사용하는 hls 정보를 메모리 buffer에 저장 함수
  |--ngx_rtmp_hls_video(...) audio, video data message 에 대한 handler 함수 
    |--ngx_rtmp_hls_postconfiguration(...) 에서 setting

hls audio record 의 경우:
ngx_rtmp_hls_flush_audio()
  |--ngx_rtmp_hls_audio(...)


아래 코드를 보면, publish, create, close_stream 등의 메시지에 대한 func 을 등록하는 것을 알 수 있다.
아래 함수는 conf 에 맞추어서 호출되는 것으로 예상된다.

```cpp
ngx_rtmp_hls_postconfiguration(ngx_conf_t *cf)
{
    ngx_rtmp_core_main_conf_t   *cmcf;
    ngx_rtmp_handler_pt         *h;

    cmcf = ngx_rtmp_conf_get_module_main_conf(cf, ngx_rtmp_core_module);

    h = ngx_array_push(&cmcf->events[NGX_RTMP_MSG_VIDEO]);
    *h = ngx_rtmp_hls_video;

    h = ngx_array_push(&cmcf->events[NGX_RTMP_MSG_AUDIO]);
    *h = ngx_rtmp_hls_audio;

    next_publish = ngx_rtmp_publish;
    ngx_rtmp_publish = ngx_rtmp_hls_publish;

    next_close_stream = ngx_rtmp_close_stream;
    ngx_rtmp_close_stream = ngx_rtmp_hls_close_stream;

    next_stream_begin = ngx_rtmp_stream_begin;
    ngx_rtmp_stream_begin = ngx_rtmp_hls_stream_begin;

    next_stream_eof = ngx_rtmp_stream_eof;
    ngx_rtmp_stream_eof = ngx_rtmp_hls_stream_eof;

    return NGX_OK;
}
```

4. nginx rtmp module 의 hls 지원

아래 rtmp module & hls 사이트 정보를 참고하여 nginx 의 conf 설정을 해주면, ffmpeg으로 보낸 flv stream 이 rtmp-module 에 의해서 hls 용 m3u8과 ts파일로 만들어진다.

* ffmpeg으로 stream 을 보내는 중에도 hls 용 chunk 는 만들어진다.
* ffmpeg으로 stream 을 보내고 나서 시간이 조금 지나면 hls용 파일이 지워진다. 왜 그런 지는 모르겠음

ffmpeg 요청:
ffmpeg -re -i /data/SampleVideo_720x480_1mb.mp4  -c copy -f flv rtmp://127.0.0.1:1935/myapp/sample_stream

만들어지는 파일:
sample_stream-0.ts
sample_stream-0.m3u8

chrome browser로 요청: chrome player plugin 을 설치한 경우, browser에서 play 된다.
http://localhost:8080/hls/sample_stream.m3u8

rtmp module & hls 참고 url:
https://docs.peer5.com/guides/setting-up-hls-live-streaming-server-using-nginx/


rtmp 형태:
rtmp://host[:port]/app_name/stream_name
http://host:8080/hls/stream_name.m3u8

rtmp module 참고 url:
https://github.com/arut/nginx-rtmp-module/wiki/Getting-started-with-nginx-rtmp

nginx debug log 남기기 참고 url:
http://nginx.org/en/docs/debugging_log.html


# 2018-10-10

### 한일 
1. nginx 의 rtmp module debugging 하기 
 1.1 ffmpeg으로 mp4 파일을 flv 형식으로 rtmp 프로토콜로 전송
 1.2 nginx 의 flv 형태로 record 하는 기능을 이용하여 debugging 하기
2. nginx 의 rtmp module를 참고해서 dump 파일 만들어보기
 2.1 nginx 의 flv 형태로 record 하는 기능을 일부 서버에서 사용해보기

### TODO:
1. nginx 의 rtmp module 코드에서 hls 지원하는지 찾아보기
2. dump 된 파일을 play 하면 조금씩 끊김. timestamp 정보가 완전하지 않기 때문인 것으로 추정됨
 2.1 flv 형식으로 저장할 때, 중간 중간에 timestamp 정보를 써주어야 하는데, 이 값이 정확하지 않기 때문으로 추정됨
 2.1 현재 서버의 timestamp 정보 관리 부분 구현이 아직 미완성인 것으로 파악되기 때문에 이렇게 추정하게 됨


## ffmpeg -re -i /data/SampleVideo_720x480_1mb.mp4  -c copy -f flv rtmp://127.0.0.1:1935/myapp/sample_stream

# 2018-10-08

1. to convert rtmp (flv) to hls
1.1. find ways to convert rtmp chunk to hls 
1.2. read streamer source
1.3. find (nginx, ffmpeg, rtmpdump) code for recording rtmp chunk in a file 

2. to improve server source code
2.1. read streamer source
2.1. read nginx source code
2.2. read rtmpdump source code
2.3. read ffmpeg source code
2.4. run ffserver 

# 2018-10-05

### TODO:
1. ffmpeg 코드에서 chunk write 하는 코드가 있는 지 찾아보기
2. ffmpeg 코드에서 "Failed to update header with correct duration" 에러 발생 이유 찾아보기
3. 서버와 ffmpeg client 를 두 개 이상 연동해보기

### ffmpeg : publishing
ffmpeg -re -i /data/SampleVideo_720x480_1mb.mp4 -c copy -f flv rtmp://127.0.0.1:1935/myapp/mystream

ffmpeg -re -i /data/750k.ts -f flv rtmp://127.0.0.1:1935/live/750k_ts_stream
ffmpeg -re -i /data/750k.mp4 -c copy -f flv rtmp://127.0.0.1:1935/live/750k_mp4_stream

### hexadaump
hexdump -C test.video.mp4 > audiodump.txt

### ffmpeg : recording
ffmpeg -i rtmp://127.0.0.1:1935/live/750k_ts_stream -b:v 900k -vcodec libx264 -acodec aac -b:a 256k -strict -2 -t 3600 /data/750k_ts_stream.mp4

### flv to mp4
ffmpeg -i yourVideo.flv yourVideo.mp4

### play
ffplay video.mp4

### DONE:

1. FCUnpublish 명령 처리, deleteStream 명령 처리
- FCUnpublish,  DeleteStream 명령은 ffmpeg 의 코드를 참고해서 만듬
- 서버에 메시지 추가 하는 게 생각보단 복잡해서, 더 간단한 방법이 있는지 다른 코드들을 참고하면 어떨까 싶음

2. 현재 서버의 로직
- 하나의 message 가 여러 chunk 로 이루어진 경우에, 모든 chunk 를 전송받은 다음에 payload 가 pasring 되고 handler 가 호출된다. 
- audio, video data 역시, 마찬가지임. 

3. open source 참고하기, 읽어보기
- ffmpeg 과 rtmpdump 코드 참고하기
- git 의 다른 rtmp 서버 코드 참고하기

4. rtmpbump 는 compile 해서 돌려봄
rtmpsrv 는 ffmpeg 연동 실패함. 이유는 일단 살펴보지 않았음

5. ffmpeg 코드를 살펴보면서 알게된 점들
서버:
- connect 이후에  클라이언트가 Connect 메시지를 보내기 전에도 PT_CHUNK_SIZE, CTRL_ABORT_MESSAGE, Acknowledgement (PT_BYTES_READ), WindowAckSize, PeerBW, 명령을 날릴 수 있다고 가정하고 있음
- Connect 명령의 SeqNumm (transaction id), App 정보를 관리함
- Connect 이후에 Send Window Acknowledgement Size 보냄
- ??? _result 메시지 다음에는 OnBWDone 메시지를 보낸다. (현재 서버에서는 삭제했음. 그래도 동작함)
- 코드에서 channel 은 chunk stream id 를 의미하는 것으로 보인다.
- ffmpeg은 audio 와 video 를 chunkstream id 를 각각 4, 6 으로 다르게 보내고, message stream id 는 서버에서 보내준 값과 같은 값으로 보낸다.

```cpp
enum RTMPChannel {
    RTMP_NETWORK_CHANNEL = 2,   ///< channel for network-related messages (bandwidth report, ping, etc)
    RTMP_SYSTEM_CHANNEL,        ///< channel for sending server control messages
    RTMP_AUDIO_CHANNEL,         ///< channel for audio data
    RTMP_VIDEO_CHANNEL   = 6,   ///< channel for video data
    RTMP_SOURCE_CHANNEL  = 8,   ///< channel for a/v invokes
};
```

클라이언트:
connect 의 응답으로 서버가 보내는 _result 메시지에 stream_id 를 보낼 수 있는데, 클라이언트가 이 값을 이후 메시지 들의 message_stream_id 로 계속 사용함
connect 이후에 ReleaseStream, FCPublishStream, CreateStream 메시지를 보내는 식으로 동작함

#2018-10-04

1. payload parsing 코드 분석, data 받지 못하는 이유 밝히기
- 특별한 문제는 없었음. 서버에 data record 부분이 구현되지 않았기 때문에, 문제가 있어 보인 것이었음

2. header parsing error 발생이유 살피기 (header parsing fail)
- 통신에서 사용하는 buffer 에 data 가 없을 때, parser의 parsing header, parsing payload 함수를 호출할 때 생기는 log 였다.
- 통신에서 사용하는 buffer 에 data 가 없을 때, parser의 parsing header, parsing payload 함수를 호출하지 않게 수정함

3. 현재 server 에는 chunk stream id 와 message stream id 관리하는 부분이 완전히 구현이 되어있지는 않는 것 같다.

3. open source 살펴보기
- git 에 올라와있는 서버 소스를 살펴보고 있음
- ffmpeg 에서 사용 한다고 하는 librtmp 소스를 살펴보아야 할 것 같음
- librtmp 소스가 포함된 rtmpdump, rtmpsrv 를 받아서 돌려보고, 소스를 살펴보아야할 것 같음

4. 끝날 때, FCUnpublish 명령이 호출됨, 
- wireshark 로 보면 destroyStream 이 호출되는 것으로 나옴
- 서버에서 message parsing 실패됨
- 정상적으로 작업이 끝나는 것인지는 해당 message 를 처리하고 나면 알 수 있을 것으로 예상함

chunk size 128 이라는 값 때문에, data 를 받지 못하는 게 아닌가 하는 추정.
- chunk size 가 setChunkSize message 로 변경되도록 구현되어있음. 
- 기본 값은 128 임 (by spec)
- spec 의 example에 나온 message length 가 128을 넘지 않는 경우에,  처리가 잘 되어있는지를 살펴보면 될 것 같음
- spec 의 example에 나온 message length 가 128을 넘는 경우에, 처리가 잘 되어있는지를 살펴보면 될 것 같음
- Connection.cpp 에 보면, buffer size 가 8192 라는 값이 있는데, 이 값은 어디에 영향을 미치는 지 확인 필요 
 - buffer size 는 server 에서 사용하는 buffer의 max size값으로 보인다. 
 - 서버에 payload 의 max size 는 정해져있지 않고, message length 만큼으로 할당하게 되어있다.

log 의 의미
- bytes_transferred : network 에서 받은 byte 수

WAITING_MORE_PAYLOAD_DATA 으로 가는 경우도 있음
- header 정보만 있고, payload 정보가 아직 오지 않은 경우에 발생

서버에서는 time 정보는 관리되고 있지 않음. 구현이 안되어있음.
spec을 읽어봐도 어디에 사용해야하는 지 파악이 안되는것도 문제.

publish 할 때, stream Name 이 오지만, 
stream Name 이 정작 data 가 올 때는 오지 않음.
data 가 어떤 stream 의 data 인 지 알 수 있는 방법을 spec 에서도 못찾겠음
chunk id 와 message id 를 사용할까?
과연 client 는 publish 할 때의 message stream id 를 audio, video data를 보낼 때에도 사용할까? spec 에도 없는 내용이라서 알 수 없음


#2018-10-02

1. googling AMF code 
아래 코드를 주로 참고함. ffmpeg 코드보다는 이해하기 쉬웠음
코드 자체를 다 이해한 것은 아니어서 부분 참고해서 현재 서버 소스를 수정함
https://github.com/mstorsjo/rtmpdump/blob/master/librtmp/amf.h

2. @setDataFrame message 처리하기, handler 만들기
예상에 OnStatus('NetStream.Publish.Start') 를 서버에서 보내면
@setDataFrame, OnMetadata, metadata 순으로 client 가 메시지를 보내고, 
AudioData 와 VideoData 를 보내는 것 같음

어제 @setDataFrame 메시지 parsing 이 안되어 서버가 error 내보내는 부분을 에러가 안나게 skip 하게 처리해주고 나니, 
Audio, Video Data 가 들어오기 시작함

@setDataFrame 메시지는 spec 에도 없고, wireShark도 명시적으로는 parsing 하지 못하는 것 같음 
원래 서버에서도 @setDataFrame 메시지가 오는 줄은 알고 있었던 것으로 파악되는데, error 를 보내고 있었음.
ffmpeg 은 서버에서 error를 보내면 진행이 안되서, 서버 코드를 수정해서 error 를 안보내게 처리해줌 

구현안된 AMF parsing 은 담 번에 하도록 하겠음

4. 서버가 Acknowledgement message 를 보내는 것도 구현이 되어있음
RtmpParser::parse 함수에서, parse 된 byte 를 계산해서 Acknowledgement message 를 보내는 식으로 구현되어있음
spec 에서도 received byte를 보내는 것으로 되어있음

5. parser들의 함수 이름 변경.

6. 아래와 같은 로그이 어떻게 하면 남는지, 소스 코드 분석 필요
header 만 연속 4개 나오는 것인지? 로그를 잘못 남긴 것인지?
payload 로그가 안남아서 header 만 연속으로 온 것처럼 보인 것으로 예상됨

예상과 달리, payload 를 더 기다리는 코드가 동작하는데, 맞는 것인지 모르겠음

이 부분은 wireshark 도 제대로 parsing 을 못하는 것 같다.

```cpp
rtmpserver,0.1.0,2018-10-02,15:10:47.990080,Information,RtmpParser.cpp::parse_private:63,,Connection[0]:RtmpParser:HEADER_PARSING:RtmpHeader:format_type:0,chunk_stream_id:4,timestamp:0,msg_length:385,msg_type_id:8,msg_stream_id:1,abs_timestamp:0
rtmpserver,0.1.0,2018-10-02,15:10:47.990090,Information,RtmpParser.cpp::parse_private:63,,Connection[0]:RtmpParser:HEADER_PARSING:RtmpHeader:format_type:3,chunk_stream_id:4,abs_timestamp:0
rtmpserver,0.1.0,2018-10-02,15:10:47.990098,Information,RtmpParser.cpp::parse_private:63,,Connection[0]:RtmpParser:HEADER_PARSING:RtmpHeader:format_type:3,chunk_stream_id:4,abs_timestamp:0
rtmpserver,0.1.0,2018-10-02,15:10:47.990105,Information,RtmpParser.cpp::parse_private:63,,Connection[0]:RtmpParser:HEADER_PARSING:RtmpHeader:format_type:3,chunk_stream_id:4,abs_timestamp:0
rtmpserver,0.1.0,2018-10-02,15:10:47.990116,Information,RtmpParser.cpp::parse_private:83,,Connection[0]:RtmpParser:PAYLOAD_PARSING:received message:AudioMessage:header:RtmpHeader:format_type:0,chunk_stream_id:4,timestamp:0,msg_length:385,msg_type_id:8,msg_stream_id:1,abs_timestamp:0,data_len:385,data:2f ff fb 94 44 00 0c 42 14 2b 1b 90 06 49 f0 3f 22 c6 32 3c 2f 66 4b 8d 76 e6 00 3d 4d c1 
rtmpserver,0.1.0,2018-10-02,15:10:47.990139,Information,Connection.cpp::handle_read:164,,Connection[0]:handle_read:readed_in_parse:400, total readed in parse : 791
rtmpserver,0.1.0,2018-10-02,15:10:47.990146,Information,RtmpHandler.cpp::handle_request:80,,RtmpHandler[0]:handle_request:AudioMessage:AudioMessage:header:RtmpHeader:format_type:0,chunk_stream_id:4,timestamp:0,msg_length:385,msg_type_id:8,msg_stream_id:1,abs_timestamp:0,data_len:385,data:2f ff fb 94 44 00 0c 42 14 2b 1b 90 06 49 f0 3f 22 c6 32 3c 2f 66 4b 8d 76 e6 00 3d 4d c1 
rtmpserver,0.1.0,2018-10-02,15:10:47.990159,Information,RtmpHandler.cpp::handle_media_message:409,,handle Media Message for Audio or Video
```

7. paylod parsing 코드 살펴보기
윤기선님의 소스 코드부터 살펴보기
서버가 이전 chunk header 정보를 기억하고 있다.
다른 오픈소스코드도 살펴볼 필요가 있을 지도 모르겠다.

로그를 더 넣어보니, header parsing 도 실패할 때가 있었다.


8. Video data와 audio data 가 분리되어서 오는 것 같은데 어떻게 해야 원래 파일로 복원되는 것인가?
그냥 이대로 저장하고, rtmp play 요청하는 client 한테도 그대로 보내주면 되는 것인가?

9. handler 가 언제 불리는 지 아직 파악 안됨



#2018-10-01

ffmpeg -i /data/750k.mp4 -f flv rtmp://127.0.0.1:1935/live/750kStream

1. 
ffmpeg 는 첫번째 path를 connect message 의 app 정보로 보내고,
제일 마지막의 750kStream 을 stream 이름으로 보냅니다.

2.
server에서 OnBWDone 메시지 제거
ffmeg 에서 _checkbw message 를 응답으로 보내지 않는 것 확인하기
  : 서버가 OnBWDone 메시지 보내지 않으면, _checkBW 응답 보내지 않음
  : spec 에는 없는 내용이지만, googling 해보면, 서버가 OnBWDone 메시지에 대해서, client 가 _checkbw 를 보내게 되어있는 것 같습니다.

3. protocol control message, user control message 는 chunkstream id : 2, messageStreamID : 0 으로 보내야 함
rtmp command message 는 spec 에 안나와있음. 현재 서버는 그냥 client 와 동일한 값을 사용함 
ffmpeg의 경우, 3을 사용하고있음. messageStreamId 는 안쓰는 것 같음

result 에 들어가는 code 와 description 이 message 종류에 따라 바뀌어야할 것 같음... client 에서 중요하게 생각하는 지 모르겠음
 - 해보아야 알 듯

4.
서버에서 에러를 왜 보내는 지 알아봅니다.
- ffmeg 에서 releaseStream 을 createStream 하기 전에 보내는 것 같습니다.
서버는 그 응답으로 보내네요.
응답을 어떻게 보내야 하는지가 spec에 없네요. 그래서 error 를 보내는 것 같고...
아니면, 해당 stream 이 없기 때문에 error 를 보내는 것일 수도 있습니다.
현재는 무조건 error로 보내는 듯.
--> 무조건 _result message 를 보내게 수정함
--> 다른 parameter는 setting 하지 않고, transaction id 만 request 에서 보낸 transaction id를 그대로 보내게 되어있음

서버가 어떻게 reponse 를 하든, 아직은 결과가 같음
error 를 보내면, ffmpeg 이 server 가 error 를 보냈다고 메시지를 남김.
AudioData, VideoData 까지 message 보내고 끝남
단순 result 를 보내면, ffmpeg 이 server가 error 메시지를 보냈다는 메시지 남기지는 않지만,
AudioData, VideoData 까지 message 보내고 끝남


5. Publish 요청에 대한 응답으로 OnStatus 를 보내는 이유는 뭘까요?
ffmpeg은 publish messate 의 parameter로 publishingName 과 publishingType 을 보내고 있음

spec 에 따라 구현한 듯 한데, 
spec 을 보면, client 가 publish 한 경우에는 
서버는 publish result를 주어야 하는 것 같다. 이 때 transactionId 는 request 에 사용하던 값을 그대로 사용해야 한다.
이 부분은 구현이 안되어 있어서 추가함

spec 에 따르면, play, 등에서도 OnStatus message 를 서버가 client 에 보내게 되는데, 
서버가 publish 할 때에는 OnStatus를 client에 보내야하는 것 같다.
따라서 onStatus 에 transactionId 는 client 가 publish 한 transactionId 를 주지 않아도 될 것 같다.
play 시에 OnStatus message와 OnMetadata message 를 호출하는 것 처럼,
서버에서 publish 할 때, OnStatus message 와 OnMetadata message 를 차례로 호출하게 되는 것으로 예상됨

일단은 서버가 publish 하는 기능을 구현하는 것은 아니므로, 이 부분은 삭제해보도록 하겠음

OnStatus 를 보내주지 않으니까. Aduio, Video Data 가 오지 않는 것 같음.

spec 에 따르면 SteamBegin 을 보내야하는 것으로 되어있는데, 
아직 서버에 StreamBegin 을 보내는 것도 구현이 되어있지는 않음.

OnStatus 를 보내주어야 하는지? 
Adobe 문서에 따르면 OnMetaData 는 OnStatus 를 보내주어야 오는 것 같다. 

그렇지만,Audio data 와 video data 가 오게 하기 위해서, 이게 필요한 지는 못찾았다.
아래 문서를 보면, connect.success 만 되면 data가 올 것 같은데... test 상으로는 그렇지는 않은 것 같다.

https://helpx.adobe.com/adobe-media-server/dev/adding-metadata-live-stream.html

6.
spec 을 보면 OnMetaData message 는 서버가 play 하거나 publish 할 때는 서버가 호출하게 되지만,
client 가 publish 할 때는 client 가 호출 하는 message 라고 되어있는 것 같음.

7.
AudioData
VideoData 처리가 필요할 듯
응답으로 어떤 게 가야하는 지는 spec 에 없음

server 에 있는 구현 
@setDataFrame 은 spec 에 없지만 구현되는 듯, 마는 듯 했음




###### 7. server 에서 OnStatus message 를 보냅니다.
server 에서 왜 OnStatus message 를 보내는지 알아봅시다.

6.
write_body_payload 함수가 쓰기 불편한 것으로 보인다.
writed 계산을 자동으로 할 수 있게 수정함

7.
googling 결과로 잡힌 ffmpeg 의 소스 코드를 잠시 살펴봄
https://www.ffmpeg.org/doxygen/2.6/rtmpproto_8c_source.html



참고 코드: java player code
```java
private void handleRxInvoke(Command invoke) {
    String commandName = invoke.getCommandName();
    switch (commandName) {
      case "_error":
        try {
          String description = ((AmfString) ((AmfObject) invoke.getData().get(1)).getProperty(
              "description")).getValue();
          Log.i(TAG, description);
          if (description.contains("reason=authfailed")) {
            connectCheckerRtmp.onAuthErrorRtmp();
            connected = false;
            synchronized (connectingLock) {
              connectingLock.notifyAll();
            }
          } else if (user != null
              && password != null
              && description.contains("challenge=")
              && description.contains("salt=")) {
            onAuth = true;
            try {
              shutdown(false);
            } catch (Exception e) {
              e.printStackTrace();
            }
            rtmpSessionInfo = new RtmpSessionInfo();
            rtmpDecoder = new RtmpDecoder(rtmpSessionInfo);
            if (!tlsEnabled) {
              socket = new Socket(host, port);
            } else {
              socket = CreateSSLSocket.createSSlSocket(host, port);
              if (socket == null) throw new IOException("Socket creation failed");
            }
            inputStream = new BufferedInputStream(socket.getInputStream());
            outputStream = new BufferedOutputStream(socket.getOutputStream());
            Log.d(TAG, "connect(): socket connection established, doing handshake...");
            salt = Util.getSalt(description);
            challenge = Util.getChallenge(description);
            opaque = Util.getOpaque(description);
            handshake(inputStream, outputStream);
            rxPacketHandler = new Thread(new Runnable() {
              @Override
              public void run() {
                handleRxPacketLoop();
              }
            });
            rxPacketHandler.start();
            sendConnectAuthPacketFinal(user, password, salt, challenge, opaque);
          } else if (description.contains("code=403") && user == null || password == null) {
            connectCheckerRtmp.onAuthErrorRtmp();
            connected = false;
            synchronized (connectingLock) {
              connectingLock.notifyAll();
            }
          } else {
            connectCheckerRtmp.onConnectionFailedRtmp(description);
            connected = false;
            synchronized (connectingLock) {
              connectingLock.notifyAll();
            }
          }
        } catch (Exception e) {
          connectCheckerRtmp.onConnectionFailedRtmp(e.getMessage());
          connected = false;
          synchronized (connectingLock) {
            connectingLock.notifyAll();
          }
        }
        break;
      case "_result":
        // This is the result of one of the methods invoked by us
        String method = rtmpSessionInfo.takeInvokedCommand(invoke.getTransactionId());

        Log.i(TAG, "handleRxInvoke: Got result for invoked method: " + method);
        if ("connect".equals(method)) {
          if (onAuth) {
            connectCheckerRtmp.onAuthSuccessRtmp();
            onAuth = false;
          }
          // Capture server ip/pid/id information if any
          // We can now send createStream commands
          connected = true;
          synchronized (connectingLock) {
            connectingLock.notifyAll();
          }
        } else if ("createStream".contains(method)) {
          // Get stream id
          currentStreamId = (int) ((AmfNumber) invoke.getData().get(1)).getValue();
          Log.d(TAG, "handleRxInvoke(): Stream ID to publish: " + currentStreamId);
          if (streamName != null && publishType != null) {
            fmlePublish();
          }
        } else if ("releaseStream".contains(method)) {
          Log.d(TAG, "handleRxInvoke(): 'releaseStream'");
        } else if ("FCPublish".contains(method)) {
          Log.d(TAG, "handleRxInvoke(): 'FCPublish'");
        } else {
          Log.w(TAG, "handleRxInvoke(): '_result' message received for unknown method: " + method);
        }
        break;
      case "onBWDone":
        Log.d(TAG, "handleRxInvoke(): 'onBWDone'");
        break;
      case "onFCPublish":
        Log.d(TAG, "handleRxInvoke(): 'onFCPublish'");
        break;
      case "onStatus":
        String code =
            ((AmfString) ((AmfObject) invoke.getData().get(1)).getProperty("code")).getValue();
        Log.d(TAG, "handleRxInvoke(): onStatus " + code);
        if (code.equals("NetStream.Publish.Start")) {
          onMetaData();
          // We can now publish AV data
          publishPermitted = true;
          synchronized (publishLock) {
            publishLock.notifyAll();
          }
        } else if (code.equals("NetConnection.Connect.Rejected")) {
          netConnectionDescription = ((AmfString) ((AmfObject) invoke.getData().get(1)).getProperty(
              "description")).getValue();
          publishPermitted = false;
          synchronized (publishLock) {
            publishLock.notifyAll();
          }
        }
        break;
      default:
        Log.e(TAG, "handleRxInvoke(): Unknown/unhandled server invoke: " + invoke);
        break;
    }
  }
```




TODO:
- reading open source for understanding RTMP publish message flow and publish AMF type commands and parameters
- state machine for aggregating of chunks by handling timestamp
- state machine for missing chunk by handling transport window message

### FFMpeg 으로 loacl file 을 rtmp 로 publishing test

ffmpeg -i /data/750k.mp4 -f flv rtmp://127.0.0.1:1935/750k

1. handshake 통과합니다.
2. 중간에 server 가 connection fail message 를 보냅니다만... 
3. ffmeg 은 무시하고 ffmpeg publish message 를 시작으로 audio, video data 를 보내기 시작하지만 중간에 끊깁니다.

4. 그러다가 ffmpeg 도 통신을 그만 두는 것 같습니다.
5. 4번의 원인 파악은 안된 상태입니다.

아래는 Wireshark 로 살펴본 내용들입니다.

#### 1. handshake 

ffmpeg 의 c1 값이 스펙 문서와 좀 다른 것 같습니다. zero 값을 보내야 하는 필드에 0 값이 아닌 값이 들어있었습니다.

server 는 c1 필드의 첫번째 값을 현재시간으로 보냈습니다. 어떤 값을 보내야하는지 스펙을 읽어봐도 모르겠네요.

스펙의 c1 의 timestamp field 에 대한 설명 : 
> a timestamp, which SHOULD be used as the epoch for all future chunks sent from this endpoint. This may be 0, or some arbitrary value. To synchronize multiple chunkstreams, the endpoint may wish to send the current value of the other chunkstream's timestamp. 

the other chunkstream's timestamp 가 의미하는 바는 뭘까? the other 이니까 이미 chunkstream 이 하나 있는데, 하나 더 있는 chunkstream의 timestamp값을 의미하는건지? 아니면 future chunkstream 의 timestamp 값을 의미하는 건지? 

c2 값의 경우, ffmpeg 은 이상이 없어보이지만, server 는 스펙과 다르게 보냈 것 같습니다. 

ffmpeg 의 c2 
의 time1 에 s1이 보낸 시간을 copy 해서 사용했습니다.
time2 는 c1을 읽은 시간이 되어야 하는데, 0 이 사용되었습니다.
data 에는 s1 의 data 가 copy 되어야 하는데, 그렇게 되어있습니다.

server 의 s2 
time1 에는 c1 의 time copy 되고,
time2 는 c1을 읽은 시간이 되어야 하고,
data 에는 c1 의 data 가 copy 되어야 하는데, 그렇게 되어있지 않습니다.

**server에서 보낸 s2 메시지가 잘못되어있기 때문에, 
handshake 가 실패할 수도 있을 것 같은데,
fail 없이 그냥 진행되었습니다.**

##### 참고 : handshake captured

###### ffmpeg : c0+c1, c2
c0 : protocol ver : 1 byte 
03

c1 : handshake data : 1536 byte
time : 00 00 00 00 // 0 를 보내도 됩니다.
zero : 09 00 7c 02 // 이 필드는 zero 를 보내야하는 것 같은데요?
randomData : 1528 byte
09007c02f778551eceab8e1e362f07c5868a70b2...

c2 : handshake data : 1536 byte
time : 5b a4 7c 4c // spec 대로 s1의 copy 값을 보냈습니다.
zero : 00 00 00 00 // s1 을 읽은 시간을 보내야 합니다.... 0일수도 있나봅니다.
randomData : 1528 byte
485aa89684be490ec157e0308c05ddef...

###### server : s0+s1+s2

s0 : protocol ver : 1 byte 
3

s1 : handshake data : 1536 byte
time : 59 a4 7c 4c 
zero : 00 00 00 00
randomData : 1528 byte
485aa89684be490ec157e0308c05ddef...

s2 : handshake data : 1536 byte
time1 : 00 fe ef b1 // c1의 값을 copy 해야 합니다만 다르게 구현한 것 같습니다.
time2 : f9 cc c2 f4 // c1을 읽은 시간을 보내야 합니다만 다르게 구현한 것 같습니다.
randomData : 1528 byte
0699320990ad4cb5e431fb60c1acbdd5...

#### 1. Connect and Publish

connect 에서 이루어진 data 를 capture 해보면, spec 과는 다르게 진행되는 것 같습니다. 현재 rtmp 서버에서 spec 에 상세하게 나오지 않을 부분도 이미 구현했기 때문인 것도 같고, 버그가 있을 수도 있다는 생각도 듭니다. 

ffmpeg -i /data/750k.mp4 -f flv "rtmp://127.0.0.1:1935/750k"

-f flv option 을 사용하면 이렇게 되는 것인지 모르겠지만,
요청 uri 의 첫번째 값인 "750k" 를 connect 명령의 "app" property 의 값으로 사용하는 것 같습니다. 
depth 를 두 개 이상 주어서 test해보면 더 정확히 알 수 있을 것 같습니다.

server 는 connect message 에 대한 reponse 를 보내기 전에 다른 message 를 보내고 있습니다. spec 상으로도 그렇게 할 수 있다고 나와있습니다.

connect fail message 가 보이지만, publish, audio data, video data 전송까지 일부 이루어지다가 끝이 납니다.
통신이 계속 진행되지 않는 원인에 대한 분석은 진행되지 않았습니다.

###### 1. ffmpeg 은 chunk stream format 으로 Connect message를 보냅니다.
parameter 들이 AMF 형태로 전달됩니다.

###### 2. server 는 OnBWDone message를 보냅니다. spec 에는 없는 내용입니다.
parameter로 Number 0, Null, Number 8192 를 보냅니다.

###### 3. ffmpeg 은 _checkbw message 를 보냅니다. spec 에는 없는 내용입니다. 
parameter로 Number 5, Null 을 보냅니다. 
OnBWDone 에 대한 응답으로 추정됩니다. 

###### 4. server 에서 _error message 를 보냅니다.
parameter로 Number 2, Null, {level:"error", code:"NetConnection.Call.Failed", description:"Specified stream not found in call to releaseStream" } 을 보냅니다.

###### 5. server 에서 _result message 를 보냅니다.
parameter로 Number 4, Null, Number 1 을 보냅니다.

이 정도에서 서버가 connection 실패했을 것 같은데,
그럼에도 불구하고,
###### 6. ffmpeg 에서는 publish message 를 보냅니다.
parameter로 Number 6, Null, String "", String "live" 를 보냅니다.

이 정도에서 서버가 connection 실패했을 것 같은데,
그럼에도 불구하고, 서버에서도 
###### 7. server 에서 OnStatus message 를 보냅니다.
parameter로 Number 6, Null, {level:"status", description:"Stream 'livestream' is now published", details:"livestream", clientid:"9_1_38567520"} 을 보냅니다.

###### 8. ffmpeg 에서 Audio data 를 보냅니다.
spec 에 payload 의 format 에 관한 내용은 포함되어있지 않습니다.
? Wireshark 에는 capture 되지도 않은 timestamp 값이 나오는 걸 봐서 이전 packet 의 timestamp 를 기억하는게 아닐까 싶네요. 

Header : 1 byte
Body : 385 byte

Wireshark는 Payload의 제일 처음 1 byte 
0x2f(0010 1111) 라는 값을 가지고 MP3 55khz 16 bit stereo 라고 해석합니다.
뭔가 알려진 규격이 있나봅니다. 
이후에는 data (384 byte) 가 나옵니다.

Wireshark 의 RTMP Body 해석
Control : 0x2f (MP3 55khz 16 bit stereo)
  0010.... : Format : MP3(2)
  ....11.. : Sample rate : 44 kHz (3)
  ......1. : Sample size : 16 bit (1)
  .......1 : Channels :stereo (1)
Audio data: fffb9444e68002e33a507b0912e85b63c9cf3cc38609f85f...

###### 9. ffmpeg 에서 Video data 를 보냅니다.
Header : 8 byte
Body : 6107 byte
control 1byte
Video data 6106 byte

Wireshark 의 RTMP Body 해석
Control: 0x12 (keyframe Sorensen H.263)
  0001 .... = Type: keyframe (1)
  .... 0010 = Format: Sorensen H.263 (2)
Video data: 0000843c814000b413a632323233feff319191919ff7f98c...


여기서 끊깁니다. 
아직 server 소스를 분석하지 못해서,
client 문제인지 server문제로 끊겼는지는 모르겠습니다.


##### 참고 : connect captured

###### ffmeg : connect

ChunkHeader : RTMP header 

BasicHeader: 1byte
Format(fmt) : ChunkType : 2bit
0
ChunkStreamID : 6bit
3
MessageHeader: chunktype 0 header size : 11byte
TimeStamp : 3byte
00 00 00
MessageLength : BodySize : 3byte 
00 00 8b // 139 byte
MessageTypeId : 1byte
0x14 // amf0 command
MessageStreamId : 4byte
00 00 00 00

ChunkData : RTMP Body 
0x02 // String type
0x00 0x07 // length : 7
0x63 6f 6e 65 63 67 // "connect"
0x00 // Number type
0x3f f0 00 00 00 00 00 00 //1
0x03 // Object type : Begin of Object
0x00 03 // Object property name : length 3
0x61 70 70 // "app"
0x02 // Object property value : String type
0x00 04 // length 4
0x37 35 30 6b // "750k"
// 이하 dump 생략하고 내용만 남김
"type" : Object property name
"nonprivate" : Object property value 
"falshVer" : Objet property name
"FMLE/3.0 (compatible; Lavf57.83.100)" : Objet property value
"tcUrl" : Object property name 
"rtmp://127.0.0.1:1935/750k" : Object property value
0x00 00 09 // End of Object Marker

이후 생략






























