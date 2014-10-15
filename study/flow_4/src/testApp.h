#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"

class testApp : public ofBaseApp{
    
public:

	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);

    bool bInit;
    ofVboMesh points;
    ofVboMesh lines;
    
    ofEasyCam cam;
    ofxAlembic::Reader abc;
    vector<ofVec3f> initial_pos;
  
};