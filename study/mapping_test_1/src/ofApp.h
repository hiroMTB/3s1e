#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
    
    ofImage img;
    int w, h;
    ofRectangle plane;
    
    ofEasyCam cam;
    
    GLint min, max;
};
