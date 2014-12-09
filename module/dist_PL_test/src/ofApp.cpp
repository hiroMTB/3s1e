#include "ofApp.h"
#include "ad_geom_util.h"

void ofApp::setup(){
	b2dTest = true;
	setRandom();
}

void ofApp::setRandom(){
	p1.set( ofRandomf(), ofRandomf(), ofRandomf() );
	p2.set( ofRandomf(), ofRandomf(), ofRandomf() );

	p1 *= 200;
	p2 *= 200;

    for (int i=0; i<30; i++) {
        points.push_back(ofVec3f(ofRandomf(), ofRandomf(), ofRandomf()) *200 );
    }
}


void ofApp::draw(){

	ofPushMatrix();
	
	if(b2dTest){
		ofSetupScreenOrtho();
		ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
	}else{
		cam.begin();
	}
	ofBackground(255);
	ofSetColor(0);
	ofSetLineWidth(1);
	ofLine( p1, p2 );
	
	ofFill();
	ofSetColor(0);
    for (int i=0; i<points.size(); i++) {
        glPointSize(2);
        glBegin(GL_POINTS);
        glVertex3f(points[i].x, points[i].y, points[i].z);
        glEnd();
    }
    
	ofSetColor(0,255,0);
	ofCircle(p1, 5);
	
	ofSetColor(0,0, 255);
	ofCircle(p2, 5);
	
	// test1
    for (int i=0; i<points.size(); i++) {
        ofVec3f v = ad_geom_util::vec_pl(points[i], p1, p2);
        ofSetLineWidth( 1 );
        ofSetColor(20, 2, 111);
        ofLine(points[i], points[i]+v);
    }
    
	if(!b2dTest){
		cam.end();
	}

	ofPopMatrix();

	ofPushMatrix(); {
		ofSetupScreenOrtho();

		ofSetColor(0);
		ofDrawBitmapString("space key to change setting", 20, 20);
		ofDrawBitmapString("m key to change 2d - 3d Mode", 20, 40);
		
		// test2
		//float d1 = v.length();
		//float d2 = ad_geom_util::dist_pl(p0, p1, p2);
		//ofDrawBitmapString("distant via vec_pl : " + ofToString(d1), 20, 80 );
		//ofDrawBitmapString("distant via dist_pl : " + ofToString(d2), 20, 100 );
	} ofPopMatrix();
}

void ofApp::keyPressed(int key){
	if( key == ' ')
		setRandom();
	else if( key == 'm')
		b2dTest = !b2dTest;
}

