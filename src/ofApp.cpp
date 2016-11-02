#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Setup the sound stream
    soundStream.setup(this, 0, MY_CHANNELS, MY_SRATE, MY_BUFFERSIZE, MY_NBUFFERS);
    
    // Resize and initialize left and right buffers...
    left.resize( MY_BUFFERSIZE, 0 );
    right.resize( MY_BUFFERSIZE, 0 );
    
    ofSetFullscreen(true);
    ofEnableDepthTest();
    ofEnableSmoothing();
    ofBackground(20);
    
    baseWidth = 80.0;
    maxWidth = 120.0;
    scale = 50.0;
    int res = 2;
    
    leftSphere.setRadius(baseWidth);
    leftSphere.setResolution(res);
    
    rightSphere.setRadius(baseWidth);
    rightSphere.setResolution(res);
}

//--------------------------------------------------------------
void ofApp::update(){
    auto leftMaxIndex = max_element(std::begin(left), std::end(left), abs_compare);
    float maxLeftVal = (left[std::distance(left.begin(), leftMaxIndex)]*scale)+baseWidth;
    float newLeftRadius = (maxLeftVal > maxWidth)? maxWidth : maxLeftVal;
    leftSphere.setRadius(newLeftRadius);
    
    auto rightMaxIndex = max_element(std::begin(right), std::end(right), abs_compare);
    float maxRightVal = (right[std::distance(right.begin(), rightMaxIndex)]*scale)+baseWidth;
    float newRightRadius = (maxRightVal > maxWidth)? maxWidth : maxRightVal;
    rightSphere.setRadius(newRightRadius);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    
    // Move to center of window
    ofTranslate(ofGetWindowWidth()*0.5, ofGetWindowHeight()*0.5);
    ofPushMatrix();
    
        // Set style for left sphere
        ofPushStyle();
            ofSetColor(100, 100, 100);
            leftSphere.setPosition(-300, 0, 0);
            leftSphere.drawWireframe();
        ofPopStyle();
    
        // Set style for right sphere
        ofPushStyle();
            ofSetColor(0, 100, 210);
            rightSphere.setPosition(300, 0, 0);
            rightSphere.drawWireframe();
        ofPopStyle();

    ofPopMatrix();
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    // Write incoming audio to buffer. Note: samples are interleaved.
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2];
        right[i]	= input[i*2+1];
    }
}
