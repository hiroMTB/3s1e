#pragma once

#include "ofMain.h"
#include "ofxExportImageSequence.h"
#include "ofxSimpleSpline.h"

class ofApp : public ofBaseApp{

public:
    void setup();
	void setup_export_layer( int w, int h, int num );
	void setup_spline();
    void add_spline();
	
    void update();
    
    void draw();
        void draw_all();
        void draw_layer_0();
        void draw_preview();
        void draw_info();
    
    void keyPressed(int key);


    bool bStart;
    bool bDrawInfo;
    int layer_num;
    int sel_layer;
	int hisnum;

	ofPoint win;
	
	vector<ofxSimpleSpline> sps;
	vector< list<ofxSimpleSpline> > sps_p;
	vector< vector<ofVec3f> > points;

	ofVboMesh cps;
    ofImage img;

	ofxExportImageSequence exporter;
	
};
