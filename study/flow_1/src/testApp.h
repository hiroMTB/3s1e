#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"

class testApp : public ofBaseApp {
public:

	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed( int key );

    vector<ofVboMesh> lines;
    vector<ofVboMesh> points;

    ofEasyCam cam;
    ofxAlembic::Reader abc;

};