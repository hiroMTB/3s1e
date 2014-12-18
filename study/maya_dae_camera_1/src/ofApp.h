#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
    
        ofxAssimpModelLoader dae;
    
		bool bAnimate;
        bool bAnimateMouse;
        float animationPosition;

        ofMesh mesh;
        ofLight	light;
    
        ofEasyCam cam;
    
    ofVec3f campos;
    ofVec3f target;
    ofVec3f model_pos;
    ofVboMesh mymesh;
};
