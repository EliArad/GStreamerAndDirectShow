GStreamer and directshow

The repo start with basic GStreamer setup in Linux ubuntu

using gst_launch-1.0 I create sinks that play file, encode to h264 , decode
and more.

The first example ( 22_5_2018)  is streaming a videotestsrc  to x264enc  and
upsink it to windows host via udp sink.

I create a direct show UDP Push filter that bind to local address , connect
it to Elecard Demultiplexer and then decode it on screen.

All steps are documented , source is here.

Next tasks
1. Do the same with Linux embedded 
2. Support encryption. 

