#include "ofApp.h"

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

// ofApp
// =====

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );
    currentVideoFilename = "";
}

void ofApp::exit () {
    
}

void ofApp::update () {
    
    videoPlayer.update();
    
    // List of all video files
    static vector<ofFile> videoFiles;
    
    // Check for new files
    static Expires reloadDirTime( 5.0f );
    if ( reloadDirTime.resetIfExpired() ) {
    
        ofDirectory d;
        d.listDir( SELFIES_DISPLAY_VIDEO_DIR );
        
        vector<ofFile> incomingVideoFiles = d.getFiles();
        vector<ofFile> differentFiles;
        set_symmetric_difference( videoFiles.begin(), videoFiles.end(), incomingVideoFiles.begin(), incomingVideoFiles.end(), back_inserter( differentFiles ) );
        
        if ( differentFiles.size() > 0 ) {
            videoFiles = differentFiles;
            ofLogNotice() << "Will open" << differentFiles.back().getFileName() ;
            currentVideoFilename = differentFiles.back().getFileName();
            videoPlayer.loadMovie( ofToDataPath( SELFIES_DISPLAY_VIDEO_DIR + currentVideoFilename ) );
            videoPlayer.play();
        }
    } else {
        if ( videoPlayer.getIsMovieDone() ) {
            // find the next file, and load it
            
        }
    }
}

void ofApp::draw () {
    
}

void ofApp::keyPressed ( int key ) {
  
}

void ofApp::keyReleased ( int key ) {

}

void ofApp::windowResized ( int w, int h ) {

}

void ofApp::gotMessage( ofMessage msg ) {

}
