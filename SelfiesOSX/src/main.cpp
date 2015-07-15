#include "ofMain.h"
#include "ofApp.h"
#include "Config.h"

int main () {
    // Set Data Path
    ofSetDataPathRoot( "../Resources/data/" );
    
    ofSetupOpenGL( 100, 100, OF_FULLSCREEN );
    
    int width = Config::getGlobal().displayWidth + Config::getGlobal().captureWidth;
    int height = max( Config::getGlobal().captureHeight, Config::getGlobal().displayHeight );
    
    ofSetWindowPosition( Config::getGlobal().windowPositionX, Config::getGlobal().windowPositionY );
    ofSetWindowShape( width, height );
    ofRunApp( new ofApp() );
}
