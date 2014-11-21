//
//  ad_grav_line.h
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

struct gvline{
    
public:
    gvline( ofVec2f _p1, ofVec2f _p2)
    :p1(_p1), p2(_p2){
        attrs.setUsage( GL_DYNAMIC_DRAW );
        attrs.setMode( OF_PRIMITIVE_POINTS );
    };
    
    bool bNegativeGrav;
    bool bGrav;
    ofVec2f p1;
    ofVec2f p2;
    ofVec2f norm;
    ofVboMesh attrs;

};


class ad_grav_line{
    
public:
    
    ad_grav_line();
    void setup( ofCamera * cam );
    void add_random_particle( int num );
    void create_line( ofVec2f p1, ofVec2f p2 );
    void update();
    void update_lines();
    void update_points();
    void update_attrs();
    void draw();
    void onCollision( ofxBulletCollisionData& cdata );

    bool bInitPhysics;
    float frame;

    ofxBulletWorldRigid world;
    vector<btSphereShape*> sphereShapes;
    vector<ofxBulletBaseShape*>	shapes;
    vector<ofVec3f> collision;
    
    ofVboMesh points;
    ofVboMesh lines;
    
    vector<gvline> gvls;
    
};