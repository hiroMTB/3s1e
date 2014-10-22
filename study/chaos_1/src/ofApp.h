#pragma once

#include "ofMain.h"
#include "StrangeAgent.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
    void calc_sa();
	void keyPressed(int key);

    ofEasyCam cam;    
    vector<StrangeAgent> sa;
};
