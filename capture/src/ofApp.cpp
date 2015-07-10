#include "ofApp.h"
#ifdef TARGET_LINUX_ARM
#include "wiringPi.h"
#endif

// Config
// ======

#define SELFIES_CAPTURE_WIDTH 800
#define SELFIES_CAPTURE_HEIGHT 480
#ifdef TARGET_OSX
    #define SELFIES_CAPTURE_THROTTLE_SEC 0.3f
#else
    #define SELFIES_CAPTURE_THROTTLE_SEC 0.2f
#endif
#define SELFIES_CAPTURE_DIGIT_IMAGE "CounterDigits.png"
#define SELFIES_CAPTURE_SAVING_IMAGE "Saving.png"
#define SELFIES_BUTTON_CAPTURE_PYS_PIN 31
#define SELFIES_BUTTON_SAVE_PYS_PIN 29
#define SELFIES_BUTTON_LED_PYS_PIN 35
#define SELFIES_MIN_FRAME_COUNT 30

// Utils
// =====

// Takes an image of digits and draws a number using them
inline void drawDigitsCentered( ofImage &image, int number ) {
    float digitsWidth = image.getWidth() / 10.0f;
    ofPushMatrix();
    int numberOfDigits = ( number == 0 ) ? 1 : log10( number ) + 1;
    ofTranslate( ( numberOfDigits * digitsWidth ) / 2.0f , -image.getHeight() / 2.0f );
    int idx = 0;
    while ( number > 0 ) {
        image.drawSubsection( 0 - ( ++idx * digitsWidth ), 0, digitsWidth, image.getHeight(), (number % 10) * digitsWidth, 0 );
        number = number / 10;
    }
    ofPopMatrix();
}

// ofApp
// =====

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );
    
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
    onionSkinSettings.numberFrames = 2;
    onionSkinSettings.show = 2;
    onionSkinSettings.width = SELFIES_CAPTURE_WIDTH;
    onionSkinSettings.height = SELFIES_CAPTURE_HEIGHT;
    
    shouldCaptureFrame = false;
    shouldDrawOnionSkin = true;
    shouldCreateRecording = false;
    shouldDrawHUD = false;
#ifndef TARGET_LINUX_ARM
    shouldDrawHUD = true;
#endif
    frameCounter = 0;
    
    frameFbo.allocate( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT, GL_RGB /*GL_RGBA32F*/ );
    
    videoPlayer.setPixelFormat( OF_PIXELS_RGBA );
    videoPlayer.loadMovie( "Overlay.mov" );
    videoPlayer.setLoopState( OF_LOOP_NORMAL );
    
    // Get the last know frame
    int lastVideoFrameNumber = 0;
    ofBuffer buffer = ofBufferFromFile( ofToDataPath( "captures/.lastFrame" ) );
    if ( buffer.size() > 0 ) {
        lastVideoFrameNumber = ofToInt( buffer.getText() );
    }
    videoPlayer.setFrame( lastVideoFrameNumber );
    
    onionskin.init( onionSkinSettings );
    
#ifdef TARGET_OSX
    if( !imageSaver.setup( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT, 20 ) ) { // last params is number of preallocated frames
        printf( "error: cannot start the screen grab saver.\n" );
        ::exit(EXIT_FAILURE);
    }
#endif
    
    taskRunner.startThread( true );
    
    digits.loadImage( SELFIES_CAPTURE_DIGIT_IMAGE );
    saving.loadImage( SELFIES_CAPTURE_SAVING_IMAGE );

#ifdef TARGET_LINUX_ARM
    if ( wiringPiSetupPhys() == -1 ) {
        ofLogNotice( "Failed to enabled WiringPi" );
        ::exit(EXIT_FAILURE);
    } else {
        ofLogNotice( "Wiring Pi Enabled" );
        pinMode( SELFIES_BUTTON_CAPTURE_PYS_PIN, INPUT );
        pinMode( SELFIES_BUTTON_SAVE_PYS_PIN, INPUT );
        pinMode( SELFIES_BUTTON_LED_PYS_PIN, OUTPUT );
    }
#endif
}

void ofApp::exit () {
    taskRunner.stopThread();
}

void ofApp::update () {
#ifdef TARGET_OSX
    videoGrabber.update();
#else
    if ( !digitalRead( SELFIES_BUTTON_CAPTURE_PYS_PIN ) ) {
        shouldCaptureFrame = true;
    }
    if ( !digitalRead( SELFIES_BUTTON_SAVE_PYS_PIN ) ) {
        shouldCreateRecording = true;
    }
    static float lastLedUpdateTime = 0.0f;
    if ( ofGetElapsedTimef() - lastLedUpdateTime > 0.5f ) {
        ofLogVerbose() << (frameCounter > SELFIES_MIN_FRAME_COUNT  ? "high" : "low" );
        digitalWrite( SELFIES_BUTTON_LED_PYS_PIN, frameCounter > SELFIES_MIN_FRAME_COUNT );
        lastLedUpdateTime = ofGetElapsedTimef();
    }
#endif
    videoPlayer.update();
}

void ofApp::draw () {
    static int sequenceStartTime = ofGetUnixTime();
    static float lastCaptureTime = -10000.0f;
    ofRectangle screenRectangle( 0, 0, SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT );
    
    // Compose Live Image
    // ------------------
    frameFbo.begin();
    
    // Scale to fill and centre
    ofRectangle grabberRectangle( 0, 0, videoGrabber.getWidth(), videoGrabber.getHeight() );
    grabberRectangle.scaleTo( screenRectangle, OF_SCALEMODE_FILL );
    
    // Flip the camera, so it's like a mirror
    ofPushMatrix();
    ofScale( -1, 1, 1 );
    ofTranslate( -ofGetWidth(), 0 );
#ifdef TARGET_LINUX_ARM
    videoGrabber.getTextureReference().draw( grabberRectangle.getX(), grabberRectangle.getY(), grabberRectangle.getWidth(), grabberRectangle.getHeight() );
#else
    videoGrabber.draw( grabberRectangle );
#endif
    ofPopMatrix();
    
    // Scale to fill and centre
    ofRectangle videoRectangle( 0, 0, videoPlayer.getWidth(), videoPlayer.getHeight() );
    videoRectangle.scaleTo( screenRectangle, OF_SCALEMODE_FILL );

    videoPlayer.draw( videoRectangle );
    
    frameFbo.end();
    frameFbo.draw( 0, 0 );
    
    // Create Recording
    // ----------------
    if ( shouldCreateRecording && frameCounter > SELFIES_MIN_FRAME_COUNT ) {
        
        // Run a gst pipeline to make the video
#ifdef TARGET_LINUX_ARM
//        makeVideo = "gst-launch-1.0 multifilesrc location=\"" + ofToDataPath( "captures/" + ofToString( sequenceStartTime ) + "-%d.png" ) + "\" index=0 caps=\"image/png,framerate=\\(fraction\\)15/1\" ! pngdec ! videoconvert ! videorate ! theoraenc ! oggmux ! filesink location=\"" + ofToDataPath( ofToString( sequenceStartTime ) + ".ogg" ) + "\"";
        taskRunner.addCommand( ofToDataPath( "script-gst-pipeline-rpi.sh" ) + " " + ofToString( sequenceStartTime ) + " " + ofToDataPath( "", true ) );
#else
        taskRunner.addCommand( ofToDataPath( "script-gst-pipeline-mac.sh" ) + " " + ofToString( sequenceStartTime ) + " " + ofToDataPath( "", true ) );
#endif
        
        // Write the last frame number
        taskRunner.addCommand( "echo " + ofToString( videoPlayer.getCurrentFrame() ) + " > " + ofToDataPath( "captures/.lastFrame" ));
        
        // ## Reset counters
        sequenceStartTime = ofGetUnixTime();
        frameCounter = 0;
        shouldCreateRecording = false;
    } else {
        shouldCreateRecording = false;
    }
    
    // Update Saving Status
    // --------------------
    bool isSaving = taskRunner.queueSize() > 0;
    
#ifdef TARGET_LINUX_ARM
    if ( saveImage.isThreadRunning() ) {
        lastCaptureTime = ofGetElapsedTimef();
    }
#endif
    
    // Capture Frame
    // -------------
    if ( !isSaving
         && shouldCaptureFrame
         && ofGetElapsedTimef() - lastCaptureTime > SELFIES_CAPTURE_THROTTLE_SEC ) {
        // ### Add Onion Skin
        onionskin.getCurrentFboPtr()->begin();
        ofClear( 0, 0, 0, 0 );
        ofPushMatrix();
        ofScale( float( onionskin.settings.width ) / float( SELFIES_CAPTURE_WIDTH ), float( onionskin.settings.height ) / float( SELFIES_CAPTURE_HEIGHT ), 1.0 );
        frameFbo.draw( 0, 0 );
        
        ofPopMatrix();
        onionskin.getCurrentFboPtr()->end();
        
        // ### Save to disk
#ifdef TARGET_LINUX_ARM
        //        ofPixels pix;
        //        frameFbo.readToPixels( pix );
        //        ofSaveImage( pix, "captures/" + ofToString( sequenceStartTime ) + "-" + ofToString( frameCounter++ ) + ".png" );
        frameFbo.readToPixels( saveImage.getPixelsRef() );
        saveImage.saveThreaded( "captures/" + ofToString( sequenceStartTime ) + "-" + ofToString( frameCounter++ ) + ".png" );
#else
        if ( !imageSaver.grabFbo( "captures/" + ofToString( sequenceStartTime ) + "-" + ofToString( frameCounter++ ) + ".png", frameFbo ) ) {
            ofLogError() << "Failed to grab screen. Maybe screen grab buffer is set to low";
        }
#endif
        
        videoPlayer.nextFrame();
        ofLogNotice() << "Current frame is: " << videoPlayer.getCurrentFrame();
        if ( videoPlayer.getCurrentFrame() == -1 ) {
            videoPlayer.setFrame( 0 );
        }
        
        // ###ÊCreate Onion Skin Overlay
        onionskin.renderAll();
        onionskin.next();
        
        lastCaptureTime = ofGetElapsedTimef();
        shouldCaptureFrame = false;
    }
    
    // Onion Skins Overlay
    // -------------------
    if ( shouldDrawOnionSkin ) {
        onionskin.layer.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    }
    
    // Debug Instructions/HUD
    // ----------------------
    if ( shouldDrawHUD ) {
        ofDrawBitmapStringHighlight( "[0-5] Onion Skin Blend", ofPoint( 200, 20 ) );
        ofDrawBitmapStringHighlight( "[o] Toggle Onion Skin", ofPoint( 200, 40 ) );
        ofDrawBitmapStringHighlight( "[space] Take Frame", ofPoint( 200, 60 ) );
        ofDrawBitmapStringHighlight( "[s] Save video & new session", ofPoint( 200, 80 ) );
        ofDrawBitmapStringHighlight( "[h] Show this display", ofPoint( 200, 100 ) );
    }
    
    // Saving Overlay
    // --------------
    if ( isSaving ) {
        ofPushStyle();
        ofSetColor( 255, 255, 0 );
        ofRect( 0, 0, ofGetWidth(), ofGetHeight() );
        ofPushMatrix();
        ofTranslate( ofGetWidth() / 2.0f, ofGetHeight() / 2.0f );
        saving.draw( -saving.getWidth() / 2.0f, -saving.getHeight() / 2.0f );
        ofPushMatrix();
        ofPopStyle();
    }
    
    // Camera flash with digits
    // ------------------------
    if ( ofGetElapsedTimef() - lastCaptureTime < SELFIES_CAPTURE_THROTTLE_SEC ) {
        ofPushMatrix();
        ofPushStyle();
        // Draw overlay
        ofSetColor( 255, 255, 0, ofMap( ofGetElapsedTimef() - lastCaptureTime, 0, SELFIES_CAPTURE_THROTTLE_SEC, 255, 0 ) );
        ofRect( 0, 0, ofGetWidth(), ofGetHeight() );
        ofPopStyle();
        // Draw digits
        ofPushStyle();
        ofSetColor( 255, 255, 255, ofMap( ofGetElapsedTimef() - lastCaptureTime, 0, SELFIES_CAPTURE_THROTTLE_SEC, 255, 0 ) );
        ofTranslate( ofGetWidth() / 2.0f, ofGetHeight() / 2.0f );
        drawDigitsCentered( digits, frameCounter );
        ofPopStyle();
        ofPopMatrix();
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
        shouldCreateRecording = true;
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
