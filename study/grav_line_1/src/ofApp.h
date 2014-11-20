#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ad_image_saver.h"

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
	ofVboMesh attrs;
	ofVboMesh p2p;

    ofxBulletWorldRigid world;
	btSphereShape * sphereShape;
    vector<ofxBulletBaseShape*>	shapes;

	
    ad_image_saver saver;
};
