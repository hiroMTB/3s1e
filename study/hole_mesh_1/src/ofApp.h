#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();

	void setVertices();
	void setIndices();
	void draw();
	void draw_info();
	void keyPressed(int key);
	
	ofVboMesh mesh;
	ofEasyCam cam;
	
	
	int mW;
	int mH;
};
