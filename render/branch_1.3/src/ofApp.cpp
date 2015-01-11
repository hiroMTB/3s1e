#include "ofApp.h"
#include "ad_util.h"

ofApp * ofApp::app = NULL;

ofApp * ofApp::init(){
    if( app == NULL )
        app = new ofApp();
}

void ofApp::setup(){
    ofSetLogLevel( OF_LOG_VERBOSE );
    ofSetVerticalSync( true );
    ofSetFrameRate(60);
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    setup_scene();
    layer_num = 1;
    setup_export_layer( win.x, win.y, layer_num );
    start_time = ofGetSystemTime();
    ofSeedRandom(start_time);
}

void ofApp::setup_scene(){
    
    bStart = false;
    bDrawInfo = true;
    max_frame = 1500;
    
    {
        /*
         *      SVG load
         */
        string path = "";
        ofFileDialogResult openFileResult= ofSystemLoadDialog("Select a svg file");
        if (openFileResult.bSuccess){
            ofLogVerbose("getPath(): "  + openFileResult.getPath());
            ofFile file (openFileResult.getPath());
            
            if (file.exists()){
                string fileExtension = ofToUpper(file.getExtension());
                if (fileExtension == "svg" || fileExtension == "SVG" )
                    path = openFileResult.getPath();
            }else{
                ofLogVerbose("file not exist");
                ofExit();
            }
        }else {
            ofLogVerbose("User hit cancel");
            ofExit();
        }

        svg.load( path );
        win.x = svg.getWidth();
        win.y = svg.getHeight();
        cout << "svg : " << win << endl;
        
        int n = svg.getNumPath();
        if( n==0 ) ofExit();
        
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
                        
                        max_dist = dir.length();
                        
                        cout << "gAngle : " << gAngle << endl;
                        cout << "start : " << start << endl;
                        cout << "max_dist : " << max_dist << endl;
                    }
                }
            }
        }
    }
    
    /*
     *      IMG load
     */
    if( img.loadImage("img/fixed_point/lg/losglaciares05.jpg") )
        cout << "Image load OK" << endl;
    else
        cout << "Can not load image" << endl;
        
    Branch::root.set( 0,0);
    
    int num_first_branch = 20;
    for (int i=0; i<num_first_branch; i++) {
        ofVec3f dirn = ofVec3f(1,0,0);
        Branch br;
        br.craete( NULL, dirn );
        br.bMainBranch = true;
        tree.push_back( br );
    }
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("rnd/L_%m%d_%H%M_%S");
    
    for (int i=0; i<num; i++) {
        exps.push_back( ofxExportImageSequence() );
        exps[i].setup(w, h, 25, GL_RGBA, 8 );
        exps[i].setFilePattern(  dir_name + "/F_%05i.tif");
        exps[i].setFrameRange( 1, max_frame+1 );
        exps[i].setAutoExit( true );
    }

    ofSetWindowShape(w/4, h/4);
    ofSetWindowPosition(0, 0);
    exps[0].startExport();
}

void ofApp::update(){

    Branch::active_total = 0;
    Branch::nonp.clear();
    Branch::dist_limit = max_dist * (float)ofGetFrameNum()/max_frame;
    
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

//    if( ofRandomuf() > 0.95 ){
//        ofVec3f dirn = ofVec3f(1,0,0);
//        Branch br;
//        tree.push_back( br );
//        tree[tree.size()-1].craete( NULL, dirn );
//    }
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
        ofSetColor(255);
        //ofBackground(255);
        
        if( bDebugDraw )
            svg.draw();
        
        ofTranslate( start );
        ofRotateZ( -gAngle );


        for (int i=0; i<tree.size(); i++) {
            tree[i].draw();
        }

        if( bDebugDraw ){
            ofSetLineWidth(5);
            ofSetColor(5, 5, 255);
            ofLine( Branch::root, Branch::root + tree[0].dirn * Branch::dist_limit );
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

    ss << "Branch Num          : " << Branch::total_bnum << "\n";
    ss << "Branch Top Depth    : " << Branch::top_depth << "\n";
    ss << "Branch active total : " << Branch::active_total << "\n";
    ss << "Branch nonp         : " << Branch::nonp.size() << "\n";
    
    ofPushMatrix(); {
        ofSetColor(0);
        ofDrawBitmapString( ss.str(), 20, 20);
    } ofPopMatrix();
    
    
//    for (int i=0; i<Branch::nonp.size(); i++) {
//        cout << Branch::nonp[i]->st << endl;
//    }
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
