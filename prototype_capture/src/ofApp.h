#pragma once

#include "ofMain.h"
#include "OnionSkinRingBuffer.h"
#include "ThreadedRunCommand.h"
#ifdef OF_TARGET_LINUXARMV7L
#include "ofxRPiCameraVideoGrabber.h"
#endif

#define SELFIES_HEIGHT 768
#define SELFIES_WIDTH 1024

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

    #ifdef OF_TARGET_LINUXARMV7L
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
