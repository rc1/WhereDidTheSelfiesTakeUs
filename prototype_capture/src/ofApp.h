#pragma once

#include "ofMain.h"
#include "OnionSkinRingBuffer.h"
#include "ThreadedRunCommand.h"
#ifdef TARGET_LINUX_ARM
#include "ofxRPiCameraVideoGrabber.h"
#endif

#define SELFIES_HEIGHT 460
#define SELFIES_WIDTH 800

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

    #ifdef TARGET_LINUX_ARM
    ofxRPiCameraVideoGrabber videoGrabber;
    OMXCameraSettings omxCameraSettings;
    #else
    ofVideoGrabber videoGrabber;
    #endif
    
    OnionSkinRingBufferSettings onionSkinSettings;
    OnionSkinRingBuffer onionskin;

    ThreadedRunCommand taskRunner;

    bool shouldCaptureFrame;
    bool shouldDrawOnionSkin;
    bool shouldSave;
};
