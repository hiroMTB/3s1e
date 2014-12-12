#include "ofApp.h"


void ofApp::setup(){
	
	mesh.setUsage( GL_DYNAMIC_DRAW );
	mesh.setMode( OF_PRIMITIVE_TRIANGLES );
	
	mW = 30;
	mH = 30;
	
	setVertices();
	setIndices();

	cam.enableOrtho();
	cam.lookAt( ofVec3f(0,0,0), ofVec3f(0,1,0) );
	cam.setDistance( mW );
}

void ofApp::setVertices(){

	mesh.clearVertices();
	
	float frame = ofGetFrameNum() * 0.01;

	for( int i=0; i<mH; i++ ){
		for( int j=0; j<mW; j++ ){
			
			float x = j - mW/2;
			float y = i - mH/2;
			float z = 0;

			x += ofSignedNoise(x, frame) * 0.1;
			y += ofSignedNoise(y, frame) * 0.1;
			z += ofSignedNoise(z, frame) * 0.1;
			
			mesh.addVertex( ofVec3f(x, y, z) );
			mesh.addColor( ofFloatColor( 0.2, 0.75) );
		}
	}
}



/*
 
	Mesh indexing
	w - w+1 - w+2 - w+3 - w+n - (2w-1)
	0 -   1 -   2 -   3 -   n - ( w-1)
 
*/
void ofApp::setIndices(){
	
	mesh.clearIndices();
	
	float frame = ofGetFrameNum() * 0.001;
	
	for( int i=0; i<mH-1; i++ ){
		for( int j=0; j<mW-1; j++){
			int index = j + i*mW;
			
			if( ofNoise(j, i, frame)>0.7 ){

				mesh.addIndex( index );
				mesh.addIndex( index+1 );
				mesh.addIndex( index+mW );
			}
				
			if( ofNoise(j, i, frame*0.5)>0.7 ){
				
				mesh.addIndex( index+1 );
				mesh.addIndex( index+1+mW );
				mesh.addIndex( index+mW );
			}
		}
	}
}


void ofApp::update(){
	setVertices();
	setIndices();
}


void ofApp::draw(){

	glPointSize(1);
	glLineWidth(1);
	
	cam.begin();
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
	ofScale( mW, mW );
	mesh.draw();
	mesh.drawWireframe();
	mesh.drawVertices();
	cam.end();


	draw_info();
}

void ofApp::draw_info(){
	
	stringstream ss;
	ss << "fps     : " << ofGetFrameRate() << "\n";
	
	ofSetColor(0);
	ofDrawBitmapString(ss.str(), 40, 40 );
	
}

void ofApp::keyPressed(int key){

}


