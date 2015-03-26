#include "ofApp.h"

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );

    // ofSetFullscreen( true );

    // Camera Settings
    omxCameraSettings.width = 1024; // default 1280
    omxCameraSettings.height = 768; // default 720
    omxCameraSettings.isUsingTexture = true; //default true
    omxCameraSettings.doRecording = false;   //default false

    // Create the grabber
    videoGrabber.setup( omxCameraSettings );

    // Onion Skin
    onionSkinSettings.numberFrames = 12 * 3;
    onionSkinSettings.show = 4;
    onionSkinSettings.width = omxCameraSettings.width / 4;
    onionSkinSettings.height = omxCameraSettings.height / 4;

    shouldCaptureFrame = false;
    shouldDrawOnionSkin = true;
    shouldSave = false;

    onionskin.init( onionSkinSettings );
}

void ofApp::update () {

}

void ofApp::draw () {

    static int frameCounter = 0;
    static int sequenceStartTime = ofGetUnixTime();

    if ( shouldSave ) {

        string cmd = "gst-launch-1.0 multifilesrc location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + "-%d.png" ) + "\" index=1 caps=\"image/png,framerate=\\(fraction\\)12/1\" ! pngdec ! videoconvert ! videorate ! theoraenc ! oggmux ! filesink location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + "\"";

        ofLogNotice() << cmd;

        system( cmd.c_str() );

        cmd = "rm " + ofToDataPath( "*.png" );

        system( cmd.c_str() );

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
        ofScale( float( onionSkinSettings.width) / float( omxCameraSettings.width ), float( onionSkinSettings.height ) / float( omxCameraSettings.height ), 1.0 );
        videoGrabber.draw();
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
    
    videoGrabber.draw();
    if ( shouldDrawOnionSkin ) {
        onionskin.layer.draw( 0, 0, omxCameraSettings.width, omxCameraSettings.height );
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
