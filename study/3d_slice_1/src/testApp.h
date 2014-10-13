#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"

class testApp : public ofBaseApp
{
public:

	void setup();
	void update();
	void draw();
    void draw_info();
    void exit();
	void keyPressed(int key);
	
    ofEasyCam cam;    
    ofxAlembic::Reader abc;

    ofVboMesh mesh;
    ofVboMesh points;
    ofVboMesh lines;
    
};