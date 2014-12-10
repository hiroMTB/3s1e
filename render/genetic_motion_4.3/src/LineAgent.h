//
//  lineAgent.h
//  genetic_motion_4.3
//
//  Created by mtb on 08/12/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxDna.h"

class LineAgent {

public:
	
	void setup( float _angle, float _length, int group );
	void add_result();
	void animate();
	void animate_noise_gpu( int agent_id );
	void animate_noise( int agent_id );
	void draw();
	void draw_connection_inside_of_agent();

	ofxDna dna;
	float g[10];
	float angle;
	float length;
	float noise_amp;
	float amp;
	int group;
	
	
	ofVec3f pos;
	ofFloatColor color;
	ofVboMesh trail;
	ofVboMesh drawer;
	
	vector<ofVec3f> speed;
};