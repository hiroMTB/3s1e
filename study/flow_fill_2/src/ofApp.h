#pragma once

#include "ofMain.h"
#include "ofxAlembic.h"
#include "ad_image_saver.h"
#include "ofxGpuNoise.h"
#include "ofxExportImageSequence.h"
#include "ofxSimpleSpline.h"


class Vine{

public:
	Vine():	radius(0), angle(0), past_top(ofVec3f(0,0,0)), age(0){}
	float radius;
	float angle;

	int age;
	int num_cv;
	ofVec3f past_top;
	vector<ofVec3f> speeds;
	vector<ofVec3f> accels;
    vector<float> freq;
    vector<float> phase;

};

class ofApp : public ofBaseApp{
    
public:
	void setup();
	void load_svg( string path = "" );
    void setup_window( int x, int y );

    void reset();
    void update();
	void update_particle();
	void update_spline();
	void add_particle();
	
	void draw();
	void keyPressed( int key );
	void draw_info();
	

    static ofColor orange;

	bool bStart;
	float frame;
    float rot;
    float max_dist;
    float gAngle;

    ofVec3f start;
    ofVec3f end;

	vector<ofxSimpleSpline> splines;
	vector<Vine> vines;
    ofVboMesh rf_points;
    ofVboMesh r_lines;

	ofxExportImageSequence exporter;
    ofPoint win;
    
    ofImage img;

    ofPath bg_shape;
    ofPath red_guide;
    ofPath green_guide;
    ofPath mask;

    ofVec2f spread_vec;
    
    char surface_char;
    
};
