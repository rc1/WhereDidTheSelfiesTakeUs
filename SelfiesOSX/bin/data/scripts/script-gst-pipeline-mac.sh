#/bin/bash

# Script to make a video out of a seqeuence of images.
# First argument is the sequence number

# Arguments
SEQUENCE_ID=$1
ANIMATIONS_DIR=$2
FRAMES_DIR=$3

echo $SEQUENCE_ID $ANIMATIONS_DIR $CAPTURES_DIR

mkdir -p $ANIMATIONS_DIR

# Create the file
/usr/local/bin/gst-launch-1.0 multifilesrc location="$FRAMES_DIR/$SEQUENCE_ID-%d.png" index=0 caps="image/png,framerate=\(fraction\)15/1" ! pngdec ! videoconvert ! videoflip method=vertical-flip ! videorate ! theoraenc ! oggmux ! filesink location="$ANIMATIONS_DIR/$SEQUENCE_ID.ogg"

# Remove the captures
rm $FRAMES_DIR/$SEQUENCE_ID-*.png

# Remove any zero byte files (including lock file)
find $TARGET_DIR -type f -size 0 -not -path '*/\.*' | xargs rm


