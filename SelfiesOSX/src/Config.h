#pragma once
#include "ofxXmlSettings.h"

// Config
// ======

// Display
// -------
#define SELFIES_DISPLAY_WIDTH Config::getGlobal().displayWidth
#define SELFIES_DISPLAY_HEIGHT Config::getGlobal().displayHeight
#define SELFIES_DISPLAY_MAX_VIDEOS Config::getGlobal().maxVideos
#define SELFIES_DISPLAY_NEW_IMAGE "sprites/NewVideo.png"

// Capture
// -------
#define SELFIES_CAPTURE_TEXTURE_WIDTH Config::getGlobal().captureTextureWidth
#define SELFIES_CAPTURE_TEXTURE_HEIGHT Config::getGlobal().captureTextureHeight
#define SELFIES_CAPTURE_DISPLAY_WIDTH Config::getGlobal().captureWidth
#define SELFIES_CAPTURE_DISPLAY_HEIGHT Config::getGlobal().captureHeight
#define SELFIES_CAPTURE_THROTTLE_SEC 1.2f
#define SELFIES_CAPTURE_DIGIT_IMAGE "sprites/CounterDigits.png"
#define SELFIES_CAPTURE_SAVING_IMAGE "sprites/Saving.png"
#define SELFIES_CAPTURE_OVERLAY_VIDEO "animation/Overlay.mov"
#define SELFIES_CAPTURE_MIN_FRAME_COUNT Config::getGlobal().minCapturesForRecording

struct Config {
    // Getter
    static Config & getGlobal () {
        static Config global;
        return global;
    }
    
#define CONFIG_VALUE(var, default) \
    (var) = xml.getValue( "xml:" #var, (default)); \
    ofLogNotice() << "Config: " << #var << ": " << ofToString((var));
    
#define CONFIG_ATTRIBUTE(var, name, attr, default) \
    (var) = xml.getAttribute( "xml:" #name, (attr), (default)); \
    ofLogNotice() << "Config: " << #var << ":" << #attr << ofToString((var));
    
    Config() {
        ofxXmlSettings xml( "settings.xml" );
        
        string str;
        xml.copyXmlToString( str );
        ofLogNotice() << str;
        
        ofLogNotice() << "====== Config =======";
        
        CONFIG_VALUE( serialDevice, "" );
        CONFIG_VALUE( minCapturesForRecording, 0 );
        CONFIG_VALUE( maxVideos, 0 );
        
        CONFIG_ATTRIBUTE( windowPositionX, windowPosition, "x", 0 );
        CONFIG_ATTRIBUTE( windowPositionY, windowPosition, "y", 0 );
        
        CONFIG_ATTRIBUTE( captureWidth, captureDisplaySize, "width", 0 );
        CONFIG_ATTRIBUTE( captureHeight, captureDisplaySize, "height", 0 );
        
        CONFIG_ATTRIBUTE( captureTextureWidth, captureTextureSize, "width", 0 );
        CONFIG_ATTRIBUTE( captureTextureHeight, captureTextureSize, "height", 0 );
        
        CONFIG_ATTRIBUTE( displayWidth, displaySize, "width", 0 );
        CONFIG_ATTRIBUTE( displayHeight, displaySize, "height", 0 );
        
        ofLogNotice() << "=====================";
    }
    
    int windowPositionX;
    int windowPositionY;
    int captureWidth;
    int captureHeight;
    int captureTextureWidth;
    int captureTextureHeight;
    int displayWidth;
    int displayHeight;
    int minCapturesForRecording;
    int maxVideos;
    string serialDevice;
};