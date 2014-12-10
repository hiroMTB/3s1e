#pragma once

#include "ofMain.h"
#include "ofxDna.h"
#include "ad_image_saver.h"
#include "ofxVectorGraphics.h"
#include "ofxGpuNoise.h"
#include "ofxExportImageSequence.h"

#include "LineAgent.h"

class ofApp : public ofBaseApp{

public:
	static ofApp * app;
	static ofApp * init(){ app = new ofApp; return app;}
	ofApp();
	void setup();
	void update();
	void draw();
    void draw_info();
    void draw_connection_between_agnet();
    void change_settings();
    void draw_vector_graphics();
    
    void keyPressed ( int key );
	void resized( int w, int h );

    bool bDraw_info;
	bool bStart;
    bool bAnimate;
    bool bRotate;
    bool bOrtho;
    bool bDraw_connection_inside_of_agent;
    bool bDraw_connection_between_agnet;
    bool bDraw_agent;
    bool bSequencial_add;
    
    int num_agent;
    int current_setting_start_frame;
	int frameFromStart;
    float in_angle;
    float out_angle;
    float initial_radius;
    float sequencial_add_speed;
    vector<LineAgent> la;
    
    ofVec3f center;
    ofVec3f pivot;
    ofVboMesh connection_between_agent;
    ad_image_saver saver;
    ofVboMesh connection_between_pivot;
	ofxGpuNoise noise;
    
    ofxExportImageSequence exporter;
	
	int current_group;
};
