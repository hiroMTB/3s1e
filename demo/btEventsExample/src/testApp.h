#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	
	void onCollision(ofxBulletCollisionData& cdata);
	void mousePickEvent( ofxBulletMousePickEvent &e );

	void keyPressed  (int key);
	void keyReleased(int key);
	
	ofxBulletWorldRigid			world;
	vector <ofxBulletBox*>		bounds;
	float						boundsWidth;
	
	ofLight						light;
	
	btBoxShape*					boxShape;
	btSphereShape*				sphereShape;
	vector<ofxBulletBaseShape*>	shapes;
	vector<bool>				bColliding;
	int							mousePickIndex;
	ofVec3f						mousePickPos;
	
	bool						bDrawDebug;
	bool						bRenderShapes;
	bool						bAddCenterAttract;
	bool						bSpacebar;
	
	ofCamera					camera;
		
};
