#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    ofSetVerticalSync( true );
    ofSetFrameRate(60);
    
    ofEnableAlphaBlending();
    
    bStart = false;
    bDrawInfo = true;
    layer_num = 1;

    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
    
    mesh.setUsage( GL_DYNAMIC_DRAW );
    mesh.setMode( OF_PRIMITIVE_POINTS );

    setup_export_layer( 2560, 1000, layer_num );
    
    load_model( "3d/hornet.ply");
    
    mesh.setUsage( GL_DYNAMIC_DRAW );
    mesh.setMode( OF_PRIMITIVE_POINTS );

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

    win.x = exps[0].getFbo().getWidth();
    win.y = exps[0].getFbo().getHeight();
    
    ofSetWindowShape(w, h);
    ofSetWindowPosition(0, 0);
}

void ofApp::load_model( string path ){
    bool optimize = false;
    model.loadModel( path, optimize );
    
    if( !model.hasMeshes() ){
        cout << "No Mesh data in " + path << endl;
        ofExit();
    }else{
        cout << "Load & found Model" <<endl;
    }

    int nMesh = model.getMeshCount();
    for (int i=0; i<nMesh; i++) {
        ofMesh m = model.getMesh(i);
        mesh = m;
        cout << "Mesh[ " << i << "] : " << mesh.getNumVertices() << " vetices"<< endl;
    }
    
    mesh.setUsage( GL_DYNAMIC_DRAW );
    mesh.setMode( OF_PRIMITIVE_POINTS );
    
    int nV = mesh.getNumVertices();
    vector<ofFloatColor> vc;
    vc.assign(nV, ofFloatColor(0,0,0) );
    mesh.addColors( vc );
    
    // Orbital
    scale = 20;
    vector<ofVec3f> &vs = mesh.getVertices();
    for (int i=0; i<vs.size(); i++) {
        vs[i] *= scale;
        aabb_max.x = MAX( aabb_max.x, vs[i].x );
        aabb_max.y = MAX( aabb_max.y, vs[i].y );
        aabb_max.z = MAX( aabb_max.z, vs[i].z );

        aabb_min.x = MIN( aabb_min.x, vs[i].x );
        aabb_min.y = MIN( aabb_min.y, vs[i].y );
        aabb_min.z = MIN( aabb_min.z, vs[i].z );
    }

    cout << "aaabb Max " << aabb_max << endl;
    cout << "aaabb Min " << aabb_min << endl;

    aabb_center = (aabb_max+aabb_min)*0.5;
    
}

void ofApp::update(){

    const vector<ofVec3f> &vs = mesh.getVertices();
    
    points.clearVertices();
    points.clearColors();
    
    int frame = ofGetFrameNum();
    for (int i=0; i<vs.size()/50; i++) {
        
        int id = ofNoise( frame, i*100 ) * vs.size() + frame;
        id %= vs.size()-1;

        ofVec3f dir = vs[id] - aabb_center;
        float dist = 700.0 - dir.length() + sin(frame*0.1+i*0.001)*250;
        float x = i;
        x *= 0.1;
        x = fmodf( x, 1000);
        
        float angle = fmodf(x, 60);
        
        ofVec3f p(0, dist + 10,0);
        p.rotate(angle, ofVec3f(0,0,1) );

        p.x += ofSignedNoise(angle, id*0.001) * 10;
        p.y += ofSignedNoise(id, id*0.001) * 10;

        points.addVertex( p );
        points.addColor( ofFloatColor(0) );
    }
    
    points.addVertex( ofVec3f(0,0,0) );
    points.addColor( ofFloatColor(0) );
    
    draw_near_line();
}

void ofApp::draw(){

    ofBackground(255);
    draw_layer_0();
    draw_preview();
    draw_info();
}

void ofApp::draw_layer_0(){
    exps[0].begin(cam); {
        ofClear(0);
        ofBackground(0);
        ofSetColor(255);

        ofTranslate(0, -400);

        ad_util::draw_axis();
        
        glPointSize(1);
        glLineWidth(1);
        
        //mesh.draw();
        lines.draw();
//        points.draw();
        
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
            
        default:
            break;
    }
}

void ofApp::draw_near_line(){
#pragma mark NEAR_LINE
    if( 1 ){

        lines.clearVertices();
        lines.clearColors();
        
        int num_line = 10;
        int num_dupl = 10;
        int vertex_per_point = num_line * num_dupl * 2;
        
        const vector<ofVec3f> &vs = points.getVertices();
        vector<ofFloatColor> &vc = points.getColors();
        
        const ofVec3f * input = &vs[0];
        vector<ofVec3f> outv;
        outv.assign( vs.size()*vertex_per_point, ofVec3f(-99999, -99999, -99999) );
        vector<ofFloatColor> outc;
        outc.assign( vs.size()*vertex_per_point, ofFloatColor(0,0,0,0) );
        
        calcNearestPoints(input, &outv[0], &vc[0], &outc[0], vs.size(), num_line, num_dupl );
        
        lines.addVertices( outv );
        lines.addColors( outc );
    }
}
