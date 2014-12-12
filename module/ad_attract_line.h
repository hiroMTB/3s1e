//
//  ad_attract_line.h
//  constraint_2
//
//  Created by mtb on 10/12/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

class Attr{
    
public:
    Attr(){};
    
    float power;
    float angle;
    float speed;
    float length;
    
    ofVec3f pivot;
};

class ad_attract_line{
	
public:
	ad_attract_line( vector<ofxBulletBaseShape*> * shapes, ofVec3f start, ofVec3f end, int num, float length, float power=1.0, float speed=1.0 );
    
    void update();
	void draw();
    
    ofVec3f start;
    ofVec3f end;
    
    vector<ofVec3f> & getPositions();
    vector<Attr> attrs;
    
private:
	ofVboMesh points;

    vector<ofxBulletBaseShape*> * shapes;
    ofVec3f norm;
    ad_attract_line(){};
};

