#pragma once
#include "ofMain.h"
#include "Config.h"

namespace Utils {
    
    // Paths
    // =====
    
    string getFramesPath () {
        return "/Users/" + ofToString( getenv( "USER" ) ) + "/Movies/Selfies/Frames";
    }
    
    string getAnimationsPath () {
        return "/Users/" + ofToString( getenv( "USER" ) ) + "/Movies/Selfies/Animations";
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

    
};
