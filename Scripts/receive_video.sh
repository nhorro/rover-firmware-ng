VIDEO_PORT=5004
gst-launch-1.0 -e -vvv udpsrc port=$VIDEO_PORT ! \
    application/x-rtp, encoding-name=JPEG,payload=26 ! \
    rtpjpegdepay ! jpegparse ! jpegdec ! \
    autovideosink