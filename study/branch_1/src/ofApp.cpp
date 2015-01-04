#include "ofApp.h"
#include "ad_util.h"

ofApp * ofApp::app = NULL;

ofApp * ofApp::init(){
    if( app == NULL )
        app = new ofApp();
}

void ofApp::setup(){

    ofSetVerticalSync( true );
    ofSetFrameRate(60);
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    layer_num = 1;
    setup_export_layer( 1920, 1080, layer_num );
    setup_scene();

    
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    for (int i=0; i<num; i++) {
        exps.push_back( ofxExportImageSequence() );
        exps[i].setup(w, h, 25, GL_RGBA, 8 );
        exps[i].setFilePattern(  dir_name + "/L" + ofToString(i) +  "/F_%05i.tif");
        exps[i].setFrameRange( 1, 3000 );
        exps[i].setAutoExit( true );
    }

    ofSetWindowPosition(0, 0);
    ofSetWindowShape(w, h);
}

void ofApp::setup_scene(){
    
    bStart = false;
    bDrawInfo = true;
    
    
    territoryFbo.allocate( 1920, 1080 );
    territoryFbo.begin();
    ofClear(0);
    territoryFbo.end();
    territoryPix.allocate( 1920, 1080, 4);
    
    Branch::root.set( 50, exps[0].getFbo().getHeight()/2 );
    
    for (int i=0; i<4; i++) {
        Branch br;
        float sp_len = ofRandom( 20, 50);
        float sp_angle = 90;
        sp_angle *= ofRandomuf()>0.5 ? 1 : -1;
        float fw_len = ofRandom( 50, 220);
        
        ofVec3f dirn = ofVec3f(1,0,0); //.rotated(sp_angle, ofVec3f(0,0,1) );
        dirn.normalize();
        br.craete(NULL, dirn, sp_len, sp_angle, fw_len, 0, 0);
        tree.push_back( br );
    }
}

void ofApp::update(){

    for (int i=0; i<tree.size(); i++) {
        tree[i].update();
    }
    
    territoryFbo.readToPixels(territoryPix);
}

void ofApp::draw(){

    ofBackground(255);

    draw_layer_0();
    draw_preview();

    if( bDebugDraw){
        ofPushMatrix();
        territoryFbo.draw(0, 0);
        ofPopMatrix();
    }

    draw_info();
}

void ofApp::draw_layer_0(){
    exps[0].begin(); {
        ofClear(0);

        for (int i=0; i<tree.size(); i++) {
            tree[i].draw();
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

    ofPushMatrix(); {
        ofSetColor(0);
        ofDrawBitmapString( ss.str(), 20, 20);
    } ofPopMatrix();
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
            
        case 'c':
            for (int i=0; i<tree.size(); i++) {
                tree[i].create_child();
            }
            break;

        case 'd':
            bDebugDraw = !bDebugDraw;
            break;
            
        default:
            break;
    }
}
