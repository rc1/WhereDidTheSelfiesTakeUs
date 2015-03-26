#pragma once

#include "ofMain.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "OnionSkinRingBuffer.h"
#include "ThreadedRunCommand.h"

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

    ofxRPiCameraVideoGrabber videoGrabber;
    OMXCameraSettings omxCameraSettings;
    OnionSkinRingBufferSettings onionSkinSettings;

    OnionSkinRingBuffer onionskin;

    ThreadedRunCommand taskRunner;

    bool shouldCaptureFrame;
    bool shouldDrawOnionSkin;
    bool shouldSave;
};
