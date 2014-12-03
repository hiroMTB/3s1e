#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

#include "ofxBulletP2PConstraint.h"

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
	void mousePressed(int x, int y, int button);
	
	ofxBulletWorldRigid			world;
	
	vector<ofxBulletBaseShape*> shapes;
	vector<ofxBulletJoint*>		joints;
    vector<ofxBulletP2PConstraint*>	p2ps;
	
	bool						bDrawDebug;	
	float						jointLength;
	
	ofCamera					camera;
	ofLight						light;
	ofVec3f						mousePos;

    vector<Attr> attrs;


    ofVboMesh points, lines;
};
