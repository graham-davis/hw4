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
    spheresPos.resize(2);
    spheresPos[0] = ofVec3f(ofGetWindowWidth()*0.5-300, ofGetWindowHeight()*0.5, 0);
    spheresPos[1] = ofVec3f(ofGetWindowWidth()*0.5+300, ofGetWindowHeight()*0.5, 0);

    
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
    readyToCreate = false;
    numParticles = 0;
    maxParticles = 100;
    particles.resize(maxParticles);
    particles.clear();
    mouseX = 0;
    mouseY = 0;
    explodingLeft = false;
    explodingRight = false;
    gravity = 0.2;
    sunParticles[0] = 0;
    sunParticles[0] = 1;
    
    int res = 2;
    
    leftSphere.setRadius(leftRadius);
    leftSphere.setResolution(res);
    
    rightSphere.setRadius(rightRadius);
    rightSphere.setResolution(res);
    
    fftLeft = ofxFft::create(MY_BUFFERSIZE, OF_FFT_WINDOW_HAMMING);
    
    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update(){
    // Update FFT
    fftLeft->setSignal(&left[0]);
    
    // Reset exploding
    explodingLeft = false;
    explodingRight = false;
    
    // Compute left channel max and sphere params
    auto leftMaxIndex = max_element(std::begin(left), std::end(left), abs_compare);
    float maxLeftVal = (left[std::distance(left.begin(), leftMaxIndex)]*scale)+baseWidth;
    float leftRadius = (maxLeftVal > maxWidth)? maxWidth : maxLeftVal;
    if (maxLeftVal > 125) {
        explodingLeft = true;
    }
    if (leftRadius < minWidth) {
        leftRadius = minWidth;
    }
    leftSphere.setRadius(leftRadius*leftGain);
    
    // Compute right channel max and sphere params
    auto rightMaxIndex = max_element(std::begin(right), std::end(right), abs_compare);
    float maxRightVal = (right[std::distance(right.begin(), rightMaxIndex)]*scale)+baseWidth;
    rightRadius = (maxRightVal > maxWidth)? maxWidth : maxRightVal;
    if (rightRadius < minWidth) {
        rightRadius = minWidth;
    }
    if (maxRightVal > 125) {
        explodingRight = true;
    }
    rightSphere.setRadius(rightRadius*rightGain);
    
    
    // Rotate Spheres
    leftRotation += rotationSpeed;
    rightRotation -= rotationSpeed;
    
    // Create particles
    if (readyToCreate && creatingParticles) {
        createParticle();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(0, 30, 160, 150);

    stringstream ss;
    ss << "[Left/Right]: Left Gain (" << leftGain << ")\n";
    ss << "[Up/Down]: Right Gain (" << rightGain << ")\n";
    ss << "[Shift+Click]: Create Particles (" << numParticles << ")\n";
    ss << "[+/-]: Gravitational Force (" << gravity << ")\n";
    ofDrawBitmapString(ss.str().c_str(), 20, 20);
    
    ofPushMatrix();
    for (int i = 0; i < particles.size(); i++) {
        float opacity = fftLeft->getAmplitudeAtBin(i)*5000;
        if (opacity < 10) {
            opacity = 10;
        }
        ofSetColor(0, 30, 160, opacity);
        if (!playAudio) {
            particles[i].setVelocity(0, 0, 0);
            particles[i].draw();
        } else if (!particles[i].isExploding()) {
            if (explodingLeft) {
                particles[i].explode(0);
            } else if(explodingRight) {
                particles[i].explode(1);
            } else {
                ofVec3f particlePos = particles[i].getPosition();
                ofVec3f particleVel = particles[i].getVelocity();
                
                float d1 = abs(particlePos[0]-spheresPos[0][0]) + abs(particlePos[1]-spheresPos[0][1]) + abs(particlePos[2]-spheresPos[0][2]);
                float d2 = abs(particlePos[0]-spheresPos[1][0]) + abs(particlePos[1]-spheresPos[1][1]) + abs(particlePos[2]-spheresPos[1][2]);
              
                int pull = ((leftGain-.05)/d1 > (rightGain-.05)/d2) ? 0 : 1;
                float xVector = 0;
                float yVector = 0;
                float zVector = 0;
                
                if (particles[i].getSun() != pull) {
                    sunParticles[pull]++;
                    sunParticles[abs(pull-1)]--;
                    
                    std::cout << sunParticles[0] << " : " << sunParticles[1] << std::endl;
                    particles[i].setSun(pull);
                }
                
                xVector = (particlePos[0] > spheresPos[pull][0]) ? -1 : 1;
                yVector = (particlePos[1] > spheresPos[pull][1]) ? -1 : 1;
                zVector = (particlePos[2] > spheresPos[pull][2]) ? -1 : 1;
                particles[i].setVelocity(particleVel[0]+xVector*gravity, particleVel[1]+yVector*gravity, particleVel[2]+zVector*gravity);
            }
            particles[i].draw();
        } else if(particles[i].isExploding()) {
            ofVec3f particlePos = particles[i].getPosition();
            if (abs(particlePos[0]) > 1000) {
                numParticles--;
                particles.erase(particles.begin() + i);
                sunParticles[particles[i].getSun()]--;
            } else {
                particles[i].draw();
            }
        }
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
    } else if (key == OF_KEY_SHIFT) {
        readyToCreate = true;
    } else if (key == OF_KEY_BACKSPACE) {
        for(int i = 0; i < numParticles; i++) {
            particles[i].explode(particles[i].getSun());
        }
    } else if (key == 45) {
        if (gravity > 0.1) {
            gravity -= 0.05;
        }
    } else if (key == 61) {
        if (gravity < 0.8) {
            gravity += 0.05;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == OF_KEY_SHIFT) {
        readyToCreate = false;
    }
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

// Audio functions
//--------------------------------------------------------------


void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    // Write to output buffer
    if (playAudio) {
        stk::StkFrames frames(bufferSize,2);
        stk::StkFrames noiseFrames(bufferSize,1);
        audio.tick(frames);
        noise.tick(noiseFrames);

        stk::StkFrames leftChannel(bufferSize,1);
        // copy the left Channel of 'frames' into `leftChannel`
        frames.getChannel(0, leftChannel, 0);
        
        stk::StkFrames rightChannel(bufferSize, 1);
        frames.getChannel(1, rightChannel, 0);
        
        float nToSL = (float)sunParticles[0]/(maxParticles*5);
        float nToSR = (float)sunParticles[1]/(maxParticles*5);
        
        for (int i = 0; i < bufferSize ; i++) {
            leftGain = gainSmoothers[0].tick(leftGainTarget)*(1.0-(nToSL));
            left[i] = leftChannel(i,0);
            output[2*i] = leftChannel(i,0)*leftGain + noiseFrames(i, 0)*nToSL;
            
            rightGain = gainSmoothers[1].tick(rightGainTarget)*(1.0-(nToSR));
            right[i] = rightChannel(i,0);
            output[2*i+1] = rightChannel(i,0)*rightGain + noiseFrames(i, 0)*nToSR;
        }
    }
}

// Particle functions
//--------------------------------------------------------------

void ofApp::createParticle() {
    if (numParticles < maxParticles) {
        numParticles++;
        sunParticles[0]++;
        Particle *newParticle = new Particle;
        newParticle->setPosition(mouseX, mouseY, 0);
        newParticle->setVelocity(ofRandom(-10, 10), ofRandom(0, 10), ofRandom(-5, 5));
        particles.insert(particles.begin(), *newParticle);
    }
}

