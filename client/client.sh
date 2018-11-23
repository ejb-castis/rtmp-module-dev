ffmpeg -re -i /data/SampleVideo_720x480_1mb.mp4  -c copy -f flv rtmp://127.0.0.1:1935/myapp/sample_stream
#ffmpeg -i /data/750k.mp4 -f flv rtmp://127.0.0.1:1935/750k_mp4_stream
#ffmpeg -i /data/750k.ts -f flv rtmp://127.0.0.1:1935/live/750k_ts_stream
