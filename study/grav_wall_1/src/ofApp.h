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
#include "ad_image_saver.h"
#include "ofxGpuNoise.h"
#include "ofxSvg.h"
#include "ofxExportImageSequence.h"

class ofApp : public ofBaseApp{
    
public:
	
	static ofApp * app;
	static ofApp * init(){ app = new ofApp(); return app; }
	
    void setup();
    void update();
    void draw();
    void draw_info();
    void keyPressed( int key );
	
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
    float frame;
    
    ofEasyCam cam;
    ad_grav_wall grav_wall;
    ad_image_saver saver;
	
	ofxGpuNoise gpu_noise;
	int noise_size;
	unsigned char * noise;
    
    ofxSVG sABC;
    ofxExportImageSequence exporter;

    ofImage img;
};
