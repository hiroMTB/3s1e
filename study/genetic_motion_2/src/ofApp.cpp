#include "ofApp.h"

ofApp::ofApp(){
    bDraw_info = true;
    bStart = false;
    bAnimate = false;
    bRotate = false;
    bOrtho = false;
}

void ofApp::setup(){

    num_agent = 400;
	for ( int i=0; i<num_agent; i++ ) {
		ofVec3f loc( 10, 0, 0 );
        LineAgent l;
        l.setup( loc );
        la.push_back( l );
	}
	
	ofSetFrameRate( 60 );
}

void ofApp::update(){

    if( bStart ){
        for ( int i=0; i<num_agent; i++ ) {
            if( ofGetKeyPressed() || ofRandom(1.0)<0.3 ||i==num_agent-1 ){
                la[i].dna.setBoundsMode( i%3 );
                la[i].dna.mutate(  ofMap(mouseX, 0, ofGetWidth(), 0, 0.5)*0.4 );
            }else{
                la[i].dna.setBoundsMode( i%3 );
                la[i].dna.setMateMode( floor(ofRandom(0,4)) );
                la[i].dna.mate( la[i+1].dna, ofMap(mouseX, 0, ofGetWidth(), 0, 0.1) *0.4 );
            }
            
            la[i].add_result();
            
            if( bAnimate ){
                la[i].animate();
                la[i].animate_noise( i );
            }
        }
    }
}

void ofApp::draw(){
	
    ofEnableAlphaBlending();
    
    
	ofBackground( 255 );
    
    ofSetColor( 255 );
    if( bOrtho )
        ofSetupScreenOrtho();

    ofPushMatrix();
    ofTranslate( 50, ofGetHeight()/2 );
//    ofRotate( ofGetFrameNum(), 1, 0, 0 );
	for ( int i=0; i<num_agent; i++ ) {
//        ofTranslate( 0, 3 );
		la[i].draw();
	}
    ofPopMatrix();

    draw_info();
}

void ofApp::draw_info(){
    if( !bDraw_info) return;
    ofSetColor( 200 );
    ofRect( 5, 10, 400, 200 );
    int y = 20;
    ofSetColor( 0 );
    ofDrawBitmapString( "space key : start genetic calculation", 20, y+=20 );
    ofDrawBitmapString( "f     key : full screen",               20, y+=20 );
    ofDrawBitmapString( "a     key : start animation",           20, y+=20 );
    ofDrawBitmapString( "o     key : toggle ortho graphic view", 20, y+=20 );
    ofDrawBitmapString( "I     key : draw info",                 20, y+=20 );
}

void ofApp::keyPressed( int key ){

    switch( key ) {
		case ' ':
            bStart = !bStart;
            break;
    
        case 'f':
            ofToggleFullscreen();
            break;

        case 'a':
            bAnimate = !bAnimate;
            break;
        
        case 'o':
            bOrtho = !bOrtho;

        case 'I':
            bDraw_info = !bDraw_info;
            break;
        default:
            break;
	}
}
