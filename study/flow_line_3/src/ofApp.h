#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"
#include "ad_image_saver.h"
#include "ofxGpuNoise.h"

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
	bool bZeroDepth;
	bool bOrtho;
	
	float frame;
	float fov;
	
    ofVboMesh points;
    ofVboMesh lines;
    
    ofEasyCam cam;
    vector<ofVec3f> initial_pos;

	ofImage img;
	ad_image_saver saver;

	float rot;
	
	ofxGpuNoise gn;

	vector<float> rotation;
};