#include "ofApp.h"
#include "ofGstVideoPlayer.h"

//--------------------------------------------------------------
void ofApp::setup(){
	player.setPlayer(ofPtr<ofGstVideoPlayer>(new ofGstVideoPlayer));
	player.loadMovie("fingers.mov");
	player.play();
}

//--------------------------------------------------------------
void ofApp::update(){
    if ()
	player.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	player.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}