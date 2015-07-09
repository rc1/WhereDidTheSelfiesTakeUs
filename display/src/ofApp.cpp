#include "ofApp.h"
#include "Poco/File.h"
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
}

bool isNotMovieFile ( const ofFile &file ) {
    string ext = file.getExtension();
    ofLogNotice() << ext << ( ext != "mov" && ext != "mp4" && ext != "avi" && ext != "ogg" ? "is not a movie" : "is a movie" );
    return ext != "mov" && ext != "mp4" && ext != "avi" && ext != "ogg";
}

// Video Shims
// ===========

#ifdef TARGET_OSX
inline bool videoPlayerIsLoaded ( ofVideoPlayer &videoPlayer ) {
    return videoPlayer.isLoaded();
}
#else
inline bool videoPlayerIsLoaded ( ofxOMXPlayer &videoPlayer ) {
    return videoPlayer.isOpen();
}
#endif

#ifdef TARGET_OSX
inline bool videoPlayerIsDone ( ofVideoPlayer &videoPlayer ) {
    return videoPlayer.getIsMovieDone();
}
#else
inline bool videoPlayerIsDone ( ofxOMXPlayer &videoPlayer ) {
    return videoPlayer.getCurrentFrame() == videoPlayer.getTotalNumFrames() - 1;
}
#endif

#ifdef TARGET_OSX
inline void restartVideoPlayer ( ofVideoPlayer &videoPlayer ) {
    videoPlayer.setFrame( 0 );
    videoPlayer.play();
}
#else
inline void restartVideoPlayer ( ofxOMXPlayer &videoPlayer ) {
    videoPlayer.restartMovie();
}
#endif

#ifdef TARGET_OSX
inline void playVideoPlayer ( ofVideoPlayer &videoPlayer ) {
    videoPlayer.play();
}
#else
inline void playVideoPlayer ( ofxOMXPlayer &videoPlayer ) {
    videoPlayer.setIsPause( false );
}
#endif

#ifdef TARGET_OSX
inline void stopVideoPlayer ( ofVideoPlayer &videoPlayer ) {
    return videoPlayer.stop();
}
#else
inline void stopVideoPlayer ( ofxOMXPlayer &videoPlayer ) {
    videoPlayer.setIsPause( true );
}
#endif


// ofApp
// =====

void ofApp::setup () {
    ofSetLogLevel( OF_LOG_NOTICE );
#ifdef TARGET_OSX
    videoPlayerA.setPlayer( ofPtr<ofGstVideoPlayer>( new ofGstVideoPlayer ) );
    videoPlayerB.setPlayer( ofPtr<ofGstVideoPlayer>( new ofGstVideoPlayer ) );
#else
    ofxOMXPlayerSettings playerSettings;
    playerSettings.enableAudio = false;
    playerSettings.enableLooping = false;
#endif
    
    activeVideoPlayer = &videoPlayerA;
    inactiveVideoPlayer = &videoPlayerB;
    
    isLoadingNewVideo = false;
    currentVideoFilename = "";
    nextUpVideoPath = "";
    
    newVideoDisplayCount = 0;
    newVideoImage.loadImage( SELFIES_CAPTURE_NEW_VIDEO_IMAGE );
    
    ofSetBackgroundColor( 0 );
    
    ofSetFrameRate( 15 );
    
#ifdef TARGET_LINUX_ARM
    ofHideCursor();
#endif

    ofLogNotice() << "Setup Complete";
}

void ofApp::exit () {
    
}

void ofApp::update () {
    
#ifdef TARGET_OSX
    if ( videoPlayerIsLoaded( *activeVideoPlayer ) ) {
        activeVideoPlayer->update();
    }
#endif
    
    // List of all video files
    static vector<ofFile> videoFiles;
    // How often we check for new files
    static Expires reloadDirTime( 2.0f );
    
    // Check any changes in the video files folder
    if ( reloadDirTime.resetIfExpired() && !isLoadingNewVideo ) {
        
        ofDirectory d;
        d.setShowHidden( false );
        d.listDir( SELFIES_DISPLAY_VIDEO_DIR );
        vector<ofFile> incomingVideoFiles = d.getFiles();
        
        // Look for lock file
        // If a file named 'lock' exists the capture app is
        // still writing the file to disk, so just ignore
        bool hasLockFile = false;
        for ( vector<ofFile>::iterator it = incomingVideoFiles.begin(); it != incomingVideoFiles.end(); ++it ) {
            // ofLogNotice() << "Incoming file: " << it->getFileName();
            if ( it->getFileName() == "lock" ) {
                hasLockFile = true;
                break;
            }
        }
        // Check the age of the lock file
        if ( hasLockFile ) {
            Poco::Timestamp::TimeDiff timeSinceCreated = ofFile( ofToString( SELFIES_DISPLAY_VIDEO_DIR ) + "lock" ).getPocoFile().getLastModified().elapsed();
            if ( timeSinceCreated > 10000 * 10 ) {
                hasLockFile = false;
                ofLogNotice() << "Ignoring log file, to old: " << ofToString( timeSinceCreated );
            }
        }
        
        // Filter any video what are not movie files
        incomingVideoFiles.erase( remove_if( incomingVideoFiles.begin(), incomingVideoFiles.end(), isNotMovieFile ), incomingVideoFiles.end() );

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
        
        if ( videoPlayerIsDone( *activeVideoPlayer ) && videoFiles.size() > 0 && !isLoadingNewVideo ) {
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
                restartVideoPlayer( *activeVideoPlayer );
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
        ofLogNotice() << "Actioning the loading of: " << nextUpVideoPath;
        if ( !isLoadingNewVideo ) {
            stopVideoPlayer( *activeVideoPlayer );
            inactiveVideoPlayer->loadMovie( nextUpVideoPath );
            isLoadingNewVideo = true;
        } else if ( videoPlayerIsLoaded( *inactiveVideoPlayer ) ) {
#ifdef TARGET_OSX
            activeVideoPlayer->close();
#endif
            playVideoPlayer( *inactiveVideoPlayer );
            
            // Swap pointers
#ifdef TARGET_OSX
            ofVideoPlayer *swap = activeVideoPlayer;
#else
            ofxOMXPlayer *swap = activeVideoPlayer;
#endif
            activeVideoPlayer = inactiveVideoPlayer;
            inactiveVideoPlayer = swap;
            isLoadingNewVideo = false;
            nextUpVideoPath = "";
        }
    }
}

void ofApp::draw () {
    ofClear( ofColor::black );
    
    ofRectangle windowRectangle( 0, 0, ofGetWidth(), ofGetHeight() );
    ofRectangle videoRectangle( 0, 0, activeVideoPlayer->getWidth(), activeVideoPlayer->getHeight() );
    windowRectangle.scaleTo( windowRectangle, OF_SCALEMODE_FILL );

    activeVideoPlayer->draw( windowRectangle );
    
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
