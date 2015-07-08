#include "ofApp.h"
#ifdef TARGET_OSX
#include "ofGstVideoPlayer.h"
#endif

// Utils
// =====

class Expires {
public:
    Expires():ttl(1.0f) { time=0; };
    Expires(float ttl):ttl(ttl) { time=0; };
    bool hasExpired() { return ofGetElapsedTimef() - time > ttl; };
    void reset() { time=ofGetElapsedTimef(); }
    bool resetIfExpired() { if ( hasExpired() ) { reset(); return true; } return false; }
private:
    float ttl;
    float time;
};

inline float scaleToBoundsRatio ( float width, float height, float boundsWidth, float boundsHeight ) {
    float widthScale = boundsWidth / width;
    float heightScale = boundsHeight / height;
    return min( widthScale, heightScale );
}

inline string getNextFilenameInFiles( const vector<ofFile> &files, const string currentFilename ) {
    if ( files.size() == 0 ) { return ""; }
    int idx = 0;
    for ( ; idx < files.size(); ++idx ) {
        if ( files[ idx ].getFileName() == currentFilename ) {
            break;
        }
    }
    return files[ ++idx >= files.size() ? 0 : idx ].getFileName();
}

inline string getPreviousFilenameInFiles( const vector<ofFile> &files, const string currentFilename ) {
    if ( files.size() == 0 ) { return ""; }
    int idx = 0;
    for ( ; idx < files.size(); ++idx ) {
        if ( files[ idx ].getFileName() == currentFilename ) {
            break;
        }
    }
    return files[ --idx < 0 ? files.size()-1 : idx ].getFileName();
}

inline void playVideo( ofApp &app, string filename ) {
    app.nextUpVideoPath = ofToDataPath( SELFIES_DISPLAY_VIDEO_DIR + filename );
    app.currentVideoFilename = filename;
    ofLogNotice() << "Next Up Video: " << filename;
}

// ofApp
// =====

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );
#ifdef TARGET_OSX
    videoPlayerA.setPlayer( ofPtr<ofGstVideoPlayer>( new ofGstVideoPlayer ) );
    videoPlayerB.setPlayer( ofPtr<ofGstVideoPlayer>( new ofGstVideoPlayer ) );
#endif
    activeVideoPlayer = &videoPlayerA;
    inactiveVideoPlayer = &videoPlayerB;
    
    isLoadingNewVideo = false;
    currentVideoFilename = "";
    nextUpVideoPath = "";
    
    newVideoDisplayCount = 0;
    newVideoImage.loadImage( SELFIES_CAPTURE_NEW_VIDEO_IMAGE );
    
    ofSetBackgroundColor( 0 );
}

void ofApp::exit () {
    
}

void ofApp::update () {
    
    if ( activeVideoPlayer->isLoaded() ) {
        activeVideoPlayer->update();
    }
    
    // List of all video files
    static vector<ofFile> videoFiles;
    // How often we check for new files
    static Expires reloadDirTime( 2.0f );
    
    // Check any changes in the video files folder
    if ( reloadDirTime.resetIfExpired() && !isLoadingNewVideo ) {
        
        ofDirectory d;
        d.listDir( SELFIES_DISPLAY_VIDEO_DIR );
        vector<ofFile> incomingVideoFiles = d.getFiles();
        
        // Look for lock file
        // If a file named 'lock' exists the capture app is
        // still writing the file to disk, so just ignore
        bool hasLockFile = false;
        for ( vector<ofFile>::iterator it = incomingVideoFiles.begin(); it != incomingVideoFiles.end(); ++it ) {
            if ( it->getFileName() == "lock" ) {
                hasLockFile = true;
                break;
            }
        }
        if ( !hasLockFile ) {
            // Find if any files have changed
            vector<ofFile> differentFiles;
            set_symmetric_difference( videoFiles.begin(), videoFiles.end(), incomingVideoFiles.begin(), incomingVideoFiles.end(), back_inserter( differentFiles ) );
            // If they have, take the newest video (by dated filename)
            if ( differentFiles.size() > 0 ) {
                videoFiles = incomingVideoFiles;
                playVideo( *this, differentFiles.back().getFileName() );
                newVideoDisplayCount = 3;
            }
        }
    }
    // If the current file is finished, queue the next filename
    else {
        ofLogNotice() << "Video is " << ( activeVideoPlayer->getIsMovieDone()?"done":"not done" ) << " on frame " << ofToString( activeVideoPlayer->getCurrentFrame() ) << " / " << activeVideoPlayer->ofBaseVideoPlayer::getTotalNumFrames();
        if ( activeVideoPlayer->getIsMovieDone() && videoFiles.size() > 0 && !isLoadingNewVideo ) {
            if ( newVideoDisplayCount == 0 ) {
                playVideo( *this, getNextFilenameInFiles( videoFiles, currentVideoFilename ) );
            }
            // Go back x number of video to see the new video in context
            else if ( newVideoDisplayCount == 1 ) {
                string previousFilename = currentVideoFilename;
                for ( int i = 0; i < 3; ++i ) {
                    previousFilename = getPreviousFilenameInFiles( videoFiles, previousFilename );
                }
                playVideo( *this, previousFilename );
                --newVideoDisplayCount;
            }
            else {
                ofLogNotice() << newVideoDisplayCount;
                activeVideoPlayer->setFrame( 0 );
                activeVideoPlayer->play();
                --newVideoDisplayCount;
            }
        }
    }
    // if the next button had been released
    // if the previous button has been released
    
    // Play any videos which have need queued up
    // by first closing the video player, then
    // once it has closed (later) load the next
    if ( nextUpVideoPath != "" ) {
        if ( !isLoadingNewVideo ) {
            activeVideoPlayer->stop();
            inactiveVideoPlayer->loadMovie( nextUpVideoPath );
            isLoadingNewVideo = true;
        } else if ( inactiveVideoPlayer->isLoaded() ) {
            activeVideoPlayer->close();
            inactiveVideoPlayer->play();
            // Swap pointer
            ofVideoPlayer *swap = activeVideoPlayer;
            activeVideoPlayer = inactiveVideoPlayer;
            inactiveVideoPlayer = swap;
            isLoadingNewVideo = false;
            nextUpVideoPath = "";
        }
    }
}

void ofApp::draw () {
    ofClear( ofColor::black );
    
    ofPushMatrix();
    // Draw the video scaled to fit the window
    ofTranslate( ofGetWidth() / 2.0f, ofGetHeight() / 2.0f );
    float scaleToFitRatio = scaleToBoundsRatio( activeVideoPlayer->getWidth(), activeVideoPlayer->getHeight(), ofGetWidth(), ofGetHeight() );
    ofScale( scaleToFitRatio, scaleToFitRatio, 1.0f );
    ofTranslate( -activeVideoPlayer->getWidth() / 2.0f, -activeVideoPlayer->getHeight() / 2.0f );
    activeVideoPlayer->draw( 0, 0 );
    ofPopMatrix();
    
    if ( newVideoDisplayCount > 0 ) {
        newVideoImage.draw( 20, 20 );
    }
}

void ofApp::keyPressed ( int key ) {
  
}

void ofApp::keyReleased ( int key ) {
    if ( key == 'f' ) {
        ofToggleFullscreen();
    }
}

void ofApp::windowResized ( int w, int h ) {

}

void ofApp::gotMessage( ofMessage msg ) {

}
