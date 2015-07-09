#pragma once

#include "ofMain.h"
#include "OnionSkinRingBuffer.h"
#include "ThreadedExecQueue.h"

#ifdef TARGET_LINUX_ARM
#include "ofxRPiCameraVideoGrabber.h"
#include "ofxThreadedSavingImage.h"
#endif

#ifdef TARGET_OSX
#include "ThreadedImageSaver.h"
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
    
#ifdef TARGET_LINUX_ARM
    ofxRPiCameraVideoGrabber videoGrabber;
    OMXCameraSettings omxCameraSettings;
#else
    ofVideoGrabber videoGrabber;
#endif
    
    OnionSkinRingBufferSettings onionSkinSettings;
    OnionSkinRingBuffer onionskin;
    
    ofFbo frameFbo;
    ThreadedExecQueue taskRunner;
    ofVideoPlayer videoPlayer;
    ofImage digits;
    ofImage saving;
    
#ifdef TARGET_LINUX_ARM
    ofxThreadedSavingImage saveImage;
#endif
    
#ifdef TARGET_OSX
    ThreadedImageSaver imageSaver;
#endif
    
    bool shouldCaptureFrame;
    bool shouldDrawOnionSkin;
    bool shouldCreateRecording;
    bool shouldDrawHUD;
};
