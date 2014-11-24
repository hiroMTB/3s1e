#include "ofApp.h"

void ofApp::setup(){

	bgColor.set(0.8);
	drawColor.set(0.1);
	
	ofSetWindowPosition( 0, 0 );
	ofSetWindowShape( 500, 500 );
	gui.setup( "myGuiGenereted.maxpat", 500, 0, 500, 500-25 );
	
	int x = 45;
	int y = 50;
	
	gui.addBang("myBang", x, y+=30, this, &ofApp::myBang1);
	gui.addToggle("myToggle", x, y+=30, &myToggle);
	gui.addInt("myInt", x, y+=30, &myInt);
	gui.addFloat("myFloat", x, y+=30, &myFloat);
	gui.addColor("drawColor", x, y+=30, &drawColor);
	gui.addColor("bgColor", x, y+=70, &bgColor);

	time_stamp = ofGetTimestampString();
	gui.addComment(x, 500-60, 300, 20, "Generated                     " + time_stamp );

	gui.write();
	gui.open();
}

void ofApp::update(){
	gui.update();
}

void ofApp::draw(){
	
	ofBackground( bgColor );
	ofSetColor( drawColor );
	ofDrawBitmapString("OSC Port       " + ofToString(gui.getOscReceivePort()), 50, 45);
	
	ofSetColor(255, 0, 0);
	ofFill();
	ofCircle(173, 90, MAX(--csize, 3));
	
	ofSetColor( drawColor );
	stringstream ss;
	ss << "myBang\n\n";
	ss << "myToggle       " << myToggle << "\n\n";
	ss << "myInt          " << myInt << "\n\n";
	ss << "myFloat        " << myFloat << "\n\n";
	ss << "drawColor\n\n";

	ofDrawBitmapString( ss.str(), 50, 95 );
	
	ofSetColor( drawColor );
	ofRect(170, 195, 200, 60);
	
	ofDrawBitmapString("Compiled      " + time_stamp, 50, 500-45);

}

void ofApp::myBang1(){
	cout << "bang 1" << endl;
	csize = 10;
}


void ofApp::exit(){
	gui.close();
}

void ofApp::keyPressed( int key ){
	
	switch ( key) {
		case ' ':
			gui.open();
			break;
			
		default:
			break;
	}
}