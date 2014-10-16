#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
    void draw_info();
    void onCollision( ofxBulletCollisionData& cdata );
	void keyPressed( int key );
    void add_dust( int num );

    bool bDraw_info;
	bool bDebug_draw;
	
    ofEasyCam cam;
    ofVboMesh points;
    ofVboMesh lines;

    ofxBulletWorldRigid world;
	btSphereShape * sphereShape;
    ofxBulletBox * attracor;
    vector<ofxBulletBaseShape*>	shapes;
    
    float rot_angle;
	vector<ofVec3f> initial_pos;

	ofVboMesh viz1;
	
};
