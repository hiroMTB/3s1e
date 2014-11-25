#include "ofMain.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);

	vector<ofVec3f> units;
};


void ofApp::setup(){
	
	units.assign(4);
	units[0].set( 10, 30, 0); units[0].normalize();
	units[1].set( -60, 3, 0); units[0].normalize();
	units[2].set( 12, 110, 0); units[0].normalize();
	units[3].set( -10, 90, 0); units[0].normalize();
}


void ofApp::update(){
	
}


void ofApp::draw(){
	ofBackground(255);
	
	ofPushMatrix();
	ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
	ofSetColor(30);

	ofRect( 0, 0, 100, 100);

	ofSetLineWidth(2);
	ofVec3f zero(0,0,0);
	for( int i; i<units.size(); i++ ){
		ofLine(zero, units[0] * 100.0);
	}
	
	ofPopMatrix();
}


void ofApp::keyPressed(int key){
	
}



void ofApp::dragEvent(ofDragInfo dragInfo){
	
}


int main( ){
	ofSetupOpenGL(500,768, OF_WINDOW);
	ofRunApp( new ofApp());
}
