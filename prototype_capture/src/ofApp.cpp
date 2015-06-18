#include "ofApp.h"

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );

    // ofSetFullscreen( true );

#ifdef TARGET_LINUX_ARM
    ofLogNotice() << "Using OMX Camera (I am a raspberry pi)" << endl;
    // Camera Settings
    omxCameraSettings.width = SELFIES_WIDTH; // default 1280
    omxCameraSettings.height = SELFIES_HEIGHT; // default 720
    omxCameraSettings.isUsingTexture = true; //default true
    omxCameraSettings.doRecording = false;   //default false

    // Create the grabber
    videoGrabber.setup( omxCameraSettings );
#else
    ofLogNotice() << "Using video grabber (I am a NOT raspberry pi)" << endl;
    videoGrabber.setDeviceID( 0 );
    videoGrabber.initGrabber( SELFIES_WIDTH, SELFIES_HEIGHT );
#endif

    // Onion Skin
    onionSkinSettings.numberFrames = 12 * 3;
    onionSkinSettings.show = 4;
    onionSkinSettings.width = SELFIES_WIDTH / 4;
    onionSkinSettings.height = SELFIES_HEIGHT / 4;
    

    shouldCaptureFrame = false;
    shouldDrawOnionSkin = true;
    shouldSave = false;
    shouldDrawHUD = false;
#ifndef TARGET_LINUX_ARM
    shouldDrawHUD = true;
#endif

    onionskin.init( onionSkinSettings );

    taskRunner.startThread( true, false ); 
}

void ofApp::exit () {
    taskRunner.stopThread();
}

void ofApp::update () {
    #ifndef TARGET_LINUX_ARM
    videoGrabber.update();
    #endif
}

void ofApp::draw () {
    
    static int frameCounter = 0;
    static int sequenceStartTime = ofGetUnixTime();

    
    if ( shouldSave ) {
        
        // Run a gst pipeline to make the video
        string makeVideo;
#ifdef TARGET_LINUX_ARM
        makeVideo = "gst-launch-1.0 multifilesrc location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + "-%d.png" ) + "\" index=1 caps=\"image/png,framerate=\\(fraction\\)12/1\" ! pngdec ! videoconvert ! videorate ! theoraenc ! oggmux ! filesink location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + "\"";
#else
        // See INSTALL.md on the mac if this is not working
        makeVideo = "/usr/local/bin/gst-launch-1.0 multifilesrc location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + "-%d.png" ) + "\" index=1 caps=\"image/png,framerate=\\(fraction\\)12/1\" ! pngdec ! videoconvert ! videorate ! theoraenc ! oggmux ! filesink location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + "\"";
        ofLogNotice() << makeVideo;
#endif
        taskRunner.addCommand( makeVideo );

        // Delete the files
        string deletePngs = "rm " + ofToDataPath( ofToString( sequenceStartTime ) + "-*.png" );
        taskRunner.addCommand( deletePngs );

        // Reset counters
        sequenceStartTime = ofGetUnixTime();
        frameCounter = 0;
        shouldSave = false;
    }
    

    // Capture a new frame to the buffer
    if ( shouldCaptureFrame ) {
        // Add the onion skin
        onionskin.getCurrentFboPtr()->begin();
        ofClear( 0, 0, 0, 0 );
        ofPushMatrix();
        ofScale( float( onionskin.settings.width ) / float( SELFIES_WIDTH ), float( onionskin.settings.height ) / float( SELFIES_HEIGHT ), 1.0 );
        #ifdef TARGET_LINUX_ARM
        videoGrabber.draw();
        #else
        videoGrabber.draw( 0, 0 );
        #endif
        
        ofPopMatrix();
        onionskin.getCurrentFboPtr()->end();

        // Save to disk
        // Why are we saving the onion skin and not the video images here?
        ofPixels pix;
        //onionskin.getCurrentFboPtr()->readToPixels( pix );
        videoGrabber.getTextureReference().readToPixels( pix );
        ofSaveImage( pix, ofToString( sequenceStartTime ) + "-" + ofToString( frameCounter++ ) + ".png" );
        
        // Add a frame number
        
        
        // Draw the onion skin
        onionskin.renderAll();
        onionskin.next();
        shouldCaptureFrame = false;
    }

    #ifdef TARGET_LINUX_ARM
    videoGrabber.draw();
    #else
    videoGrabber.draw( 0, 0 );
    #endif
    
    if ( shouldDrawOnionSkin ) {
        onionskin.layer.draw( 0, 0, SELFIES_WIDTH, SELFIES_HEIGHT );
    }

    static int currentFrame = 0;
    static float lastUpdate = 0;
    if ( ofGetElapsedTimef() - lastUpdate > 1.0f / 12.0f ) {
        if ( ++currentFrame >= onionskin.settings.numberFrames ) {
            currentFrame = 0;
        }
        lastUpdate = ofGetElapsedTimef();
    }
    
    
    onionskin.fbos[ currentFrame ]->draw( 20, 20, ofGetWidth() * 0.2, ofGetHeight() * 0.2 );
    
    if ( shouldDrawHUD ) {
        ofDrawBitmapStringHighlight( "[0-5] Onion Skin Blend", ofPoint( 200, 20 ) );
        ofDrawBitmapStringHighlight( "[o] Toggle Onion Skin", ofPoint( 200, 40 ) );
        ofDrawBitmapStringHighlight( "[space] Take Frame", ofPoint( 200, 60 ) );
        ofDrawBitmapStringHighlight( "[s] Save video & new session", ofPoint( 200, 80 ) );
        ofDrawBitmapStringHighlight( "[h] Show this display", ofPoint( 200, 100 ) );
    }
 
    
}

void ofApp::keyPressed ( int key ) {
   
    if ( key == ' ' ) {
         shouldCaptureFrame = true;
    }
    else if ( key == '0' ) {
        onionskin.settings.blendMode = OF_BLENDMODE_DISABLED;
    }
    else if ( key == '1' ) {
        onionskin.settings.blendMode = OF_BLENDMODE_ALPHA;
    }
    else if ( key == '2' ) {
        onionskin.settings.blendMode = OF_BLENDMODE_ADD;
    }
    else if ( key == '3' ) {
        onionskin.settings.blendMode = OF_BLENDMODE_SUBTRACT;
    }
    else if ( key == '4' ) {
        onionskin.settings.blendMode = OF_BLENDMODE_MULTIPLY;
    }
    else if ( key == '5' ) {
        onionskin.settings.blendMode = OF_BLENDMODE_SCREEN;
    }
    else if ( key == 'o' ) {
        shouldDrawOnionSkin = !shouldDrawOnionSkin;
    }
    else if ( key == 's' ) {
        shouldSave = true;
    }
    else if ( key == 'h' ) {
        shouldDrawHUD =! shouldDrawHUD;
    }
}

void ofApp::keyReleased ( int key ) {

}

void ofApp::windowResized ( int w, int h ) {

}

void ofApp::gotMessage( ofMessage msg ) {

}
