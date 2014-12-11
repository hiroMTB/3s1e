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
    
private:
    gvline();
    
public:
    gvline( ofVec2f _p1, ofVec2f _p2)
    :p1(_p1), p2(_p2){
        attrs.setUsage( GL_DYNAMIC_DRAW );
        attrs.setMode( OF_PRIMITIVE_POINTS );
        inited = true;
    };
    
    bool bNegativeGrav;
    bool bGrav;
    bool inited;
    ofVec2f p1;
    ofVec2f p2;
    ofVec2f norm;
    ofVboMesh attrs;

};


class ad_grav_line{
    
public:
    
    ad_grav_line();
    void setup( ofImage * img=NULL, ofCamera * cam = NULL );
    void add_random_particle( int num );
    void create_line( ofVec2f p1, ofVec2f p2, float density=0.1 );
    void update();
    void update_lines();
    void update_points();
    void update_attrs();
    void draw();
    void draw_attr();
    void onCollision( ofxBulletCollisionData& cdata );

    bool bInitPhysics;
    float frame;

    ofxBulletWorldRigid world;
    vector<btSphereShape*> sphereShapes;
    vector<ofxBulletBaseShape*>	shapes;
    vector<ofVec3f> pivots;
    
    vector<ofVec3f> collision;
    vector<ofxBulletJoint*>	joints;
//    vector<ofxBulletJoint*>	joints;
    
    ofVboMesh points;
    ofVboMesh lines;
    ofVboMesh prep_lines;
    
    vector<gvline*> gvls;
  
    ofImage * colref;
};