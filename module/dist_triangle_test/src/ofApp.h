#pragma once

#include "ofMain.h"

class triangle{
public:
	ofVec3f p0, p1, p2;

	void draw(){
		glBegin( GL_LINE_LOOP);
		glVertex3f(p0.x, p0.y, p0.z);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glEnd();
	}
};

class ofApp : public ofBaseApp{
public:
	void setup();
	void draw();
	
	void keyPressed(int key);
	void setRandom_tri();
	void setRandom_p();
	bool b2dTest;
	
	triangle tri, tri2;	
	ofEasyCam cam;

	ofVec3f point;
};
