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
    
    ofEnableAlphaBlending();
    

    svg.load( "svg_r/v3/C_r.svg");
    win.x = svg.getWidth();
    win.y = svg.getHeight();
    cout << "svg : " << win << endl;
    
    int n = svg.getNumPath();
    for (int i=0; i<n; i++) {
        ofPath &p = svg.getPathAt(i);
        if( p.getStrokeColor().r == 255){
            vector<ofPolyline> polys = p.getOutline();
            for (int j=0; j<polys.size(); j++) {
                const vector<ofVec3f> &vs = polys[j].getVertices();
                if( vs.size() == 2 ){
                    ofVec3f st = vs[0];
                    ofVec3f end = vs[1];
                    ofVec3f dir = end-st;
                    gAngle = dir.angle(ofVec3f(1,0,0) );
                    if(dir.y > 0)
                        gAngle = -gAngle;
                    start = st;
                    
                    cout << "gAngle : " << gAngle << endl;
                    cout << "start : " << start << endl;
                }
            }
        }
    }
    
    layer_num = 1;
    setup_export_layer( win.x, win.y, layer_num );
    setup_scene();
    
    start_time = ofGetSystemTime();
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    for (int i=0; i<num; i++) {
        exps.push_back( ofxExportImageSequence() );
        exps[i].setup(w, h, 25, GL_RGB, 0 );
        exps[i].setFilePattern(  dir_name + "/L" + ofToString(i) +  "/F_%05i.png");
        exps[i].setFrameRange( 1, 1501 );
        exps[i].setAutoExit( true );
    }

    ofSetWindowPosition(0, 0);
    ofSetWindowShape(w/2, h/2);
    exps[0].startExport();
}

void ofApp::setup_scene(){
    
    bStart = false;
    bDrawInfo = true;
    
    if( img.loadImage("img/fixed_point/lg/losglaciares04.jpg") )
        cout << "Image load OK" << endl;
    else
        cout << "Can not load image" << endl;
    
    territoryFbo.allocate( win.x, win.y );
    territoryFbo.begin();
    ofClear(0);
    territoryFbo.end();
    territoryPix.allocate( win.x, win.y, 4);
    
//    Branch::root.set( 50, exps[0].getFbo().getHeight()/2 );
    Branch::root.set( 0,0);
    
    int num_first_branch = 4;
    for (int i=0; i<num_first_branch; i++) {
        ofVec3f dirn = ofVec3f(1,0,0);
        Branch br;
        br.craete( NULL, dirn );
        br.bMainBranch = true;
        tree.push_back( br );
    }
}

void ofApp::update(){

    Branch::active_total = 0;
    
    for (int i=0; i<tree.size(); i++) {
        tree[i].update();
    }
        
    for (int i=0; i<tree.size(); i++) {
        tree[i].revive();
    }
    //territoryFbo.readToPixels(territoryPix);
    
    for (int i=0; i<tree.size(); i++) {
        tree[i].create_child();
    }
}

void ofApp::draw(){

    ofBackground(255);
    
    draw_layer_0();
    draw_preview();

//    if( bDebugDraw){
//        ofPushMatrix();
//        territoryFbo.draw(0, 0);
//        ofPopMatrix();
//    }

    draw_info();
}

void ofApp::draw_layer_0(){
    exps[0].begin(); {
        ofClear(0);
        //ofBackground(255);
        
        if( bDebugDraw )
            svg.draw();
        
        ofTranslate( start );
        ofRotateZ( -gAngle );

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

    ss << "Branch Num       : " << Branch::total_bnum << "\n";
    ss << "Branch Top Depth : " << Branch::top_depth << "\n";
    ss << "Branch active total : " << Branch::active_total << "\n";
    
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
