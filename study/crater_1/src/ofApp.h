#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void keyPressed( int key );
	
    float extrusionAmount;
    ofImage img;
    ofEasyCam cam;
	ofVboMesh mainMesh;

};
