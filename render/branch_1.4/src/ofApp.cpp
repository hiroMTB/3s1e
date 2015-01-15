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
    
    ofEnableAlphaBlending();
    
    setup_scene();
    layer_num = 1;
    setup_export_layer( win.x, win.y, layer_num );
    ofSeedRandom(1234);
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
        win.x = round( svg.getWidth()) ;
        win.y = round( svg.getHeight() );
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
            }else if( 1 ){
                angle_between = 0;
                cout << "angle_between : " << angle_between << endl;
            }
        }
    }
    
    /*
     *      IMG load
     */
    //string path = "img/fixed_point/lg/losglaciares05.jpg";
    string path = "img/photo/1.jpg";
    if( img.loadImage( path ) )
        cout << "Image load OK" << endl;
    else
        cout << "Can not load image" << endl;
        
        Branch::root.set( 0,0);
    
    num_first_branch = 5;
    for (int i=0; i<num_first_branch; i++) {
        ofVec3f dirn = ofVec3f(1,0,0);
        tree.push_back( Branch() );
        tree[tree.size()-1].craete( NULL, dirn );
        tree[tree.size()-1].bMainBranch = true;
    }
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("rnd/L_%m%d_%H%M_%S");
    
    for (int i=0; i<num; i++) {
        exps.push_back( ofxExportImageSequence() );
        exps[i].setup(w, h, 25, GL_RGB, 4 );
        exps[i].setFilePattern(  dir_name + "/F_%05i.png");
        exps[i].setFrameRange( 1, max_frame+1 );
        exps[i].setAutoExit( true );
    }

    ofSetWindowShape(w, h);
    ofSetWindowPosition(0, 0);
    //exps[0].startExport();
}

void ofApp::update(){

    for (int i=0; i<tree.size(); i++) {
        tree[i].update();
        tree[i].revive();
        tree[i].create_child();
    }

    Branch::update_s();
    
    //
    //
    for (int i=0; i<tree2.size(); i++) {
        tree2[i].update();
        tree2[i].revive();
        tree2[i].create_child();
    }
    
    if( ofGetFrameNum() == 200){
        for (int i=0; i<6; i++) {
            ofVec3f dirn = ofVec3f(1,0,0);
            Branch br;
            tree2.push_back( br );
            tree2[tree2.size()-1].craete( NULL, dirn );
            tree2[tree2.size()-1].bMainBranch = true;
        }
    }
}

void ofApp::draw(){

    ofBackground(255);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
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
        ofBackground(255);
        
        if( bDebugDraw )
            svg.draw();
        
        ofTranslate( start );
        ofRotateZ( -gAngle );

        angle_between = 60;
        float stepAngle = angle_between / num_first_branch;
        for (int i=0; i<tree.size(); i++) {

            int add = i%num_first_branch;
            float angle = stepAngle * add - angle_between/2;
            ofPushMatrix(); {
                ofRotate(angle, 0, 0, 1);
                tree[i].draw();
            } ofPopMatrix();
        }

        ofPushMatrix(); {
            ofTranslate(200, 0);
            ofRotate(-10, 0, 0, 1);
            for (int i=0; i<tree2.size(); i++) {
                tree2[i].draw();
            }
        }ofPopMatrix();

        
        if( bDebugDraw ){
            ofSetLineWidth(5);
            ofSetColor(5, 5, 255);
            ofLine( Branch::root, Branch::root + tree[0].dirn * Branch::dist_limit );
        }

    } exps[0].end();
}

void ofApp::draw_preview(){
    
    ofPushMatrix(); {
        ofClear(0);
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
    ss << "Branch nonp         : " << Branch::allb.size() << "\n";
    
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
