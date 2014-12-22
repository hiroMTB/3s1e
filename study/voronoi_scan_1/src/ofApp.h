#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"
#include "ofxVoronoi.h"
#include "ofxExportImageSequence.h"

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

    ofxGpuNoise gn1, gn2;
    vector<ofVec3f> nd1, nd2;
    vector<ofVec3f> possition, speeds;
    int mW;
	int mH;
    
    ofxVoronoi voro;
    ofxExportImageSequence exporter;
};
