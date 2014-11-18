#include "ofApp.h"
#include "ad_geom_util.h"

void ofApp::setup(){
	b2dTest = true;
	setRandom_tri();
	setRandom_p();
}

void ofApp::setRandom_tri(){
	tri.p0.set( ofRandomf(), ofRandomf(), ofRandomf() );
	tri.p1.set( ofRandomf(), ofRandomf(), ofRandomf() );
	tri.p2.set( ofRandomf(), ofRandomf(), ofRandomf() );

	tri.p0 *= 300;
	tri.p1 *= 300;
	tri.p2 *= 300;

	if( b2dTest )
		tri.p0.z = tri.p1.z = tri.p2.z = 0;
}

void ofApp::setRandom_p(){
	point.set( ofRandomf(), ofRandomf(), ofRandomf() );
	point *= 100;
	if( b2dTest )
		point.z = 0;
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

	
	// draw trignale
	ofSetColor(0);
	tri.draw();
	
	// draw point
	ofCircle(point, 3);
	
	
	// calculate prependicular line
	ofVec3f d1 = ad_geom_util::vec_pl(point, tri.p0, tri.p1);
	ofVec3f d2 = ad_geom_util::vec_pl(point, tri.p1, tri.p2);
	ofVec3f d3 = ad_geom_util::vec_pl(point, tri.p2, tri.p0);
	
	ofSetColor(0, 220, 0);
	ofLine( point, point+d1 );
	ofLine( point, point+d2 );
	ofLine( point, point+d3 );
	
	if(!b2dTest){
		cam.end();
	}

	ofPopMatrix();

	ofPushMatrix(); {
		ofSetupScreenOrtho();

		ofSetColor(0);
		ofDrawBitmapString("space key to change setting", 20, 20);
		ofDrawBitmapString("m key to change 2d - 3d Mode", 20, 40);
		
	} ofPopMatrix();
}

void ofApp::keyPressed(int key){
	if( key == ' '){
		
		setRandom_p();
		setRandom_tri();
		
	}else if( key== 'p'){
		
		setRandom_p();
		
	}else if( key== 't'){

		setRandom_tri();
		
	}else if( key == 'm'){
		
		b2dTest = !b2dTest;
		
	}
}
