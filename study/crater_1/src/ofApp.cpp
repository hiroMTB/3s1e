#include "ofApp.h"

void ofApp::setup(){

	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
	
    img.loadImage( "crater2.jpg" );
    int width = img.getWidth();
    int height = img.getHeight();
    
	for( int y = 0; y < height; y++ ){
		for( int x = 0; x<width; x++ ){
			mainMesh.addVertex( ofPoint(x,y,0) );
			mainMesh.addColor( ofFloatColor(0,0,0) );
		}
	}
	
	for( int y = 0; y<height-1; y++ ){
		for( int x=0; x<width-1; x++ ){
			mainMesh.addIndex( x+y*width );				// 0
			mainMesh.addIndex( (x+1)+y*width );			// 1
			mainMesh.addIndex( x+(y+1)*width );			// 10
			
			mainMesh.addIndex( (x+1)+y*width );			// 1
			mainMesh.addIndex( (x+1)+(y+1)*width );		// 11
			mainMesh.addIndex( x+(y+1)*width );			// 10
		}
	}
	
	extrusionAmount = 100.0;
}

void ofApp::update(){

    int ch = img.getPixelsRef().getNumChannels();

    for( int y=0; y<img.getHeight(); y++ ){
        for( int x=0; x<img.getWidth(); x++ ){
            int index = x + y*img.getWidth();
            ofFloatColor c = img.getPixelsRef().getColor( x, y );
            ofVec3f tmpVec = mainMesh.getVertex( index );
            tmpVec.z = c.getBrightness() * extrusionAmount;
            mainMesh.setVertex( index, tmpVec );
            mainMesh.setColor( index, c );
        }
    }
}

void ofApp::draw(){
	ofEnableDepthTest();
	cam.begin();
    ofTranslate( -img.getWidth()/2, - img.getHeight()/2 );
	mainMesh.drawWireframe();
    mainMesh.drawVertices();
	cam.end();
	
	ofSetColor( 255 );
	string msg = "fps: " + ofToString(ofGetFrameRate(), 2 );
	ofDrawBitmapString( msg, 10, 20 );
	
}

void ofApp::keyPressed( int key ){
	switch(key) {
		case 'f':
			ofToggleFullscreen();
			break;
            
        case OF_KEY_UP:
            extrusionAmount += 10;
            break;

        case OF_KEY_DOWN:
            extrusionAmount -= 10;
            break;
	}
}
