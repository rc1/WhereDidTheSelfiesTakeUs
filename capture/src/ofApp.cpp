#include "ofApp.h"

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );

    // ofSetFullscreen( true );

#ifdef TARGET_LINUX_ARM
    ofLogNotice() << "Using OMX Camera (I am a raspberry pi)" << endl;
    // Camera Settings
    omxCameraSettings.width = SELFIES_CAPTURE_WIDTH; // default 1280
    omxCameraSettings.height = SELFIES_CAPTURE_HEIGHT; // default 720
    omxCameraSettings.isUsingTexture = true; //default true
    omxCameraSettings.doRecording = false;   //default false

    // Create the grabber
    videoGrabber.setup( omxCameraSettings );
#else
    ofLogNotice() << "Using video grabber (I am a NOT raspberry pi)" << endl;
    videoGrabber.setDeviceID( 0 );
    videoGrabber.initGrabber( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT );
#endif

    // Onion Skin
    onionSkinSettings.numberFrames = 12 * 3;
    onionSkinSettings.show = 4;
    onionSkinSettings.width = SELFIES_CAPTURE_WIDTH / 4;
    onionSkinSettings.height = SELFIES_CAPTURE_HEIGHT / 4;
    
    shouldCaptureFrame = false;
    shouldDrawOnionSkin = true;
    shouldSave = false;
    shouldDrawHUD = false;
#ifndef TARGET_LINUX_ARM
    shouldDrawHUD = true;
#endif

    frameFbo.allocate( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT, GL_RGB, OF_PIXELS_BGRA /*GL_RGBA32F*/ );
    
    videoPlayer.setPixelFormat( OF_PIXELS_RGBA );
    videoPlayer.loadMovie( "Overlay.mov" );
    videoPlayer.setLoopState( OF_LOOP_NORMAL );
    videoPlayer.setFrame( 0     );

    onionskin.init( onionSkinSettings );

#ifdef TARGET_OSX
    if( !imageSaver.setup( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT, 20 ) ) { // last params is number of preallocated frames
        printf( "error: cannot start the screen grab saver.\n" );
        ::exit(EXIT_FAILURE);
    }
#endif

    taskRunner.startThread( true, false ); 
}

void ofApp::exit () {
    taskRunner.stopThread();
}

void ofApp::update () {
    #ifndef TARGET_LINUX_ARM
    videoGrabber.update();
    #endif
    videoPlayer.update();
}

void ofApp::draw () {
    
    static int frameCounter = 0;
    static int sequenceStartTime = ofGetUnixTime();

    frameFbo.begin();
    
    //#�Video Drawing
    ofPushMatrix();
    ofScale( -1, 1, 1 );
    ofTranslate( -SELFIES_CAPTURE_WIDTH, 0 );
#ifdef TARGET_LINUX_ARM
    videoGrabber.draw();
#else
    videoGrabber.draw( 0, 0 );
#endif
    ofPopMatrix();
    
    videoPlayer.draw( 0, 0, SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT );
    frameFbo.end();
    frameFbo.draw( 0, 0 );

    if ( shouldSave && frameCounter > 5 ) {
        
        // Run a gst pipeline to make the video
        string makeVideo;
#ifdef TARGET_LINUX_ARM
        makeVideo = "gst-launch-1.0 multifilesrc location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + "-%d.png" ) + "\" index=0 caps=\"image/png,framerate=\\(fraction\\)15/1\" ! pngdec ! videoconvert ! videorate ! theoraenc ! oggmux ! filesink location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + "\"";
#else
        // See INSTALL.md on the mac if this is not working
        // Note: index is appears to be 0 on OSX
        makeVideo = "/usr/local/bin/gst-launch-1.0 multifilesrc location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + "-%d.png" ) + "\" index=0 caps=\"image/png,framerate=\\(fraction\\)15/1\" ! pngdec ! videoconvert ! videoflip method=vertical-flip ! videorate ! theoraenc ! oggmux ! filesink location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + "\"";
        ofLogNotice() << makeVideo;
#endif
        taskRunner.addCommand( makeVideo );
        
        // Remove any zero byte files that make have happened
        taskRunner.addCommand( "find " + ofToDataPath( "." ) + " -type f -size 0 | xargs rm" );
        
        // Set a .lock file in the target video dir (to stop the player app from loading an incomplete viode)
        taskRunner.addCommand( "touch " + ofToDataPath( SELFIES_CAPTURE_TARGET_DIR ) + "lock" );
        
        // Move the files
        taskRunner.addCommand( "mv " + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + " " + ofToDataPath( SELFIES_CAPTURE_TARGET_DIR ) );
        
        // Remove the lock file
        taskRunner.addCommand( "rm " + ofToDataPath( SELFIES_CAPTURE_TARGET_DIR ) + "lock" );
        
        ofLogVerbose() << "rm " + ofToDataPath( SELFIES_CAPTURE_TARGET_DIR ) + "lock" ;
    

        // #�Delete the files
        // Delete only the sequence files
        //string deletePngs = "rm " + ofToDataPath( ofToString( sequenceStartTime ) + "-*.png" );
        // Delete them all
        string deletePngs = "rm " + ofToDataPath( "*.png" );
        taskRunner.addCommand( deletePngs );

        // Reset counters
        sequenceStartTime = ofGetUnixTime();
        frameCounter = 0;
        shouldSave = false;
    }
    
    bool isSaving = taskRunner.queueSize() > 0;

    // Capture a new frame to the buffer
    if ( !isSaving && shouldCaptureFrame ) {
        // Add the onion skin
        onionskin.getCurrentFboPtr()->begin();
        ofClear( 0, 0, 0, 0 );
        ofPushMatrix();
        ofScale( float( onionskin.settings.width ) / float( SELFIES_CAPTURE_WIDTH ), float( onionskin.settings.height ) / float( SELFIES_CAPTURE_HEIGHT ), 1.0 );
        frameFbo.draw( 0, 0 );
        
        ofPopMatrix();
        onionskin.getCurrentFboPtr()->end();

        // Save to disk
#ifdef TARGET_LINUX_ARM
        ofPixels pix;
        frameFbo.readToPixels( pix );
        ofSaveImage( pix, ofToString( sequenceStartTime ) + "-" + ofToString( frameCounter++ ) + ".png" );
#else
        if ( !imageSaver.grabFbo( ofToString( sequenceStartTime ) + "-" + ofToString( frameCounter++ ) + ".png", frameFbo ) ) {
            ofLogError() << "Failed to grab screen. Maybe screen grab buffer is set to low";
        }
#endif
        
        videoPlayer.nextFrame();
        
        // Draw the onion skin
        onionskin.renderAll();
        onionskin.next();
        shouldCaptureFrame = false;
    }
    
    if ( shouldDrawOnionSkin ) {
        onionskin.layer.draw( 0, 0, SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT );
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
    
    if ( isSaving ) {
        ofPushStyle();
        ofSetColor( 255, 0, 0 );
        ofCircle( ofGetWidth() - 20.0f, 20.0f, 10.0f );
        ofPopStyle();
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
