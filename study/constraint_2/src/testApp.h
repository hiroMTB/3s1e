#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "constraint_line.h"
#include "ad_attract_line.h"
#include "ofxSvg.h"
#include "ofxExportImageSequence.h"
#include "ad_attract_line.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void keyPressed  (int key);
	
	ofxBulletWorldRigid			world;
		
	bool						bDrawDebug;	
	float						jointLength;
	
	ofCamera					camera;
	ofLight						light;

    vector<ad_attract_line*> attrs;
    vector<constraint_line*> clines;
    
    ofxSVG svg;
	
	ofxExportImageSequence exporter;
};
