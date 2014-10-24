#pragma once

#include "ofMain.h"
#include "ad_image_saver.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
    void draw_info();
    void keyPressed( int key );
    void load_mesh( ofImage &img, ofVec2f start, int resolution, float extrusion, int size );
    void make_random_mesh();
    void set_indices();

    bool bDraw_info;
    bool bStart;
    int mesh_w, mesh_h;
    int res;
    float extrusionAmount;
    ofEasyCam cam;
	ofVboMesh mainMesh;
    
    ad_image_saver saver;
};
