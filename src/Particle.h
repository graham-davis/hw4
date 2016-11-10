//
//  Particle.h
//  
//
//  Created by Graham Davis on 11/6/16.
//
//

#ifndef ____Particle__
#define ____Particle__

#include <string>
#include <iostream>
#include "of3dPrimitives.h"
#include "smooth.h"
#include "ofGraphics.h"

class Particle {
public:
    Particle();
    ~Particle();
    
    void draw(float opacity);
    void setVelocity(float x, float y, float z);
    void setPosition(int x, int y, int z);
    ofVec3f getPosition();
    ofVec3f getVelocity();
    float getRadius();
    bool isExploding();
    void explode(int explodingSun);
    void setSun(int newSun);
    int getSun();

private:
    ofIcoSpherePrimitive dot;
    
    float radius;
    
    ofVec3f pos;
    ofVec3f vel;
    
    int xPos;
    int yPos;
    int zPos;
    
    float xVel;
    float yVel;
    float zVel;
    
    bool exploding;
    
    int sun;
};

#endif /* defined(____Particle__) */
