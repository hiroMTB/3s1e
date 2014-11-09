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

    ofxGpuNoise noise1, noise2, noise3, noise4;
    
    ofVboMesh mesh;
    ofEasyCam cam;
};
