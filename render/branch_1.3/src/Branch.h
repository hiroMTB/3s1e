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
    static float dist_limit;

    bool bDrawShapeAsPoint;
    bool bMainBranch;
  
    static int max_b_type;
    static int max_f_type;
    
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
    
    static vector<Branch*> nonp;

    ofVec2f speed;
    ofVec2f initpos;
    
    void add_line( const ofVec2f & st, const ofVec2f & anc, const ofVec2f & end, const ofFloatColor &c, int btype, int ftype);
  
    bool bStation;
    int num_son;
    float node_anim_time;
    float node_anim_speed;
};