#pragma once

#include "ofMain.h"

struct OnionSkinRingBufferSettings {
    OnionSkinRingBufferSettings();
    uint numberFrames;
    uint width;
    uint height;
    ofBlendMode blendMode;
    uint show;
};

typedef std::shared_ptr<ofFbo> OnionSkinFboPtr;

class OnionSkinRingBuffer {
 public:

    // Lifecycle
    OnionSkinRingBuffer ();
    virtual ~OnionSkinRingBuffer ();

    void init ( OnionSkinRingBufferSettings settings );

    // Cycling
    void next ();
    OnionSkinFboPtr getCurrentFboPtr ();

    void renderAll ();

    ofFbo layer;

    // Settings
    OnionSkinRingBufferSettings settings;

    
    // Frames
    vector< OnionSkinFboPtr > fbos;
    uint currentFboIdx;
};
