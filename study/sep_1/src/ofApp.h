#pragma once

#include "ofMain.h"
#include "ofxExportImageSequence.h"

class ofApp : public ofBaseApp{

public:
    void setup();
        void setup_export_layer( int w, int h, int num );

    void update();
    
    void draw();
        void draw_all();
        void draw_layer_0();
        void draw_layer_1();
        void draw_layer_2();
        void draw_preview();
        void draw_info();
    
    void keyPressed(int key);


    bool bStart;
    bool bDrawInfo;
    int layer_num;
    int sel_layer;
    vector<ofxExportImageSequence> exps;
};
