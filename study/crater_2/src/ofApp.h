#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
    void set_indices();
	void draw();
    void draw_info();
    void keyPressed( int key );
    void load_mesh( ofImage &img, ofVec2f start, int resolution, float extrusion, int size=1.0 );

    bool bDraw_info;
    bool bStart;
    ofImage img;
    ofEasyCam cam;
	ofVboMesh mainMesh;
    ofVboMesh dots;
    
    int mesh_w, mesh_h;
    vector<ofVec3f> speed;
    
};
