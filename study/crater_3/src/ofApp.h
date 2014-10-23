#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void keyPressed( int key );
    void load_mesh( ofImage &img, ofVec2f start, int resolution, float extrusion, int size );
    void make_random_mesh();
    void set_indices();
    int res;
    int file_num;
    float extrusionAmount;
    ofEasyCam cam;
	ofVboMesh mainMesh;

    
    int mesh_w, mesh_h;
};
