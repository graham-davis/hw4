#pragma once

#include "ofMain.h"
#include "ofxStk.h"
#include "smooth.h"
#include "Particle.h"
#include "ofxFft.h"

//-----------------------------------------------------------------------------
// Preprocessor definitions
//-----------------------------------------------------------------------------
#define MY_SRATE         44100            // sample rate
#define MY_CHANNELS      2                // number of channels
#define MY_BUFFERHISTORY 50               // number of buffers to save
#define MY_BUFFERSIZE    512              // number of frames in a buffer
#define MY_NBUFFERS      2                // number of buffers latency
#define MY_PIE           3.14159265358979 // for convenience

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void audioOut(float * input, int bufferSize, int nChannels);
    
        ofIcoSpherePrimitive leftSphere;
        ofIcoSpherePrimitive rightSphere;
    
    private:
        void createParticle();
        // Our sound stream object
        ofSoundStream soundStream;
    
        float baseWidth;
        float maxWidth;
        float minWidth;
        float scale;
    
        // Audio params for left and right channel
        float leftGain;
        float leftGainTarget;
        float rightGain;
        float rightGainTarget;
        float gainInterval;
        Smooth gainSmoothers[2];
    
        // Params for left and right sphere
        float leftRadius;
        float rightRadius;
        float rightRotation;
        float leftRotation;
        float rotationSpeed;
        vector<ofVec3f> spheresPos;
    
        // Vectors for our left- and right-channel waveforms
        vector<float> left;
        vector<float> right;
    
        // STK audio units
    
        stk::FileWvIn audio;
        stk::Noise noise;
    
        // Audio processing variables
        bool playAudio;
        bool useMic;
    
        // Particle variables
        bool creatingParticles;
        int numParticles;
        int maxParticles;
        vector<Particle> particles;
        int mouseX;
        int mouseY;
        bool explodingLeft;
        bool explodingRight;
        float gravity;
        ofVec2f sunParticles;
    
        float ww;
        float wh;
    
        static bool abs_compare(int a, int b)
        {
            return (std::abs(a) < std::abs(b));
        }
    
        bool showHelp;
    
        // Fft variables
        ofxFft *fftLeft;
        ofxFft *fftRight;
};
