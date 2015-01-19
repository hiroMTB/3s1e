#pragma once

#include "ofMain.h"
#include "ofxSimpleSpline.h"
#include "Physics1.h"
#include "Physics2.h"

#include "ofxExportImageSequence.h"

class Whip{

public:
    float freq;
    float amp;
    float length;
};

class ofApp : public ofBaseApp
{
public:

    void setup();
    void setup_export_layer( int w, int h, int num );

    void update();
	void draw();
    void keyPressed( int key );
    
    vector<RopeSimulation*> ropesim;
    vector< vector<ofVec3f>*> cvs;
    vector<ofxSimpleSpline*> curve;
    vector< vector<ofFloatColor> > cols;

    bool bDebugDraw;

    ofVboMesh connecter;

    ofImage img;
    float scale;

    vector< list<ofxSimpleSpline> > curve_history;
    
    int hisnum;

    ofxExportImageSequence exporter;
    ofPoint win;    

    vector<Whip> whips;
};


