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

#define SELFIES_CAPTURE_HEIGHT 460
#define SELFIES_CAPTURE_WIDTH 800
#ifdef TARGET_OSX
    #define SELFIES_CAPTURE_THROTTLE_SEC 0.3f
#else
    #define SELFIES_CAPTURE_THROTTLE_SEC 2.0f
#endif
#define SELFIES_CAPTURE_DIGIT_IMAGE "CounterDigits.png"
#define SELFIES_CAPTURE_SAVING_IMAGE "Saving.png"

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
