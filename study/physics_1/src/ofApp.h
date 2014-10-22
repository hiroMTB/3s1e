#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
    void draw_info();
    void onCollision( ofxBulletCollisionData& cdata );
	void keyPressed( int key );
    void add_dust( int num );

    bool bDraw_info;

    ofEasyCam cam;
    ofVboMesh points;
    ofVboMesh lines;

    ofxBulletWorldRigid world;
	btSphereShape * sphereShape;
    ofxBulletBox * attracor;
    vector<ofxBulletBaseShape*>	shapes;
    
    float rot_angle;
};
