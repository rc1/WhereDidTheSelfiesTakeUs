#pragma once

#include "ofMain.h"

#define SELFIES_DISPLAY_HEIGHT 1366
#define SELFIES_DISPLAY_WIDTH 768
#define SELFIES_DISPLAY_VIDEO_DIR "./videos"

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
    
    ofVideoPlayer videoPlayer;
    
    string currentVideoFilename;
    
};
