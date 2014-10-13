#pragma once

#include "ofMain.h"
#include "ad_image_saver.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();    
    void keyPressed(int key);
    void make_line();
    void make_points();
    
    bool bDraw_underline;
    bool bSmooth;
    float gap;
    float rand_y;
    
    ofFloatColor color;
    vector<ofVboMesh> lines;
    vector<ofVboMesh> points;

    ad_image_saver saver;
    
};
