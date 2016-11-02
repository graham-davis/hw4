#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    playAudio = false;
    useMic = false;
    
    // Setup the sound stream
    soundStream.setup(this, MY_CHANNELS, MY_CHANNELS, MY_SRATE, MY_BUFFERSIZE, MY_NBUFFERS);
    
    
    // Setup audio file playback
    audio.setRate(MY_SRATE);
    audio.openFile(ofToDataPath("twig.wav", true));
    stk::Stk::setSampleRate(MY_SRATE);
    
    // Resize and initialize left and right buffers...
    left.resize( MY_BUFFERSIZE, 0 );
    right.resize( MY_BUFFERSIZE, 0 );
    
    //ofSetFullscreen(true);
    ofEnableDepthTest();
    ofEnableSmoothing();
    ofBackground(20);
    
    baseWidth = 80.0;
    maxWidth = 100.0;
    minWidth = 70.0;
    scale = 30.0;
    
    leftRotation = 0;
    rightRotation = 0;
    rotationSpeed = 0.15;
    
    int res = 2;
    
    leftSphere.setRadius(baseWidth);
    leftSphere.setResolution(res);
    
    rightSphere.setRadius(baseWidth);
    rightSphere.setResolution(res);
    
    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update(){
    auto leftMaxIndex = max_element(std::begin(left), std::end(left), abs_compare);
    float maxLeftVal = (left[std::distance(left.begin(), leftMaxIndex)]*scale)+baseWidth;
    float newLeftRadius = (maxLeftVal > maxWidth)? maxWidth : maxLeftVal;
    if (newLeftRadius < minWidth) {
        newLeftRadius = minWidth;
    }
    leftSphere.setRadius(newLeftRadius);
    
    auto rightMaxIndex = max_element(std::begin(right), std::end(right), abs_compare);
    float maxRightVal = (right[std::distance(right.begin(), rightMaxIndex)]*scale)+baseWidth;
    float newRightRadius = (maxRightVal > maxWidth)? maxWidth : maxRightVal;
    if (newRightRadius < minWidth) {
        newRightRadius = minWidth;
    }
    rightSphere.setRadius(newRightRadius);
    
    leftRotation += rotationSpeed;
    rightRotation -= rotationSpeed;
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
            leftSphere.rotate(-rotationSpeed, 0.0, 1.0, 0.0);
            leftSphere.drawWireframe();
        ofPopStyle();
    
        // Set style for right sphere
        ofPushStyle();
            ofSetColor(0, 100, 210);
            rightSphere.setPosition(300, 0, 0);
            rightSphere.rotate(rotationSpeed, 0.0, 1.0, 0.0);
            rightSphere.drawWireframe();
        ofPopStyle();

    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 32) {
        useMic = false;
        playAudio = !playAudio;
    } else if(key == OF_KEY_RETURN) {
        playAudio = false;
        useMic = !useMic;
    }
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

void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
    if (useMic) {
        for (int i = 0; i < bufferSize ; i++) {
            left[i] = input[2*i];
            right[i] = input[2*i+1];
        }
    }
}

//--------------------------------------------------------------

void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    // Write to output buffer
    if (playAudio) {
        stk::StkFrames frames(bufferSize,2);
        audio.tick(frames);

        stk::StkFrames leftChannel(bufferSize,1);
        // copy the left Channel of 'frames' into `leftChannel`
        frames.getChannel(0, leftChannel, 0);
        
        stk::StkFrames rightChannel(bufferSize, 1);
        frames.getChannel(1, rightChannel, 0);

        for (int i = 0; i < bufferSize ; i++) {
            left[i] = leftChannel(i,0);
            output[2*i] = leftChannel(i,0);
            
            right[i] = rightChannel(i,0);
            output[2*i+1] = rightChannel(i,0);
        }
    }
}
