#include "ofApp.h"

void ofApp::setup(){
    
    ofSetFrameRate( 60 );

    for( int i=0; i<100; i++ ){
        StrangeAgent s;
        s.setRandom();
        sa.push_back( s );
    }
}

void ofApp::update(){
    for( int j=0; j<256; j++ ){
        for( int i=0; i<sa.size(); i++ ){
            sa[i].update();
        }
    }
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofBackground( 255 );
    
    ofPushMatrix(); {
        cam.begin();
        float scale = 60;
        ofScale( scale, scale, scale );
        glPointSize( 1 );
        for( int i=0; i<sa.size(); i++ ){
            sa[i].draw();
        }
        cam.end();
        
    } ofPopMatrix();
}


void ofApp::keyPressed(int key){
    for( int i=0; i<sa.size(); i++ ){
        sa[i].setRandom();
    }
}
