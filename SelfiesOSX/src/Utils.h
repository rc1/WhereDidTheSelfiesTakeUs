#pragma once
#include "ofMain.h"
#include "Config.h"
#include "Display.h"

namespace Utils {
    
    // Paths
    // =====
    
    string getFramesPath () {
        return "/Users/" + ofToString( getenv( "USER" ) ) + "/Movies/Selfies/Frames";
    }
    
    string getAnimationsPath () {
        return "/Users/" + ofToString( getenv( "USER" ) ) + "/Movies/Selfies/Animations";
    }
    
    string getOverlayPath () {
        return "/Users/" + ofToString( getenv( "USER" ) ) + "/Movies/Selfies/Overlay/Overlay.mov";
    }
    
    // Digits
    // ======
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
    
    
    // Expires
    // =======
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
    
    inline void playVideo( Display &display, string filename ) {
        display.nextUpVideoPath = ofToDataPath( getAnimationsPath() + "/" + filename );
        display.currentVideoFilename = filename;
    }
    
    bool isNotMovieFile ( const ofFile &file ) {
        string ext = file.getExtension();
        return ext != "mov" && ext != "mp4" && ext != "avi" && ext != "ogg";
    }
    
};
