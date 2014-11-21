#include "ofApp.h"

void ofApp::setup(){

    ofSetWindowPosition( 1560, 0 );
    code = gui.create( "my_gui.maxpat" );
}

void ofApp::update(){

}

void ofApp::draw(){
    ofBackground( 0 );
    ofSetColor( 0, 255, 0 );
    ofDrawBitmapString( code, 20, 20 );
}

void ofApp::keyPressed( int key ){
    switch( key ){
        case 'o':
            gui.open();
            break;
    }
}
