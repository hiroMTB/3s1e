//
//  ofApp.h
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#pragma once

#include "ofMain.h"
#include "ad_grav_wall.h"
#include "ofxGpuNoise.h"
#include "ofxSvg.h"
#include "ofxExportImageSequence.h"

class ofApp : public ofBaseApp{
    
public:
	
    static bool bRender;
    static float density;
    
	static ofApp * app;
	static ofApp * init(){ app = new ofApp(); return app; }
	
    void setup();
    void update();
    void draw();
    void draw_info();
    void keyPressed( int key );
	
    void load_svg( string path );
    void load_noise();
    void setup_window( int w, int h );
    
	inline float getNoise( int index, int ch=0 ){
		index %= noise_size;
		return (float)noise[index*3 + ch]/255.0;
	}

	inline float getSignedNoise( int index, int ch=0 ){
		index %= noise_size;
		return (float)(noise[index*3 + ch]-128)*2.0/255.0;
	}

    bool bStart;
    bool bInit;
    bool bDraw_info;
    int mode;
    float frame;
    int noise_size;
    unsigned char * noise;
    
    ofPoint global_pivot;
    ofPoint win;
    ofEasyCam cam;
	ofxGpuNoise gpu_noise;
    ofxSVG svg;
    ofImage img;
    ofPath mask;
    vector<ofPath> wall_path;
    vector<ofxExportImageSequence> exps;

    ad_grav_wall grav_wall;
	
	
	int sel_layer;
};
