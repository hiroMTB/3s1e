#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void keyPressed( int key );
    void make_quad( string file_name );
    void make_line( string file_name );
    void make_random_quad();
    int res;
    int file_num;
    float extrusionAmount;
    ofImage img;
    ofEasyCam cam;
	ofVboMesh mainMesh;
};
