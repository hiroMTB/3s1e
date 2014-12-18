#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"
#include "ofxVoronoi.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();

	void setVertices();
	void setIndices();
    void setColors();
	void draw();
	void draw_info();
	void keyPressed(int key);
	
    vector<ofVec3f> seeds;
//    ofVboMesh mesh;
	ofEasyCam cam;
    ofxGpuNoise gn1, gn2;
    vector<ofVec3f> nd1, nd2;
    vector<ofVec3f> possition, speeds;
    vector<int> hole;
    int mW;
	int mH;
    
    ofImage img;
    
    ofxVoronoi voro;
};
