#include "ofApp.h"
#include "ofGstVideoPlayer.h"

// Utils
// =====

class Expires {
public:
    Expires():ttl(1.0f) { time=ofGetElapsedTimef(); };
    Expires(float ttl):ttl(ttl) { time=ofGetElapsedTimef(); };
    bool hasExpired() { return ofGetElapsedTimef() - time > ttl; };
    void reset() { time=ofGetElapsedTimef(); }
    // Returns true if expired
    bool resetIfExpired() { if ( hasExpired() ) { reset(); return true; } return false; }
private:
    float ttl;
    float time;
};

inline float scaleToBounds ( float width, float height, float boundsWidth, float boundsHeight ) {
    float widthScale = boundsWidth / width;
    float heightScale = boundsHeight / height;
    return min( widthScale, heightScale );
}

inline string getNextFilename( const vector<ofFile> &files, const string currentFileName ) {
    if ( files.size() == 0 ) { return ""; }
    int idx = 0;
    for ( ; idx < files.size(); ++idx ) {
        if ( files[ idx ].getFileName() == currentFileName ) {
            break;
        }
    }
    return files[ ++idx >= files.size() ? 0 : idx ].getFileName();
}

inline void playVideo( ofApp &app, string filename ) {
    app.nextUpVideoPath = ofToDataPath( SELFIES_DISPLAY_VIDEO_DIR + filename );
    app.currentVideoFilename = filename;
    ofLogVerbose() << "Next Up Video: " << filename;
}

// ofApp
// =====

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );
    videoPlayer.setPlayer( ofPtr<ofGstVideoPlayer>( new ofGstVideoPlayer ) );

    currentVideoFilename = "";
    nextUpVideoPath = "";
    
    ofSetBackgroundColor( 0 );
    
    ofLogNotice() << "videoPlayer:" << videoPlayer.getIsMovieDone();
}

void ofApp::exit () {
    
}

void ofApp::update () {
    
    if ( videoPlayer.isLoaded() ) {
        videoPlayer.update();
    }
    
    // List of all video files
    static vector<ofFile> videoFiles;
    // How often we check for new files
    static Expires reloadDirTime( 2.0f );
    
    // Check for a new file
    if ( reloadDirTime.resetIfExpired() ) {
        
        ofDirectory d;
        d.listDir( SELFIES_DISPLAY_VIDEO_DIR );
        
        vector<ofFile> incomingVideoFiles = d.getFiles();
        
        // Look for lock file
        bool hasLockFile = false;
        for ( vector<ofFile>::iterator it = incomingVideoFiles.begin(); it != incomingVideoFiles.end(); ++it ) {
            if ( it->getFileName() == "lock" ) {
                hasLockFile = true;
                break;
            }
        }
        if ( !hasLockFile ) {
            vector<ofFile> differentFiles;
            set_symmetric_difference( videoFiles.begin(), videoFiles.end(), incomingVideoFiles.begin(), incomingVideoFiles.end(), back_inserter( differentFiles ) );
            
            if ( differentFiles.size() > 0 ) {
                videoFiles = incomingVideoFiles;
                playVideo( *this, differentFiles.back().getFileName() );
            }
        }
    }
    // If the current file is finished
    else {
        if ( videoPlayer.getIsMovieDone() && videoFiles.size() > 0 ) {
            playVideo( *this, getNextFilename( videoFiles, currentVideoFilename ) );
        }
    }
    // if the next button had been released
    // if the previous button has been released
    
    // Play any videos
    if ( nextUpVideoPath != "" ) {
        if ( videoPlayer.isLoaded() ) {
            videoPlayer.closeMovie();
        } else {
            ofLogNotice() << "Will load: " << nextUpVideoPath;
            videoPlayer.loadMovie( nextUpVideoPath );
            nextUpVideoPath = "";
            videoPlayer.play();
        }
    }
}

void ofApp::draw () {
    ofClear( ofColor::black );
    ofTranslate( ofGetWidth() / 2.0f, ofGetHeight() / 2.0f );
    float scaleToFit = scaleToBounds( videoPlayer.getWidth(), videoPlayer.getHeight(), ofGetWidth(), ofGetHeight() );
    ofScale( scaleToFit, scaleToFit, 1.0f );
    ofTranslate( -videoPlayer.getWidth() / 2.0f, -videoPlayer.getHeight() / 2.0f );
    videoPlayer.draw( 0, 0 );
}

void ofApp::keyPressed ( int key ) {
  
}

void ofApp::keyReleased ( int key ) {

}

void ofApp::windowResized ( int w, int h ) {

}

void ofApp::gotMessage( ofMessage msg ) {

}
