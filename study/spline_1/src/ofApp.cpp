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
	setup_spline();
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

void ofApp::setup_spline(){

	int n = 1000;
	
	for( int i=0; i<n; i++ ){
		vector<ofVec3f> vs;
		ofVec3f v( 0,0,0 );
		vs.push_back( v );

		v.x = 100 + ofRandom(0, 100);
		vs.push_back( v );

		for (int j=2; j<6; j++) {
			ofVec3f v( vs[j-1].x + ofRandom(100,200) + j*10, 0 );
			vs.push_back( v );
		}
		
		points.push_back(vs);
		
		sps.push_back( ofxSimpleSpline() );
		sps[i].setSubdivisions(10);
		sps[i].setControlVertices( points[i] );
	}
}

void ofApp::update(){
	
	for (int i=0; i<points.size(); i++) {

		float r = ofNoise( i, (float)i*0.01 );
		float freq = ofNoise( i*2, (float)i*0.02 )*0.1;
		r *= 120;
		r += 30;
		
		for (int j=1; j<points[i].size(); j++) {
			float rr = r + j;
			points[i][j].y = sin(ofGetFrameNum()*freq + j) * rr;
		}
	}
	
	for (int i=0; i<sps.size(); i++) {
		sps[i].setControlVertices( points[i] );
		sps[i].update();
	}

}

void ofApp::draw(){

	ofEnableAlphaBlending();
	
    ofBackground(255);
    draw_layer_0();
    draw_preview();
    draw_info();
}

void ofApp::draw_layer_0(){
    exps[0].begin(); {
        ofClear(0);
		ofBackground(0);

        ofTranslate( 100, win.y/2 );
		ofRotateZ( -30.0 );

		ofNoFill();
		ofSetColor(220, 30);

		for (int i=0; i<sps.size(); i++) {
			ofRotateZ( 60.0/sps.size() );
			sps[i].draw();
		}
		
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

