#/bin/bash

# Script to make a video out of a seqeuence of images.
# First argument is the sequence number

# Arguments
SEQUENCE_ID=$1
DATA_PATH=$2

#Config
TARGET_DIR="$DATA_PATH../../../display/bin/data/videos"

# Create a lock file
touch "$TARGET_DIR/lock"

# Create the file
# gst-launch-1.0 multifilesrc location="$DATA_PATH/captures/$SEQUENCE_ID-%d.png" index=0 caps="image/png,framerate=\(fraction\)15/1" ! pngdec ! videoconvert ! videorate ! theoraenc ! oggmux ! filesink location="$TARGET_DIR/$SEQUENCE_ID.ogg"
 #gst-launch-1.0 multifilesrc location="$DATA_PATH/captures/$SEQUENCE_ID-%d.png" index=0 caps="image/png,framerate=\(fraction\)15/1" ! pngdec ! videoconvert ! videorate ! omxh264enc ! mpegtsmux ! filesink location="$TARGET_DIR/$SEQUENCE_ID.mp4"
gst-launch-1.0 multifilesrc location="$DATA_PATH/captures/$SEQUENCE_ID-%d.png" index=0 caps="image/png,framerate=\(fraction\)15/1" ! pngdec ! videoconvert ! videorate ! omxh264enc target-bitrate=2000000 control-rate=variable ! "video/x-h264,profile=high" ! h264parse ! mp4mux ! filesink sync=false location="$TARGET_DIR/$SEQUENCE_ID.mp4"


# Remove any zero byte files (including lock file)
find $TARGET_DIR -type f -size 0 -not -path '*/\.*' | xargs rm

# Remove the captures
rm $DATA_PATH/captures/$SEQUENCE_ID-*.png
