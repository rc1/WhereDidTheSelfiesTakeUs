#include "ofApp.h"

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );

    // ofSetFullscreen( true );

#ifdef OF_TARGET_LINUXARMV7L
    // Camera Settings
    omxCameraSettings.width = SELFIES_WIDTH; // default 1280
    omxCameraSettings.height = SELFIES_HEIGHT; // default 720
    omxCameraSettings.isUsingTexture = true; //default true
    omxCameraSettings.doRecording = false;   //default false

    // Create the grabber
    videoGrabber.setup( omxCameraSettings );
#else
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

    onionskin.init( onionSkinSettings );

    taskRunner.startThread( true, false ); 
}

void ofApp::exit () {
    taskRunner.stopThread();
}

void ofApp::update () {
    videoGrabber.update();
}

void ofApp::draw () {
    
    static int frameCounter = 0;
    static int sequenceStartTime = ofGetUnixTime();

    
    if ( shouldSave ) {

        #ifdef OF_TARGET_LINUXARMV7L
        string makeVideo = "gst-launch-1.0 multifilesrc location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + "-%d.png" ) + "\" index=1 caps=\"image/png,framerate=\\(fraction\\)12/1\" ! pngdec ! videoconvert ! videorate ! theoraenc ! oggmux ! filesink location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + "\"";

        taskRunner.addCommand( makeVideo );

        string deletePngs = "rm " + ofToDataPath( ofToString( sequenceStartTime ) + "-*.png" );

        taskRunner.addCommand( deletePngs );
        #endif

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
        ofScale( float( SELFIES_WIDTH ) / float( SELFIES_WIDTH ), float( SELFIES_HEIGHT ) / float( SELFIES_HEIGHT ), 1.0 );
        videoGrabber.draw( 0, 0 );
        ofPopMatrix();
        onionskin.getCurrentFboPtr()->end();

        // Save to disk
        ofPixels pix;
        onionskin.getCurrentFboPtr()->readToPixels( pix );
        ofSaveImage( pix, ofToString( sequenceStartTime ) + "-" + ofToString( ++frameCounter ) + ".png" );
        
        onionskin.renderAll();
        onionskin.next();
        shouldCaptureFrame = false;
    }
    
    videoGrabber.draw( 0, 0 );
    
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
    
    onionskin.fbos[ currentFrame ]->draw( 0, 0, ofGetWidth() * 0.2, ofGetHeight() * 0.2 );
    
 
    // ofDrawBitmapStringHighlight( "[0-5] Onion Skin Blend", ofPoint( 20, 20 ) );
    // ofDrawBitmapStringHighlight( "[o] Toggle Onion Skin", ofPoint( 20, 40 ) );
    // ofDrawBitmapStringHighlight( "[space] Take Frame", ofPoint( 20, 40 ) );
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
}

void ofApp::keyReleased ( int key ) {

}

void ofApp::windowResized ( int w, int h ) {

}

void ofApp::gotMessage( ofMessage msg ) {

}
