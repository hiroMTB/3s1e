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
    void add_dust( int num, ofVec3f center, float random=10.0 );
    void make_wall();
    
    bool bDraw_info;
	bool bDebug_draw;
    bool bViz1_draw;
    bool bAttract;
    
    ofEasyCam cam;
    ofVboMesh points;
    ofVboMesh lines;
    ofVboMesh viz1;
    ofVboMesh attrs;
    ofVboMesh trail;

    ofxBulletWorldRigid world;
    ofxBulletBox * attracor;
    btSphereShape * sphereShape;
    vector<ofxBulletBaseShape*>	shapes;
    vector <ofxBulletBox*>		bounds;
    
	vector<ofVec3f> initial_pos;

    int trail_head;
    static const int trail_max = 10000*10;
};
