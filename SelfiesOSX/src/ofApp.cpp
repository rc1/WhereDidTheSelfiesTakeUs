#include "ofApp.h"
#include "Utils.h"
#include "FrameNumber.h"
#include "ThreadedImageSaver.h"
#include "ThreadedExecQueue.h"
#include "OnionSkinRingBuffer.h"
#include "Capture.h"
#include "Display.h"

static Capture capture;
static Display display;

void ofApp::setup (){
    ofLogNotice() << "User: " << getenv( "USER" );

    // Inital Settings
    // ===============
    ofSetDataPathRoot( "../Resources/data/" );
    ofSetBackgroundColor( 0x2A2B2F );
    ofSetLogLevel( OF_LOG_NOTICE );
    
    // Capture Setup
    // =============
    
    // Camera
    // ------
    capture.videoGrabber.setDeviceID( 0 );
    capture.videoGrabber.initGrabber( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT );
    
    // Onion Skin
    // ----------
    capture.onionSkinSettings.numberFrames = 2;
    capture.onionSkinSettings.show = 2;
    capture.onionSkinSettings.width = SELFIES_CAPTURE_WIDTH;
    capture.onionSkinSettings.height = SELFIES_CAPTURE_HEIGHT;
    capture.onionskin.init( capture.onionSkinSettings );
    
    // Video Player
    // ------------
    capture.videoPlayer.setPixelFormat( OF_PIXELS_RGBA );
    capture.videoPlayer.load( SELFIES_CAPTURE_OVERLAY_VIDEO );
    capture.videoPlayer.setLoopState( OF_LOOP_NORMAL );
    capture.videoPlayer.setFrame( FrameNumber::loadFrameNumber() );
    ofLogNotice() << "Should have set frame number to: " << FrameNumber::loadFrameNumber();

    // Fbo
    // ---
    capture.frameFbo.allocate( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT, GL_RGB );
    
    // Task Runner
    // -----------
    capture.taskRunner.startThread( true );
    capture.imageSaver.setup( SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT, 20 );
    
    // Images
    // ------
    capture.digitsImage.load( SELFIES_CAPTURE_DIGIT_IMAGE );
    capture.savingImage.load( SELFIES_CAPTURE_SAVING_IMAGE );
    
    // Display Setup
    // =============
    display.activeVideoPlayer = &display.videoPlayerA;
    display.inactiveVideoPlayer = &display.videoPlayerB;
    display.newVideoImage.load( SELFIES_DISPLAY_NEW_IMAGE );

}

void ofApp::exit () {
    capture.taskRunner.stopThread();
}

void ofApp::update () {
    
    // Capture
    // =======
    capture.videoGrabber.update();
    capture.videoPlayer.update();
    
    // Display
    // =======
    display.videoPlayerA.update();
    display.videoPlayerB.update();
    
    // Update New Files
    // ----------------
    // List of all video files
    static vector<ofFile> videoFiles;
    // How often we check for new files
    static Utils::Expires reloadDirTime( 2.0f );
    
    // Check any changes in the video files folder
    if ( reloadDirTime.resetIfExpired() && !display.isLoadingNewVideo ) {
        
        ofDirectory d;
        d.setShowHidden( false );
        d.listDir( Utils::getAnimationsPath() );
        d.sort();
        vector<ofFile> incomingVideoFiles = d.getFiles();
        // reverse( incomingVideoFiles.begin(), incomingVideoFiles.end() );
        
        // Look for lock file
        // If a file named 'lock' exists the capture app is
        // still writing the file to disk, so just ignore
        bool hasLockFile = false;
        for ( vector<ofFile>::iterator it = incomingVideoFiles.begin(); it != incomingVideoFiles.end(); ++it ) {
            if ( it->getFileName() == "lock" ) {
                hasLockFile = true;
                ofLogNotice() << "Have a lock file in place";
                break;
            }
        }
        
        // Filter any video what are not movie files
        incomingVideoFiles.erase( remove_if( incomingVideoFiles.begin(), incomingVideoFiles.end(), Utils::isNotMovieFile ), incomingVideoFiles.end() );
        
//        int c = 0;
//        for( vector<ofFile>::iterator it = incomingVideoFiles.begin(); it != incomingVideoFiles.end(); ++it ) {
//            ofLogNotice() << ofToString( ++c ) << " " << it->getFileName();
//        }
        
        // Only keep the 50 newest
        if ( incomingVideoFiles.size() > SELFIES_DISPLAY_MAX_VIDEOS ) {
            incomingVideoFiles.resize( SELFIES_DISPLAY_MAX_VIDEOS );
        }
        
        if ( !hasLockFile ) {
            // Find if any files have changed
            vector<ofFile> differentFiles;
            set_symmetric_difference( videoFiles.begin(), videoFiles.end(), incomingVideoFiles.begin(), incomingVideoFiles.end(), back_inserter( differentFiles ) );
            // If they have, take the newest video (by dated filename)
            if ( differentFiles.size() > 0 ) {
                videoFiles = incomingVideoFiles;
                ofLogNotice() << "====================================";
                ofLogNotice() << "New Videos Found. Current Play list:";
                Utils::playVideo( display, differentFiles.back().getFileName() );
                display.newVideoDisplayCount = 3;
                for ( vector<ofFile>::iterator it = videoFiles.begin(); it != videoFiles.end(); ++it ) {
                    ofLogNotice() << "- " << it->getFileName();
                }
                ofLogNotice() << "------------------------------------";
                ofLogNotice() << "Loading:";
                ofLogNotice() << differentFiles.back().getFileName();
                ofLogNotice() << "====================================";
                
            }
        }
    }
    // If the current file is finished, queue the next filename
    else {
        
        if ( display.activeVideoPlayer->getIsMovieDone() && videoFiles.size() > 0 && !display.isLoadingNewVideo ) {
            if ( display.newVideoDisplayCount == 0 ) {
                Utils::playVideo( display, Utils::getNextFilenameInFiles( videoFiles, display.currentVideoFilename ) );
            }
            // Go back x number of video to see the new video in context
            else if ( display.newVideoDisplayCount == 1 ) {
                string previousFilename = display.currentVideoFilename;
                for ( int i = 0; i < 3; ++i ) {
                    previousFilename = Utils::getPreviousFilenameInFiles( videoFiles, previousFilename );
                }
                Utils::playVideo( display, previousFilename );
                --display.newVideoDisplayCount;
            }
            else {
                display.activeVideoPlayer->setFrame( 0 );
                display.activeVideoPlayer->play();
                --display.newVideoDisplayCount;
            }
        }
    }
    // if the next button had been released
    // if the previous button has been released
    
    // Play any videos which have need queued up
    // by first closing the video player, then
    // once it has closed (later) load the next
    static int actioningLoadCount = 0;
    if ( display.nextUpVideoPath != "" ) {
        ++actioningLoadCount;
        if ( actioningLoadCount > 100 ) {
            ofLogNotice() << "Taking to long to load";
            display.isLoadingNewVideo = false;
            Utils::playVideo( display, Utils::getNextFilenameInFiles( videoFiles, display.currentVideoFilename ) );
            actioningLoadCount = 0;
            return;
        }
        ofLogNotice() << "Actioning the loading of: " << display.nextUpVideoPath;
        if ( !display.isLoadingNewVideo ) {
            display.activeVideoPlayer->stop();
            display.inactiveVideoPlayer->load( display.nextUpVideoPath );
            display.isLoadingNewVideo = true;
        } else if ( display.inactiveVideoPlayer->isLoaded() ) {
            display.activeVideoPlayer->close();
            display.inactiveVideoPlayer->play();
            
            // Swap pointers
            ofVideoPlayer *swap = display.activeVideoPlayer;
            display.activeVideoPlayer = display.inactiveVideoPlayer;
            display.inactiveVideoPlayer = swap;
            display.isLoadingNewVideo = false;
            display.nextUpVideoPath = "";
        }
    }

}

void ofApp::draw () {
    
    // Display
    // =======
    
    ofRectangle windowRectangle( 0, 0, SELFIES_DISPLAY_WIDTH, SELFIES_DISPLAY_HEIGHT );
    ofRectangle videoRectangle( 0, 0, display.activeVideoPlayer->getWidth(), display.activeVideoPlayer->getHeight() );
    windowRectangle.scaleTo( windowRectangle, OF_SCALEMODE_FILL );
    
    display.activeVideoPlayer->draw( windowRectangle.getX(), windowRectangle.getY(), windowRectangle.getWidth(), windowRectangle.getHeight() );
    
    if ( display.newVideoDisplayCount > 0 ) {
        display.newVideoImage.draw( 20, 20 );
    }
    
    ofDrawBitmapStringHighlight( display.activeVideoPlayer->getMoviePath(), 20, SELFIES_DISPLAY_HEIGHT + 20 );
    
    // Capture
    // =======
    static int sequenceStartTime = ofGetUnixTime();
    static float lastCaptureTime = -10000.0f;
    ofRectangle screenRectangle( 0, 0, SELFIES_CAPTURE_WIDTH, SELFIES_CAPTURE_HEIGHT );
    
    // Compose Live Image
    // ------------------
    capture.frameFbo.begin();
    {
        // Calculate scale to fills
        ofRectangle grabberRectangle( 0, 0, capture.videoGrabber.getWidth(), capture.videoGrabber.getHeight() );
        ofRectangle videoRectangle( 0, 0, capture.videoPlayer.getWidth(), capture.videoPlayer.getHeight() );
        grabberRectangle.scaleTo( screenRectangle, OF_SCALEMODE_FILL );
        videoRectangle.scaleTo( screenRectangle, OF_SCALEMODE_FILL );
        
        // Flip camera, like a mirror
        ofPushMatrix();
        {
            ofScale( -1, 1, 1 );
            ofTranslate( -SELFIES_CAPTURE_WIDTH, 0 );
            // Draw camera
            capture.videoGrabber.draw( grabberRectangle );
        }
        ofPopMatrix();
        
        // Draw animation overlay
        capture.videoPlayer.draw( videoRectangle );
    }
    capture.frameFbo.end();
    
    // Live Preview
    // ------------
    capture.frameFbo.draw( SELFIES_DISPLAY_WIDTH, 0, SELFIES_CAPTURE_DISPLAY_WIDTH, SELFIES_CAPTURE_DISPLAY_HEIGHT );
    
    // Onion Skins Overlay
    // -------------------
    if ( capture.shouldDrawOnionSkin ) {
        capture.onionskin.layer.draw( SELFIES_DISPLAY_WIDTH, 0, SELFIES_CAPTURE_DISPLAY_WIDTH, SELFIES_CAPTURE_DISPLAY_HEIGHT );
    }
    
    if ( capture.shouldCreateRecording && capture.frameCounter < SELFIES_CAPTURE_MIN_FRAME_COUNT ) {
        capture.shouldCreateRecording = false;
    }
    
    // Create Video Recording
    // ----------------------
    if ( !capture.isSaving && capture.shouldCreateRecording && capture.frameCounter > SELFIES_CAPTURE_MIN_FRAME_COUNT ) {
        FrameNumber::saveFrameNumber( capture.videoPlayer.getCurrentFrame() + 1 );
        ofLogNotice() << "Saved last frame number to: " << capture.videoPlayer.getCurrentFrame() + 1;
        // Run the script (that uses likely uses GStreamer to make the mp4 file)
        capture.taskRunner.addCommand( ofToDataPath( "scripts/script-gst-pipeline-mac.sh" ) + " " + ofToString( sequenceStartTime ) + " " + Utils::getAnimationsPath() + " " + Utils::getFramesPath() );
        // Save the frame number
        capture.shouldCreateRecording = false;
        capture.frameCounter = 0;
        sequenceStartTime = ofGetUnixTime();
    }
    
    // Update Status
    // -------------
    capture.isSaving = capture.taskRunner.queueSize() > 0;
    

    // Camera flash with digits
    // ------------------------
    if ( ofGetElapsedTimef() - lastCaptureTime < SELFIES_CAPTURE_THROTTLE_SEC ) {
        ofPushMatrix();
        ofPushStyle();
        // Draw overlay
        ofSetColor( 255, 255, 0, ofMap( ofGetElapsedTimef() - lastCaptureTime, 0, SELFIES_CAPTURE_THROTTLE_SEC, 255, 0 ) );
        ofDrawRectangle( SELFIES_DISPLAY_WIDTH, 0, SELFIES_CAPTURE_DISPLAY_WIDTH, SELFIES_CAPTURE_DISPLAY_HEIGHT );
        ofPopStyle();
        // Draw digits
        ofPushStyle();
        ofSetColor( 255, 255, 255, ofMap( ofGetElapsedTimef() - lastCaptureTime, 0, SELFIES_CAPTURE_THROTTLE_SEC, 255, 0 ) );
        ofTranslate( SELFIES_DISPLAY_WIDTH, 0 );
        ofTranslate( SELFIES_CAPTURE_DISPLAY_WIDTH / 2.0f, SELFIES_CAPTURE_DISPLAY_HEIGHT / 2.0f);
        Utils::drawDigitsCentered( capture.digitsImage, capture.frameCounter );
        ofPopStyle();
        ofPopMatrix();
        
        // Stop new frames from being captured
        capture.shouldCaptureFrame = false;
    }
    
    
    // Capture a Frame
    // -------------
    if ( !capture.isSaving && capture.shouldCaptureFrame && ofGetElapsedTimef() - lastCaptureTime > SELFIES_CAPTURE_THROTTLE_SEC ) {
        
        ofLogNotice() << "Capturing Frame";
        
        // Render the frame to the onion skin
        capture.onionskin.getCurrentFboPtr()->begin();
        {
            ofClear( 0, 0, 0, 0 );
            ofPushMatrix();
            ofScale( float( capture.onionskin.settings.width ) / float( SELFIES_CAPTURE_WIDTH ), float( capture.onionskin.settings.height ) / float( SELFIES_CAPTURE_HEIGHT ), 1.0 );
            capture.frameFbo.draw( 0, 0 );
            ofPopMatrix();
        }
        capture.onionskin.getCurrentFboPtr()->end();
        
        // Save the image file
        string capturePath = Utils::getFramesPath() + "/" + ofToString( sequenceStartTime ) + "-" + ofToString( capture.frameCounter++ ) + ".png";
        ofLogNotice() << "Capturing to: " << capturePath;
        if ( !capture.imageSaver.grabFbo( capturePath, capture.frameFbo ) ) {
            ofLogError() << "Failed to grab screen. Maybe screen grab buffer is set to low";
        }
        
        // Advance to the next overlay video frame
        int frame = capture.videoPlayer.getCurrentFrame();
        capture.videoPlayer.setFrame( frame + 2 );
        ofLogNotice() << "Current frame is: " << capture.videoPlayer.getCurrentFrame();
        
        // Update the onion skins
        capture.onionskin.renderAll();
        capture.onionskin.next();
        
        // Update flags
        lastCaptureTime = ofGetElapsedTimef();
        capture.shouldCaptureFrame = false;
    }
    
    
    // Saving Overlay
    // --------------
    if ( capture.isSaving ) {
        ofPushStyle();
        ofSetColor( 255, 255, 0 );
        ofDrawRectangle( SELFIES_DISPLAY_WIDTH, 0, SELFIES_CAPTURE_DISPLAY_WIDTH, SELFIES_CAPTURE_DISPLAY_HEIGHT );
        ofPushMatrix();
        ofTranslate( SELFIES_DISPLAY_WIDTH, 0 );
        ofTranslate( SELFIES_CAPTURE_DISPLAY_WIDTH / 2.0f, SELFIES_CAPTURE_DISPLAY_HEIGHT / 2.0f );
        capture.savingImage.draw( -capture.savingImage.getWidth() / 2.0f, -capture.savingImage.getHeight() / 2.0f );
        ofPopMatrix();
        ofPopStyle();
    }
    
    // Debug Instructions/HUD
    // ----------------------
    ofPushMatrix();
    {
        ofTranslate( ofGetWidth()-200, 0 );
        if ( !capture.shouldDrawHUDOnScreen ) {
            ofTranslate( 0, SELFIES_CAPTURE_DISPLAY_HEIGHT );
        }
        ofDrawBitmapStringHighlight( "[0-5] Onion Skin Blend", ofPoint( 0, 20 ) );
        ofDrawBitmapStringHighlight( "[o] Toggle Onion Skin", ofPoint( 0, 40 ) );
        ofDrawBitmapStringHighlight( "[space] Take Frame", ofPoint( 0, 60 ) );
        ofDrawBitmapStringHighlight( "[s] Save video & new session", ofPoint( 0, 80 ) );
        ofDrawBitmapStringHighlight( "[h] Show this display", ofPoint( 0, 100 ) );
    }
    ofPopMatrix();
    


}

void ofApp::keyPressed ( int key ) {
    if ( key == ' ' ) {
        capture.shouldCaptureFrame = true;
    }
    else if ( key == '0' ) {
        capture.onionskin.settings.blendMode = OF_BLENDMODE_DISABLED;
    }
    else if ( key == '1' ) {
        capture.onionskin.settings.blendMode = OF_BLENDMODE_ALPHA;
    }
    else if ( key == '2' ) {
        capture.onionskin.settings.blendMode = OF_BLENDMODE_ADD;
    }
    else if ( key == '3' ) {
        capture.onionskin.settings.blendMode = OF_BLENDMODE_SUBTRACT;
    }
    else if ( key == '4' ) {
        capture.onionskin.settings.blendMode = OF_BLENDMODE_MULTIPLY;
    }
    else if ( key == '5' ) {
        capture.onionskin.settings.blendMode = OF_BLENDMODE_SCREEN;
    }
    else if ( key == 'o' ) {
        capture.shouldDrawOnionSkin = !capture.shouldDrawOnionSkin;
    }
    else if ( key == 's' ) {
        capture.shouldCreateRecording = true;
    }
    else if ( key == 'h' ) {
        capture.shouldDrawHUDOnScreen =! capture.shouldDrawHUDOnScreen;
    }
}
