#include "testApp.h"

testApp::testApp(){
    bDraw_info = true;
    bStart = false;
    bAnimate = true;
    bRotate = false;
    bOrtho = false;
}

void testApp::setup(){

    num_agent = 100;
    ofVec3f center( 0, 0, 0 );

    for ( int i=0; i<num_agent; i++ ) {
        LineAgent l;
        l.setup( center, 300 );
        la.push_back( l );
	}
	
	ofSetFrameRate( 60 );
}

void testApp::update(){

    if( bStart ){
        for ( int i=0; i<num_agent; i++ ) {
            if( ofRandom(1.0)<0.3 || i==num_agent-1 ){
                la[i].dna.setBoundsMode( i%3 );
                la[i].dna.mutate( ofMap(mouseX, 0, ofGetWidth(), 0, 0.5)*0.4 );
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

void testApp::draw(){
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
	ofBackground( 255 );
    
    ofSetColor( 255 );
    if( bOrtho )
        ofSetupScreenOrtho();

    ofPushMatrix(); {
        ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
        for( int i=0; i<num_agent; i++ ) {
            la[i].draw();
        }
    } ofPopMatrix();

    draw_info();
}

void testApp::draw_info(){
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

void testApp::keyPressed( int key ){

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