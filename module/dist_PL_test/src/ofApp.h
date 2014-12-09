#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void draw();
	void keyPressed(int key);
	void setRandom();
	
	ofVec3f p1, p2;
	ofEasyCam cam;
	bool b2dTest;
    
    vector<ofVec3f> points;
};
