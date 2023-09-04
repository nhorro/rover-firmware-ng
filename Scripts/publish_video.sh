DEST_IP=192.168.1.11
DEST_PORT=5004
gst-launch-1.0 -vvv v4l2src device=/dev/video0 \
    ! "image/jpeg,width=320,height=240,framerate=30/1" \
    ! rtpjpegpay \
    ! udpsink host=$DEST_IP port=$DEST_PORT