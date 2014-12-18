#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
	
        ofxAssimpModelLoader model;
	
        ofMesh mesh;
        ofLight	light;
    
    ofEasyCam cam;
};

#endif
