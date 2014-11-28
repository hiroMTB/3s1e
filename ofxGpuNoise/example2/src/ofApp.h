#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);

	float w, h;
	ofxGpuNoise noise1, noise2;
    ofVboMesh mesh1, mesh2;
};
