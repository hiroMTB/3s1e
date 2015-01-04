//
//  Branch.h
//
//  Created by hiroshi matoba on 1/3/15.
//
//

#pragma once

#include "ofMain.h"

class Branch{
    
public:
    
    void craete( Branch * parent, ofVec3f dirn, float sp_len, float sp_angle, float fw_len, int b_type, int f_type );
    void calc_posision();
    void calc_shape();
    void update();
    void draw();
    void create_child();
    
    static int top_depth;
    static ofVec3f root;

    bool bDrawShapeAsPoint;
    int b_type;
    int f_type;
    int depth;
    float sp_len;
    float fw_len;
    float sp_angle;
    
    Branch * parent;
    ofVec3f st;
    ofVec3f anchor;
    ofVec3f end;
    ofVec3f dirn;
    ofVec3f dirn_p;
    
    ofVboMesh points;
    ofVboMesh lines;
    
    vector<Branch> children;
    
};