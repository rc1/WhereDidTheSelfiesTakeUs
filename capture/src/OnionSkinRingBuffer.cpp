#include "OnionSkinRingBuffer.h"

#pragma mark - Settings

OnionSkinRingBufferSettings::OnionSkinRingBufferSettings () {
    numberFrames = 1;
    width = 800;
    height = 480;
    blendMode = OF_BLENDMODE_ALPHA;
    show = 4;
}

#pragma mark - Lifecycle

OnionSkinRingBuffer::OnionSkinRingBuffer () {

}

OnionSkinRingBuffer::~OnionSkinRingBuffer () {
    
}

#pragma mark - Config

void OnionSkinRingBuffer::init ( OnionSkinRingBufferSettings settings ) {

    this->settings = settings;

    layer.allocate( settings.width, settings.height, GL_RGBA );
    ofLogNotice() << "Allocated Onionskin Buffer with " << ofToString( settings.width ) << "x" << ofToString( settings.height ) << "px";

    // Create the Fbos with the same
    for ( int i=0; i < settings.numberFrames; ++i ) {
        OnionSkinFboPtr framePtr( new ofFbo() );
        framePtr->allocate( settings.width, settings.height, GL_RGBA );
        fbos.push_back( framePtr );
    }

    currentFboIdx = 0;
}

#pragma mark - Frames

void OnionSkinRingBuffer::next () {
    currentFboIdx++;
    if ( currentFboIdx >= settings.numberFrames ) {
        currentFboIdx = 0;
    }
}

OnionSkinFboPtr OnionSkinRingBuffer::getCurrentFboPtr () {
    return fbos[ currentFboIdx ];
}

void OnionSkinRingBuffer::renderAll () {

    static bool hasRun = false;
    if ( !hasRun ) {
        for ( int i=0; i < settings.numberFrames; ++i ) {
            fbos[ i ]->begin();
            ofClear( 0, 0, 0, 255 );
            fbos[ i ]->end();
        }
        hasRun = true;
    }

    layer.begin();

    ofClear( 0, 0, 0, 255 );

    for ( int i=0; i < settings.show; ++i ) {
        ofPushStyle();
        int alpha = (int) ofMap( i, 0, settings.show, 200, 20 );
        ofSetColor( 255, 255, 255, alpha );

        int frameIdx = currentFboIdx - i;
        if ( frameIdx < 0 ) {
            frameIdx += settings.numberFrames;
        }
        
        ofEnableBlendMode( settings.blendMode );
        fbos[ frameIdx ]->draw( 0, 0, settings.width, settings.height );
        ofEnableBlendMode( OF_BLENDMODE_ALPHA );
        ofPopStyle();
    }

    layer.end();
}
