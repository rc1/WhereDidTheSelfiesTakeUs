#pragma once

#include "ofMain.h"

#define SELFIES_DISPLAY_HEIGHT 1366
#define SELFIES_DISPLAY_WIDTH 768
#define SELFIES_DISPLAY_VIDEO_DIR "./videos/"
#define SELFIES_CAPTURE_NEW_VIDEO_IMAGE "newVideo.png"

class ofApp : public ofBaseApp {
 public:
    void setup ();
    void exit ();
    void update ();
    void draw ();
		
    void keyPressed ( int key );
    void keyReleased ( int key );
    void windowResized ( int w, int h);
    void gotMessage ( ofMessage msg );
    
    ofVideoPlayer videoPlayerA;
    ofVideoPlayer videoPlayerB;
    ofVideoPlayer *activeVideoPlayer;
    ofVideoPlayer *inactiveVideoPlayer;
    bool isLoadingNewVideo;
    
    string currentVideoFilename;
    string nextUpVideoPath;
    int newVideoDisplayCount;
    ofImage newVideoImage;
    
};
