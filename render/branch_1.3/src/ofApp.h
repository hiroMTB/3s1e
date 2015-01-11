#pragma once

#include "ofMain.h"
#include "ofxExportImageSequence.h"
#include "Branch.h"
#include "ofxSvg.h"

class ofApp : public ofBaseApp{

public:

    static ofApp * app;
    static ofApp * init();
    void setup();
        void setup_export_layer( int w, int h, int num );
        void setup_scene();
    void update();
    
    void draw();
        void draw_all();
        void draw_layer_0();
        void draw_layer_1(){};
        void draw_layer_2(){};
        void draw_preview();
        void draw_info();
    
    void keyPressed(int key);


    bool bStart;
    bool bDrawInfo;
    bool bDebugDraw;
    int layer_num;
    vector<ofxExportImageSequence> exps;
    
    vector<Branch> tree;
    
    ofImage img;
    ofxSVG svg;
    
    float gAngle;
    ofVec2f start;

    ofPoint win;
    
    double start_time;
    float max_dist;
    int max_frame;

};
