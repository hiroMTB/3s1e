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
    
    void craete( Branch * parent, ofVec3f dirn );
        void set_param();
        void calc_posision();
        void calc_shape();
        bool check_territory();
    
    void update();
    void revive();
    void animate();
    
    
    void draw();
    void create_child();
    
    static int top_depth;
    static int total_bnum;
    static ofVec3f root;
    static int active_total;
    
    bool bDrawShapeAsPoint;
    bool bMainBranch;
    
    int b_type;
    int f_type;
    int depth;
    int bid;
    float sp_len;
    float fw_len;
    float sp_angle;
    
    Branch * parent;
    ofVec3f st;
    ofVec3f anchor;
    ofVec3f end;
    ofVec3f dirn;
    ofVec3f dirn_p;
    ofFloatColor color;
    
    ofVboMesh points;
    ofVboMesh lines;
    
    vector<Branch> children;
    

};