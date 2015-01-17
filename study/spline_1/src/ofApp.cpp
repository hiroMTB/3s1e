#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    ofSetVerticalSync( true );
    ofSetFrameRate(60);
    
    ofEnableAlphaBlending();
    
    bStart = false;
    bDrawInfo = true;
    layer_num = 3;
    sel_layer = 0;
 
    win.x = 1920;
    win.y = 1080;
    setup_export_layer( win.x, win.y, layer_num );
	
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    for (int i=0; i<num; i++) {
        exps.push_back( ofxExportImageSequence() );
        exps[i].setup(w, h, 25, GL_RGB, 0 );
        exps[i].setFilePattern(  dir_name + "/L" + ofToString(i) +  "/F_%05i.png");
        exps[i].setFrameRange( 1, 3000 );
        exps[i].setAutoExit( true );
    }

    ofSetWindowShape(w, h);
    ofSetWindowPosition(0, 0);
}

void ofApp::update(){


}

void ofApp::draw(){

    ofBackground(255);
    draw_layer_0();
    draw_preview();
    draw_info();
}

void ofApp::draw_layer_0(){
    exps[0].begin(); {
        ofClear(0);
		ofBackground(0);
        ofSetColor(255);

        ofTranslate( win.x/2, win.y/2 );
		vg.beginShape();
		vg.setLineWidth(1);
		ofNoFill();
		
        float frame = ofGetFrameNum()/2;
		ofPoint p1( -1, 0);
        //ofPoint p2( ofNoise(3, frame*0.01), ofSignedNoise(4, frame*0.01));
        //ofPoint p3( ofNoise(5, frame*0.01), ofSignedNoise(6, frame*0.01));
        ofPoint p2( sin(frame*0.1+0)/2+0.5, sin(frame*0.1-1.7) );
        ofPoint p3( sin(frame*0.1+3.14)/2+0.5, -sin(frame*0.1+1.7) );
        //ofPoint p2( 1, sin(frame*0.1-1.7) );
        //ofPoint p3( 1, sin(frame*0.1+1.7) );

        ofPoint p4( 1, 0);

        p1 *= 500;
        p4 *= 500;
        
        p2 *= 200;
        p3 *= -200;
        
        p2 += p1;
        p3 += p4;
        
        if( 1 ){
            ofSetColor(0,0,255,150);
            ofCircle(p1, 4);
            ofCircle(p2, 4);
            ofCircle(p3, 4);
            ofCircle(p4, 4);
            
            ofLine(p1, p2);
            ofLine(p3, p4);
        }

        ofSetColor(250, 250);
        //vg.curve( p2.x, p2.y, p1.x, p1.y, p4.x, p4.y, p3.x, p3.y );
		vg.bezier( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y );
		
    } exps[0].end();
}

void ofApp::draw_preview(){
    
    ofPushMatrix(); {
        ofBackground(255);
        ofSetColor(255);
        exps[0].draw(0, 0);
    } ofPopMatrix();
}

void ofApp::draw_info(){
    if( !bDrawInfo ) return;
    stringstream ss;
    ss << ad_util::getFrameInfoString();

    ofSetColor(0);
    ofDrawBitmapString( ss.str(), 20, 20);
}

void ofApp::keyPressed(int key){

    switch (key) {
        case 'I':
            bDrawInfo = !bDrawInfo;
            break;

        case ' ':
            bStart = !bStart;
            break;
            
        case 'S':
            for (int i=0; i<layer_num; i++) {
                exps[i].startExport();
            }
            break;

        case '0':
            sel_layer = 0;
            break;

        case '1':
            sel_layer = 1;
            break;

        case '2':
            sel_layer = 2;
            break;
            
        default:
            break;
    }
}

