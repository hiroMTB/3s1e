#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ad_image_saver.h"
#include "ad_grav_line.h"

class ofApp : public ofBaseApp{

public:
	void setup();
    void setup_physics();
    void setup_particles();
    void setup_attrs();
    
	void update();
    void update_points();
    void update_lines();
    void update_attrs();
    
    void draw();
    void draw_info();
    void onCollision( ofxBulletCollisionData& cdata );
	void keyPressed( int key );
    void apply_grav();
    
    bool bDraw_info;
    bool bGrav;
    bool bNegativeGrav;
    
    float frame;
    
    ofEasyCam cam;
    ofVboMesh points;
    ofVboMesh lines;
	ofVboMesh attrs;

    ofxBulletWorldRigid world;
	vector<btSphereShape*> sphereShapes;
    vector<ofxBulletBaseShape*>	shapes;
    vector<ofVec3f> collision;
    
    ad_grav_line gravline;
    ad_image_saver saver;

};
