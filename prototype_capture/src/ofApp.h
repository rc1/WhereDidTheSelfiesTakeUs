#pragma once

#include "ofMain.h"
#include "OnionSkinRingBuffer.h"
#include "ThreadedRunCommand.h"

#ifdef TARGET_LINUX_ARM
#include "ofxRPiCameraVideoGrabber.h"
#endif

#ifdef TARGET_OSX
#include "ThreadedImageSaver.h"
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

    ofFbo frameFbo;
    
    OnionSkinRingBufferSettings onionSkinSettings;
    OnionSkinRingBuffer onionskin;

    ThreadedRunCommand taskRunner;

#ifdef TARGET_OSX
    ThreadedImageSaver imageSaver;
#endif

    bool shouldCaptureFrame;
    bool shouldDrawOnionSkin;
    bool shouldSave;
    bool shouldDrawHUD;
};
