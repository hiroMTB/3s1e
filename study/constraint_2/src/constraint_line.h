//
//  constraint_line.h
//  constraint_1
//
//  Created by hiroshi matoba on 12/3/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

class constraint_line{
    
public:
    
    constraint_line( ofxBulletWorldRigid * world, ofVec3f start, ofVec3f end, int num, float length, float random_factor );
    
    void update();
    void draw();
    
    int num;
    float random_factor;
    
    ofVec3f start, end;
   	ofxBulletWorldRigid * world;

    vector<ofVec3f> particle_pos;
    vector<ofVec3f> pivot_pos;
    

    ofVec3f norm;
    vector<ofxBulletBaseShape*> shapes;
    vector<ofxBulletJoint*>		joints;
    
    ofVboMesh points;
    ofVboMesh lines;
};