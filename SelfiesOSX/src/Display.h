#pragma once
#include "ofMain.h"

// Display
// =======
struct Display {
    Display():
    currentVideoFilename( "" ),
    nextUpVideoPath( "" ),
    isLoadingNewVideo( false ),
    newVideoDisplayCount( 0 )
    {}
    
    // Flags
    // -----
    string currentVideoFilename;
    string nextUpVideoPath;
    bool isLoadingNewVideo;
    int newVideoDisplayCount;
    
    // Video Players
    // -------------
    ofVideoPlayer videoPlayerA;
    ofVideoPlayer videoPlayerB;
    ofVideoPlayer *activeVideoPlayer;
    ofVideoPlayer *inactiveVideoPlayer;
    
    // Image
    // -----
    ofImage newVideoImage;
};