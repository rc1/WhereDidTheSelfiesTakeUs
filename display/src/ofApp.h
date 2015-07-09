#pragma once

#include "ofMain.h"
#ifdef TARGET_LINUX_ARM
#include "ofxOMXPlayer.h"
#endif

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

#ifdef TARGET_OSX
    ofVideoPlayer videoPlayerA;
    ofVideoPlayer videoPlayerB;
    ofVideoPlayer *activeVideoPlayer;
    ofVideoPlayer *inactiveVideoPlayer;
#else
    ofxOMXPlayer videoPlayerA;
    ofxOMXPlayer videoPlayerB;
    ofxOMXPlayer *activeVideoPlayer;
    ofxOMXPlayer *inactiveVideoPlayer;
#endif
    bool isLoadingNewVideo;
    
    string currentVideoFilename;
    string nextUpVideoPath;
    int newVideoDisplayCount;
    ofImage newVideoImage;
    
};
