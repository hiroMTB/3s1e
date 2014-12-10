//
//  ad_attractor.h
//  constraint_2
//
//  Created by mtb on 10/12/14.
//
//

#pragma once

#include "ofMain.h"

class ad_attractor{
	
public:
	ad_attractor(){};
	ad_attractor( ofVec3f _pos, float _power, float _speed );
	
	static void draw_all();
	ofVec3f getPosition();
	const vector<ofVec3f> & getPositions() const;
	vector<ofVec3f> & getPositions();
	
	ofVec3f init_pos;
	int id;
	float power;
	float speed;
	
private:
	static ofVboMesh points;
	static bool init;
	
};

