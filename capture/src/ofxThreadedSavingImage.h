#pragma once
#include "ofMain.h"

#ifdef TARGET_LINUX_ARM

class ofxThreadedSavingImage : public ofThread, public ofImage {
public:
    string fileName;
    
    void threadedFunction() {
        if( lock() ) {
            saveImage(fileName);
            unlock();
        } else {
            printf( "ofxThreadedSavingImage - cannot save %s cos I'm locked", fileName.c_str() );
        }
        stopThread();
    }
    
    void saveThreaded(string fileName) {
        this->fileName = fileName;
        startThread( false );   // blocking, verbose
    }  
};

#endif