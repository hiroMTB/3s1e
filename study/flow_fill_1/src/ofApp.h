#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"
#include "ad_image_saver.h"
#include "ofxGpuNoise.h"
#include "ofxExportImageSequence.h"
#include "ofxSimpleSpline.h"

class ofApp : public ofBaseApp{
    
public:

	void setup();
	void update();
	void draw();
	void keyPressed( int key );
	void draw_info();
	
	bool bStart;
	
	float frame;
    float rot;

    ofVboMesh points;
    ofVboMesh points_a;
    ofVboMesh lines;
    
    vector<ofVec3f> initial_pos;

	ofImage img;
	ofxExportImageSequence exporter;
    vector<ofxSimpleSpline> splines;
    ofVboMesh tracker;
    vector<ofVboMesh> spRmxs;
};