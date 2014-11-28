#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void windowResized(int w, int h);

	float w, h;
	ofxGpuNoise noise;
    ofVboMesh mesh;
};
