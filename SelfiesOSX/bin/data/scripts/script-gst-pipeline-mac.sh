#/bin/bash

# Script to make a video out of a seqeuence of images.
# First argument is the sequence number

# Arguments
SEQUENCE_ID=$1
ANIMATIONS_DIR=$2
FRAMES_DIR=$3

echo $SEQUENCE_ID $ANIMATIONS_DIR $CAPTURES_DIR

mkdir -p $ANIMATIONS_DIR

touch "$ANIMATIONS_DIR/lock"

# Create the file
# /usr/local/bin/gst-launch-1.0 multifilesrc location="$FRAMES_DIR/$SEQUENCE_ID-%d.png" index=0 caps="image/png,framerate=\(fraction\)15/1" ! pngdec ! videoconvert ! videoflip method=vertical-flip ! videorate ! qtmux ! filesink location="$ANIMATIONS_DIR/$SEQUENCE_ID.mov"


# /usr/local/bin/gst-launch-1.0 multifilesrc location="$FRAMES_DIR/$SEQUENCE_ID-%d.png" index=0 caps="image/png,framerate=\(fraction\)15/1" ! pngdec ! videoconvert ! videoflip method=vertical-flip ! videorate ! avimux ! filesink location="$ANIMATIONS_DIR/$SEQUENCE_ID.mov"

/usr/local/bin/ffmpeg -framerate 15/1 -i "$FRAMES_DIR/$SEQUENCE_ID-%d.png" -c:v libx264 -r 30 -pix_fmt yuv420p "$ANIMATIONS_DIR/$SEQUENCE_ID.mp4"

# Remove the captures
rm $FRAMES_DIR/*.png

# Remove the loci
rm "$ANIMATIONS_DIR/lock"

# Remove any zero byte files (including lock file)
find $TARGET_DIR -type f -size 0 -not -path '*/\.*' | xargs rm


