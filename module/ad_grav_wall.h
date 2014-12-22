//
//  ad_grav_wall.h
//  grav_wall
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

struct gvWall{
    
private:
    gvWall();
    
public:
    gvWall( ofVec2f _p1, ofVec2f _p2)
    :p1(_p1), p2(_p2){
        inited = true;
    };
    
    bool bGrav;
    bool inited;
    ofVec2f p1;
    ofVec2f p2;
    ofVec2f norm;
};


class ad_grav_wall{
    
public:
    
    ad_grav_wall();
    void setup( ofImage * img=NULL, ofCamera * cam = NULL );

    void create_line( ofVec2f p1, ofVec2f p2, float density=0.1 );
    void update();
    void update_lines();
    void update_points();
    void update_attrs();
    void draw();
    void onCollision( ofxBulletCollisionData& cdata );
    
    static int particle_col_group;
    static int wall_col_group;
    static int particle_col_setting;
    static int wall_col_setting;

    bool bInitPhysics;
    float frame;

    float impulse;
    
    ofxBulletWorldRigid world;
    vector<btSphereShape*> sphereShapes;

    vector<ofxBulletBaseShape*>	shapes;
    vector<ofxBulletBox*>	walls;
    
    vector<ofVec3f> pivots;
    
    vector<ofVec3f> collision;
    
    ofVboMesh points;
    ofVboMesh lines;
    ofVboMesh prep_lines;
    
    vector<gvWall*> gvws;
  
    ofImage * colref;
    
    static int pid;
    
    
    vector<ofPolyline> polys;
  
    int wall_thickness;
    
};