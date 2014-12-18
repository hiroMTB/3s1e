#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"
#include "ofxMaxGui.h"
#include "ofxExportImageSequence.h"
#include "ofxSvg.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
    void keyPressed( int key );
    void load_mesh( ofFloatImage &img );
    void set_indices();
    void dragEvent( ofDragInfo info );
    
	bool bMove;
	bool bDrawLineMesh;
	bool bDrawMesh;
	bool bThruAccel;
	bool bThruSpeed;
    bool bBaseMove;
    bool bMakeHole;
    
	int res;
    int mW, mH;
    float extrusion;
	float friction;
    float threthold;
	float animSpeed;
	float camDist;

    vector<ofVec3f> speed, accel;
    vector<ofVec3f> speed_b, accel_b;
    vector<float> hole_factors;

    ofFloatColor bg;
    ofFloatImage img;
    ofEasyCam cam;
    ofVboMesh mesh;

#ifdef USE_LINE_MESH
    ofVboMesh lines;
#endif
    
    ofxGpuNoise gn, gn2, gn3;
    ofxExportImageSequence exporter;
    
    //ofxSVG svg;
    //ofxMaxGui gui;

    int frame;
};
