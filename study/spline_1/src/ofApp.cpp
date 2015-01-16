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
    setup_export_layer( 1920, 1080, layer_num );
	
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

    ofSetWindowPosition(0, 0);
    ofSetWindowShape(w/2, h/2);
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
		vg.beginShape();
		vg.setLineWidth(1);
		ofNoFill();
		
		ofPoint p1( ofNoise( ),100);
		ofPoint p2(300,200);
		ofPoint p3(600,800);
		ofPoint p4(900,100);

		//vg.curve( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y );
		vg.bezier( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y );
		
		//ofBezier(, <#float y0#>, <#float x1#>, <#float y1#>, <#float x2#>, <#float y2#>, <#float x3#>, <#float y3#>);
    
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

