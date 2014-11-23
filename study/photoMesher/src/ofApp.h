#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
    void keyPressed( int key );
    void load_mesh( ofFloatImage &img );
    void update_mesh();
    void set_indices();
    void dragEvent( ofDragInfo info );
    
    bool bMove;
    int res;
    float extrusion;
    float threthold;

    ofFloatImage img;
    ofEasyCam cam;
    ofVboMesh mesh;
    ofxGpuNoise gn;
};
