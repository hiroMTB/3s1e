#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"
#include "ad_image_saver.h"

class ofApp : public ofBaseApp{
    
public:

	void setup();
	void update();
	void draw();
	void keyPressed( int key );
	void draw_info();
	
    bool bInit;
	bool bDraw_info;
	bool bStart;

	float frame;
	
    ofVboMesh points;
    ofVboMesh lines;
    
    ofEasyCam cam;
    vector<ofVec3f> initial_pos;
	
	ad_image_saver saver;

};