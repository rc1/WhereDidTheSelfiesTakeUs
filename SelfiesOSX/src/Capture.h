#pragma once

// Capture
// =======
struct Capture {
    Capture():
    shouldCaptureFrame( false ),
    shouldDrawOnionSkin( true ),
    shouldCreateRecording( false ),
    shouldDrawHUDOnScreen( false ),
    isSaving( false ),
    frameCounter( 0 )
    {};
    
    // Flags
    // -----
    bool shouldCaptureFrame;
    bool shouldDrawOnionSkin;
    bool shouldCreateRecording;
    bool shouldDrawHUDOnScreen;
    bool isSaving;
    int frameCounter;
    
    // Assets
    // ------
    ofImage digitsImage;
    ofImage savingImage;
    
    // Tools
    // -----
    ofVideoGrabber videoGrabber;
    ofVideoPlayer videoPlayer;
    ThreadedImageSaver imageSaver;
    ThreadedExecQueue taskRunner;
    OnionSkinRingBufferSettings onionSkinSettings;
    OnionSkinRingBuffer onionskin;
    ofFbo frameFbo;
};