#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"
#include "ofxMaxGui.h"

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
	bool bUseMask;
	bool bDrawLineMesh;
	bool bDrawMesh;
	bool bThruAccel;
	bool bThruSpeed;

	int res;
    float extrusion;
	float friction;
    float threthold;
	float animSpeed;
	
	float camDist;

    ofFloatImage img, mask;
    ofEasyCam cam;
    ofVboMesh mesh;
	ofVboMesh lines;
	
	ofxGpuNoise gn;
    ofxGpuNoise gn2;
	
	ofxMaxGui gui;
    
    vector<ofVec3f> speed, accel;

	ofFloatColor bg;
};
