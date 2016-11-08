//
//  Particle.cpp
//  
//
//  Created by Graham Davis on 11/6/16.
//
//

#include "Particle.h"

Particle::Particle() {
    for (int i=0; i<3; i++) {
        pos[i] = 0;
        vel[i] = 0;
    }
    
    radius = ofRandom(1.0, 7.0);
    
    dot.setRadius(radius);
    
    exploding = false;
    
    sun = 0;
}

Particle::~Particle(){}

void Particle::draw() {
    setPosition(pos[0]+vel[0], pos[1]+vel[1], pos[2]+vel[2]);
    dot.setPosition(pos[0], pos[1], pos[2]);
    dot.setRadius(radius);
    dot.draw();
}

void Particle::setPosition(int x, int y, int z) {
    pos = ofVec3f(x, y, z);
}

void Particle::setVelocity(float x, float y, float z) {
    vel = ofVec3f(x, y, z);
}

ofVec3f Particle::getPosition() {
    return pos;
}

ofVec3f Particle::getVelocity() {
    return vel;
}

float Particle::getRadius() {
    return radius;
}

bool Particle::isExploding() {
    return exploding;
}

void Particle::explode(int explodingSun) {
    if (explodingSun == sun) {
        if (ofRandom(0, 1) > .65) {
            exploding = true;
            setVelocity(ofRandom(-50, 50), ofRandom(-50, 50), ofRandom(-50, 50));
        }
    }
}

void Particle::setSun(int newSun) {
    sun = newSun;
}

int Particle::getSun() {
    return sun;
}
