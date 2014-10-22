#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"

class ofApp : public ofBaseApp{
    
public:

	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);

    bool bInit;
    bool bStart;
    float frame;
    ofVboMesh points;
    ofVboMesh lines;
    
    ofEasyCam cam;
    ofxAlembic::Reader abc;
    vector<ofVec3f> initial_pos;
  
};