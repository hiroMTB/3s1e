#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ofxBulletP2PConstraint.h"
#include "constraint_line.h"


class Attr{
    
public:
    Attr(){};
    Attr( ofVec3f _pos, float _power, float _speed): pos(_pos), power(_power), speed(_speed){}
    ofVec3f pos;
    float power;
    float speed;
    
    void draw(){
        ofSetColor(0,255,0);
        ofCircle( pos, 3 );
    }
};


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

    vector<Attr> attrs;


    constraint_line *cline1, *cline2, *cline3;
};
