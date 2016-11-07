#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    playAudio = false;
    useMic = true;
    
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
    ofBackground(255,255,240);
    
    baseWidth = 100.0;
    maxWidth = 120.0;
    minWidth = 80.0;
    scale = 30.0;
    leftRadius = baseWidth;
    rightRadius = baseWidth;
    leftSpherePos = ofVec3f(ofGetWindowWidth()*0.5-300, ofGetWindowHeight()*0.5, 0);
    rightSpherePos = ofVec3f(ofGetWindowWidth()*0.5+300, ofGetWindowHeight()*0.5, 0);

    
    leftGain = 0.05;
    rightGain = 0.05;
    leftGainTarget = .5;
    rightGainTarget = .5;
    gainInterval = 0.05;
    gainSmoothers[0].setSmooth(.9992);
    gainSmoothers[1].setSmooth(.9992);
    
    leftRotation = 0;
    rightRotation = 0;
    rotationSpeed = 0.15;
    
    creatingParticles = false;
    numParticles = 0;
    maxParticles = 300;
    particles.resize(maxParticles);
    particles.clear();
    mouseX = 0;
    mouseY = 0;
    
    int res = 2;
    
    leftSphere.setRadius(leftRadius);
    leftSphere.setResolution(res);
    
    rightSphere.setRadius(rightRadius);
    rightSphere.setResolution(res);
    
    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update(){
    auto leftMaxIndex = max_element(std::begin(left), std::end(left), abs_compare);
    float maxLeftVal = (left[std::distance(left.begin(), leftMaxIndex)]*scale)+baseWidth;
    float leftRadius = (maxLeftVal > maxWidth)? maxWidth : maxLeftVal;
    if (leftRadius < minWidth) {
        leftRadius = minWidth;
    }
    leftSphere.setRadius(leftRadius*leftGain);
    
    auto rightMaxIndex = max_element(std::begin(right), std::end(right), abs_compare);
    float maxRightVal = (right[std::distance(right.begin(), rightMaxIndex)]*scale)+baseWidth;
    rightRadius = (maxRightVal > maxWidth)? maxWidth : maxRightVal;
    if (rightRadius < minWidth) {
        rightRadius = minWidth;
    }
    rightSphere.setRadius(rightRadius*rightGain);
    
    leftRotation += rotationSpeed;
    rightRotation -= rotationSpeed;
    
    if (creatingParticles) {
        createParticle();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    
    ofSetColor(0, 30, 160);
    for (int i = 0; i < particles.size(); i++) {
        ofVec3f particlePos = particles[i].getPosition();
        ofVec3f particleVel = particles[i].getVelocity();
        
        float d1 = sqrt(pow(particlePos[0]-leftSpherePos[0], 2) + pow(particlePos[1]-leftSpherePos[1], 2) + pow(particlePos[2]-leftSpherePos[2], 2));
        float d2 = sqrt(pow(particlePos[0]-rightSpherePos[0], 2) + pow(particlePos[1]-rightSpherePos[1], 2) + pow(particlePos[2]-rightSpherePos[2], 2));
        float leftGrav = (leftGain-.05)/pow(d1, 2);
        float rightGrav = (rightGain-.05)/pow(d2, 2);
      
        int pull = (leftGrav > rightGrav) ? 0 : 1;
        float xVector = 0;
        float yVector = 0;
        float zVector = 0;
        
        if (pull == 0) {
            xVector = (particlePos[0] > leftSpherePos[0]) ? (-1)*GRAVITY : GRAVITY;
            yVector = (particlePos[1] > leftSpherePos[1]) ? (-1)*GRAVITY : GRAVITY;
            zVector = (particlePos[2] > leftSpherePos[2]) ? (-1)*GRAVITY : GRAVITY;
            particles[i].setVelocity(particleVel[0]+xVector, particleVel[1]+yVector, particleVel[2]+zVector);
        } else {
            xVector = (particlePos[0] > rightSpherePos[0]) ? (-1)*GRAVITY : GRAVITY;
            yVector = (particlePos[1] > rightSpherePos[1]) ? (-1)*GRAVITY : GRAVITY;
            zVector = (particlePos[2] > rightSpherePos[2]) ? (-1)*GRAVITY : GRAVITY;
            particles[i].setVelocity(particleVel[0]+xVector, particleVel[1]+yVector, particleVel[2]+zVector);
        }
        
        particles[i].draw();
    }
    
    
    // Move to center of window
    ofTranslate(ofGetWindowWidth()*0.5, ofGetWindowHeight()*0.5);
    ofPushMatrix();
        ofSetColor(140, 0, 25, 256*leftGain);

        // Set style for left sphere
        leftSphere.setPosition(-300, 0, 0);
        leftSphere.rotate(-rotationSpeed, 0.0, 1.0, 0.0);
        leftSphere.drawWireframe();
    
    // Set style for right sphere
        ofSetColor(140, 0, 25, 256*rightGain);

        rightSphere.setPosition(300, 0, 0);
        rightSphere.rotate(rotationSpeed, 0.0, 1.0, 0.0);
        rightSphere.drawWireframe();

    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 32) {
        useMic = !useMic;
        playAudio = !playAudio;
    } else if (key == OF_KEY_UP) {
        rightGainTarget += gainInterval;
        if (rightGainTarget > 1) rightGainTarget = 1;
    } else if (key == OF_KEY_DOWN) {
        rightGainTarget -= gainInterval;
        if (rightGainTarget < 0.05) rightGainTarget = 0.05;
    } else if (key == OF_KEY_RIGHT) {
        leftGainTarget += gainInterval;
        if (leftGainTarget > 1) leftGainTarget = 1;
    } else if (key == OF_KEY_LEFT) {
        leftGainTarget -= gainInterval;
        if (leftGainTarget < 0.05) leftGainTarget = 0.05;
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
    mouseX = x;
    mouseY = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mouseX = x;
    mouseY = y;
    creatingParticles = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    creatingParticles = false;
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

// Audio functions
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
            leftGain = gainSmoothers[0].tick(leftGainTarget);
            left[i] = leftChannel(i,0);
            output[2*i] = leftChannel(i,0)*leftGain;
            
            rightGain = gainSmoothers[1].tick(rightGainTarget);
            right[i] = rightChannel(i,0);
            output[2*i+1] = rightChannel(i,0)*rightGain;
        }
    }
}

// Particle functions
//--------------------------------------------------------------

void ofApp::createParticle() {
    if (numParticles < maxParticles) {
        numParticles++;
        Particle *newParticle = new Particle;
        newParticle->setPosition(mouseX, mouseY, 0);
        newParticle->setVelocity(ofRandom(-10, 10), ofRandom(0, 10), ofRandom(-5, 5));
        particles.insert(particles.begin(), *newParticle);
    }
}

