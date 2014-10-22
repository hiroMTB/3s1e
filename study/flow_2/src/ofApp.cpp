#include "ofApp.h"

void ofApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	
	string path = "test1.abc";
	abc.open(path);
    abc.dumpNames();
    
    cam.setDistance( 15 );
    cam.setNearClip( 0.01 );
    cam.setFarClip( 1000 );

    ofVboMesh m;
    m.setUsage( GL_DYNAMIC_DRAW );
    for( int i=0; i<4; i++ ){
        m.setMode( OF_PRIMITIVE_LINES );
        lines.push_back( m );
        
        m.setMode( OF_PRIMITIVE_POINTS );
        points.push_back( m );
    }

}

void ofApp::exit(){
	abc.close();
}

void ofApp::update(){
	float t = fmodf(ofGetElapsedTimef(), abc.getMaxTime());
	
	// update alemblic reader with time in sec
	abc.setTime( t );

    vector<ofVec3f> pos;
    abc.get( 0, pos );

    
    for( int i=0; i<points.size(); i++ ){
        points[i].clear();
    }
    
    for( int i=0; i<pos.size(); i++ ){
        int type = ( 5.0-log10(ofRandom(1.0, 10000.0)) ) - 1.0;
        type = MAX( 0, type );
        ofVec3f &p = pos[i];
        //p.x = 0;
        points[type].addVertex( p );
        
        ofFloatColor c;
        c.white;
        c.setHsb( ofRandom(1.0), 0.3, 0.6, 0.6 - type*0.1 );
        points[type].addColor( c );
    }
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    ofBackground( 255 );
	cam.begin();
    
	glPointSize(1);
    
    ofPushMatrix();
    ofSetColor( 20 );
    for( int i=0; i<points.size(); i++ ){
        glPointSize( points.size()-i-1 );
        points[i].draw( OF_MESH_POINTS );
    }
    
    ofPopMatrix();
    
    cam.end();
    
    
    ofSetColor( 0 );
    for( int i=0; i<points.size(); i++ ){
        ofDrawBitmapString( "point size" + ofToString(i) + " : " + ofToString( points[i].getNumVertices() ), 10, 20+20*i);
    }
}

void ofApp::keyPressed(int key){
}