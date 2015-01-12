#pragma once

#include "ofMain.h"
#include "ofxExportImageSequence.h"
#include "ofxSvg.h"

class bLine{
    
public:
    ofVec2f st;
    ofVec2f end;
    
    void draw(){
        ofLine( st, end );
    }
};

class ofApp : public ofBaseApp{

public:
    void setup();
        void setup_export_layer( int w, int h, int num );
        void setup_scene();

    void update();
    
    void draw();
        void draw_all();
        void draw_layer_0();
        void draw_preview();
        void draw_info();

    void keyPressed(int key);


    bool bStart;
    bool bDebugDraw;
    bool bDrawInfo;
    int layer_num;
    vector<ofxExportImageSequence> exps;
    
    int frame;
    
    ofVboMesh n_points;
    ofVboMesh points;
    ofVboMesh lines;
    ofVboMesh prep_lines;
    ofVboMesh prep_lines_b;

    ofVboMesh branchs;
    ofVboMesh flowers;
    ofVboMesh circles;
    
    ofxSVG svg;
    ofPoint win;
    ofPoint center;
    ofPoint st;
    ofPoint end;
    
    float gAngle;

    ofMesh circle;

    vector<bLine> bLines;
    
    list<int> branchIds;
    
    ofImage img;
    ofImage img2;
};
