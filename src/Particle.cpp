//
//  Particle.cpp
//  
//
//  Created by Graham Davis on 11/6/16.
//
//

#include "Particle.h"

Particle::Particle() {
    xPos = 0;
    yPos = 0;
    zPos = 0;
    
    xVel = 0;
    yVel = 0;
    zVel = 0;
    
    radius = ofRandom(1.0, 7.0);
    std::cout << radius << std::endl;
    
    dot.setRadius(radius);
}

Particle::~Particle(){}

void Particle::draw() {
    setPosition(xPos+xVel, yPos+yVel, zPos+zVel);
    dot.setPosition(xPos, yPos, zPos);
    dot.setRadius(radius);
    dot.draw();
}

void Particle::setPosition(int x, int y, int z) {
    xPos = x;
    yPos = y;
    zPos = z;
}

void Particle::setVelocity(float x, float y, float z) {
    xVel = x;
    yVel = y;
    zVel = z;
}

ofVec3f Particle::getPosition() {
    ofVec3f pos = ofVec3f(xPos, yPos, zPos);
    return pos;
}

ofVec3f Particle::getVelocity() {
    ofVec3f vel = ofVec3f(xVel, yVel, zVel);
    return vel;
}

float Particle::getRadius() {
    return radius;
}
