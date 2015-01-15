#include "ofApp.h"
#include "ad_util.h"
#include "ad_geom_util.h"
#include "ofxAlembic.h"
#include "ad_graph.h"

//#define A_G_SETTING 1
#define RENDER 1

void ofApp::setup(){

    ofSetVerticalSync( true );
    ofSetFrameRate(25);
    
    ofEnableAlphaBlending();

#ifdef A_G_SETTING
    ofSeedRandom(4441);
#else
    ofSeedRandom(1247);
#endif
    
    bStart = false;
    bDrawInfo = true;
    bDebugDraw = true;
    layer_num = 1;
    setup_scene();
    setup_export_layer( win.x, win.y, layer_num );
}

void ofApp::setup_scene(){
    points.setMode( OF_PRIMITIVE_POINTS );
    points.setUsage( GL_DYNAMIC_DRAW );

    lines.setMode( OF_PRIMITIVE_LINES );
    lines.setUsage( GL_DYNAMIC_DRAW );

    
    // img
    string img_name_1 = "img/photo/2.jpg";
    img.loadImage( img_name_1 );
    
    // svg
#ifdef A_G_SETTING
    svg.load("svg/expl/A_G.svg");
#else
    svg.load("svg/expl/G_M.svg");
#endif

    win.x = (int)svg.getWidth();
    win.y = (int)svg.getHeight();
    win.x -= 1;

    cout << "svg : " << win << endl;
    
    int n = svg.getNumPath();
    if( n==0 ) ofExit();
    
    for (int i=0; i<n; i++) {
        ofPath &p = svg.getPathAt(i);
        if( p.getStrokeColor() == ofColor(255,0,0) ){
            vector<ofPolyline> polys = p.getOutline();
            for (int j=0; j<polys.size(); j++) {
                const vector<ofVec3f> &vs = polys[j].getVertices();
                if( vs.size() == 2 ){
                    st = vs[0];
                    end = vs[1];
                    ofVec3f dir = end-st;
                    gAngle = dir.angle(ofVec3f(0,1,0) );
                    if(dir.y > 0)
                        gAngle = -gAngle;

                    center = (end+st) * 0.5;
                    cout << "gAngle : " << gAngle << endl;
                    cout << "Center : " << center << endl;
                }
            }
        }
    }
    
#ifndef A_G_SETTING
    gAngle = -gAngle;
#endif
    
    sim_frame = 750;
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    exporter.setup(w, h, 25, GL_RGB, 0 );
    exporter.setFilePattern(  dir_name + "/F_%05i.png");
    exporter.setFrameRange( sim_frame, 750 );
    exporter.setAutoExit( true );

#ifdef RENDER
    ofSetWindowShape(200, 200);
    ofSetWindowPosition(0, 0);
    exporter.startExport();
    bDebugDraw = false;
#else
    ofSetWindowShape(win.x/2, win.y/2);
    ofSetWindowPosition(0, 0);
    bDebugDraw = true;
#endif
    
}

void ofApp::update(){

  	sim_frame = sim_frame%751;
    string path_L = "sim/L/pL_" + ofToString( sim_frame,0,5,'0' )+ ".abc";
    string path_R = "sim/R/pR_" + ofToString( sim_frame,0,5,'0' )+ ".abc";

    points.clearVertices();
    points.clearColors();
    
    lines.clearColors();
    lines.clearVertices();
    
#pragma mark LOAD_PARTICLE
    float scale = 72;
    {
        // L load
        ofxAlembic::Reader abc;
        abc.open(path_L);
     
        vector<ofVec3f> pos;
        abc.get( 0, pos );
        for (int i=0; i<pos.size(); i++) {
            pos[i].x = 0;
            pos[i] *= scale;

#ifdef A_G_SETTING
            if( ofNoise(pos[i].z*0.1, pos[i].y*0.1) < 0.5 ) points.addVertex(pos[i]);
#else
            if( ofNoise(pos[i].z*0.1, pos[i].y*0.1) > 0.5 ) points.addVertex(pos[i]);
#endif
        }
    }

    {
        // R load
        ofxAlembic::Reader abc;
        abc.open(path_R);
        
        vector<ofVec3f> pos;
        abc.get( 0, pos );
        for (int i=0; i<pos.size(); i++) {
            pos[i].x = 0;
            pos[i] *= scale;
            
#ifdef A_G_SETTING
            if( ofNoise(pos[i].z*0.1, pos[i].y*0.1) < 0.5 ) points.addVertex(pos[i]);
#else
            if( ofNoise(pos[i].z*0.1, pos[i].y*0.1) > 0.5 ) points.addVertex(pos[i]);
#endif

        }
    }
    
    int np = points.getNumVertices();
    int w = img.getWidth();
    int h = img.getHeight();
    vector<ofFloatColor> vc;
    vc.assign(np, ofFloatColor(0));
    points.addColors( vc );
    for (int i=0; i<np; i++) {
        int x = i%w;
        int y = i/w;
        ofFloatColor c = img.getPixelsRef().getColor(x, y);
        c.a = 0.8;
        points.setColor( np-i-1, c );
    }

    return;
    
#pragma mark NEAR_LINE
    if( 1 ){
        int num_line = 10;
        int num_dupl = 5;
        int vertex_per_point = num_line * num_dupl * 2;
        
        const vector<ofVec3f> &vs = points.getVertices();
        vector<ofFloatColor> &vc = points.getColors();
        
        const ofVec3f * input = &vs[0];
        vector<ofVec3f> outv;
        outv.assign( vs.size()*vertex_per_point, ofVec3f(-99999, -99999, -99999) );
        vector<ofFloatColor> outc;
        outc.assign( vs.size()*vertex_per_point, ofFloatColor(0,0,0,0) );
        
        calcNearestPoints(input, &outv[0], &vc[0], &outc[0], vs.size(), num_line, num_dupl, sim_frame );
        
        lines.addVertices( outv );
        lines.addColors( outc );
    }

    return;
}

void ofApp::draw(){
    draw_layer_0();

#ifndef RENDER
    draw_preview();
#endif

    draw_info();
    
    sim_frame++;
}

void ofApp::draw_layer_0(){
    
    ofDisableAntiAliasing();
    ofDisableSmoothing();
    ofEnableAlphaBlending();

    exporter.begin(); {
        ofClear(0);
        ofBackground(255);

        if( bDebugDraw ) svg.draw();

        ofPushMatrix();{
            ofTranslate(center);
            ofRotate( -gAngle, 0,0,1 );
            ofRotate( 90, 0, 1, 0 );
            
#ifndef A_G_SETTING
//            ofRotate( 180, 0, 1, 0 );   //  switch L/R
//            ofRotate( 180, 1, 0, 0 );   //  switch TOP/BOTTOM
#endif

            //glLineWidth(1);
            //lines.draw();

            glPointSize(2);
            points.draw();
        } ofPopMatrix();
        
    } exporter.end();
}

void ofApp::draw_preview(){
    ofPushMatrix(); {
        ofBackground(110);
        ofSetColor(255);
        exporter.draw(0, 0);
    } ofPopMatrix();
}

void ofApp::draw_info(){
    if( !bDrawInfo ) return;
    stringstream ss;
    ss << ad_util::getFrameInfoString();
    ss << "num vert  : " << points.getNumVertices() << "\n";
    ss << "num color : " << points.getNumColors() << "\n";
    ss << "sim frame : " << sim_frame << "\n";
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

        case 'd':
            bDebugDraw = !bDebugDraw;
            break;
            
        case 'S':
            for (int i=0; i<layer_num; i++) {
                exporter.startExport();
                bDebugDraw = false;
            }
            break;
            
        default:
            break;
    }
}
