#include "testApp.h"

testApp::testApp(){
	ofSetWindowPosition( 0, 0 );
    bDraw_info = true;
    bStart = false;
    bAdd_agent = true;
}

void testApp::setup(){

    num_agent = 500;
    ofVec3f center( 0, 0, 0 );

    for ( int i=0; i<num_agent; i++ ) {
        LineAgent l;
        l.setup( center, 600 );
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
                la[i].dna.mate( la[i+1].dna,  0.1*0.4 );
            }
            
            la[i].update_agent();
            
            if( bAdd_agent )
                la[i].add_result();
            
            la[i].animate();
            la[i].animate_noise( i );
        }
    }
}

void testApp::draw(){
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
	ofBackground( 255 );
    
    ofSetColor( 255 );

    ofPushMatrix(); {
        ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
        for( int i=0; i<num_agent; i++ ) {
            la[i].draw();
        }
    } ofPopMatrix();

    saver.save();
    
    draw_info();
}

void testApp::draw_info(){
    if( !bDraw_info) return;
    ofSetColor( 200 );
    ofRect( 5, 10, 400, 200 );
    int y = 20;
    ofSetColor( 0 );
    
    ofSetColor( 0 );
    stringstream ss;
    ss << "fps       : " << (int)ofGetFrameRate() << "\n";
    ss << "cur frame : " << saver.frame_cur << "\n";
    ss << "end frame : " << saver.frame_end << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
    
    ss << "space key : start\n";
    ss << "f     key : render resolution\n";
    ss << "I     key : draw info\n";
    ss << "S     key : save image\n";
    ofDrawBitmapString( ss.str(), 20, 20 );
}

void testApp::keyPressed( int key ){

    switch( key ) {
		case ' ':
            bStart = !bStart;
            break;

        case 'a':
            bAdd_agent = !bAdd_agent;
            break;
            
        case 'f':
            ofSetWindowShape( 1920*2, 1080*2 );
            break;

        case 'I':
            bDraw_info = !bDraw_info;
            break;
            
        case 'S':
            bStart = true;
            saver.start( ofGetTimestampString(), "", 3000 );
            break;

        default:
            break;
	}
}
