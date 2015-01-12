#include "ofApp.h"
#include "ad_util.h"
#include "ad_geom_util.h"
#include "ofxAlembic.h"
#include "ad_graph.h"

void ofApp::setup(){

    ofSetVerticalSync( true );
    ofSetFrameRate(25);
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    bStart = false;
    bDrawInfo = true;
    bDebugDraw = true;
    frame = 0;
    layer_num = 1;
    setup_scene();
    setup_export_layer( win.x, win.y, layer_num );
    
    
    int res = 24;
    float adder = TWO_PI/res;
    for( int i=0; i<res; i++ ){
        int id = i;
        float x = cos(id * adder);
        float y = sin(id * adder);
        circle.addVertex( ofVec2f(x, y) );

        id++;
        x = cos(id * adder);
        y = sin(id * adder);
        circle.addVertex( ofVec2f(x, y) );
    }

    circles.setMode( OF_PRIMITIVE_LINES );
    circles.setUsage( GL_DYNAMIC_DRAW );
}

void ofApp::setup_scene(){
    points.setMode( OF_PRIMITIVE_POINTS );
    points.setUsage( GL_DYNAMIC_DRAW );

    lines.setMode( OF_PRIMITIVE_LINES );
    lines.setUsage( GL_DYNAMIC_DRAW );

    prep_lines.setMode( OF_PRIMITIVE_LINES );
    prep_lines.setUsage( GL_DYNAMIC_DRAW );

    prep_lines_b.setMode( OF_PRIMITIVE_LINES );
    prep_lines_b.setUsage( GL_DYNAMIC_DRAW );

    branchs.setMode( OF_PRIMITIVE_LINES );
    branchs.setUsage( GL_DYNAMIC_DRAW );

    flowers.setMode( OF_PRIMITIVE_LINES );
    flowers.setUsage( GL_DYNAMIC_DRAW );

    // svg
    svg.load("svg/expl/A_G.svg");
    win.x = svg.getWidth();
    win.y = svg.getHeight();
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
        }else if( p.getStrokeColor() == ofColor(0,0,255) ){
            vector<ofPolyline> polys = p.getOutline();
            for (int j=0; j<polys.size(); j++) {
                const vector<ofVec3f> &vs = polys[j].getVertices();
                if( vs.size() == 2 ){
                    bLines.push_back( bLine() );
                    bLines[bLines.size()-1].st = vs[0];
                    bLines[bLines.size()-1].end = vs[1];
                    cout << "Found bLine : " << bLines.size() << endl;
                }
            }
        }
    }
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
    ofSetWindowShape(w/2, h/2);
    //exps[i].startExport();
}

void ofApp::update(){

    frame++;
  	frame = frame%750;
    string path_L = "sim/v3/L/pL_" + ofToString( frame,0,5,'0' )+ ".abc";
    string path_R = "sim/v3/R/pR_" + ofToString( frame,0,5,'0' )+ ".abc";

    points.clearVertices();
    points.clearColors();
    lines.clearColors();
    lines.clearVertices();

    prep_lines.clearColors();
    prep_lines.clearVertices();
    
    branchs.clearColors();
    branchs.clearVertices();
    
    flowers.clearVertices();
    flowers.clearColors();
    
    circles.clearVertices();
    circles.clearColors();
    
#pragma mark LOAD_PARTICLE
    
    float scale = 105;
    
    {
        ofxAlembic::Reader abc;
        abc.open(path_L);
        //abc.dumpNames();
    
        vector<ofVec3f> pos;
        abc.get( 0, pos );
        points.addVertices( pos );
    }
    
    {
        ofxAlembic::Reader abc;
        abc.open(path_R);
        //abc.dumpNames();
        vector<ofVec3f> pos;
        abc.get( 0, pos );
        points.addVertices( pos );
    }
    
    // adjust
    vector<ofVec3f> & vs = points.getVertices();
    for( int i=0; i<vs.size(); i++){
        vs[i] *= scale;
        vs[i].z = 0;
        vs[i].rotate( -gAngle, ofVec3f(0,0,1) );
        vs[i] += center;
    }

#pragma mark DUPLICATE
    int n = 0; //points.getNumVertices();
    for( int i=0; i<n; i++){
        for (int j=0; j<1; j++) {
            ofVec2f r( ofRandomf(), ofRandomf() );
            r *= 1.2;
            points.addVertex( points.getVertex(i) + r );
        }
    }
    vector<ofFloatColor> cs;
    cs.assign(points.getNumVertices(), ofFloatColor(0.1, 0.8) );
    points.addColors( cs );
    
#pragma PREP_LINE
    {
        int n = 1000;
        int np = points.getNumVertices()-1;
        ofFloatColor c(0,0.5);
        
        for (int i=0; i<n; i++) {
            
            int id = ofRandomuf() * np;
            ofVec3f p = points.getVertex(id);
            ofVec3f prep = ad_geom_util::vec_pl(p, st, end );
            ofVec3f onLine = p + prep;
            float len = prep.length();

            float limit = 80;
            bool longl = ofRandomuf()>0.8;
            if( longl ) limit *= 4;
            if( 1 <=len && len<=limit){
                if( ad_geom_util::isOnline(onLine, st, end ) ){

                    for (int j=0; j<10; j++) {
                        ofVec3f r1 = ofVec2f( ofRandomf(), ofRandomf() );
                        ofVec3f r2 = ofVec2f( ofRandomf(), ofRandomf() );
                        r1 *= 1.0;
                        r2 *= 1.0;

                        prep_lines.addVertex( p + r1 );
                        prep_lines.addVertex( onLine + r2 );
                        prep_lines.addColor( c );
                        prep_lines.addColor( c );
                    }
                }else{
                    
                    float l1 = p.distance(st);
                    float l2 = p.distance(end);
                    ofVec2f rf_dir = end - st;
                    ofVec2f edge;
                    if( l1<l2 ){
                        edge = st;
                        edge += rf_dir * ofRandomuf()*0.2;
                    }else{
                        edge = end;
                        edge -= rf_dir * ofRandomuf()*0.2;
                    }
                    
                    if( MIN(l1, l2) < limit*1.2 ){
                        for (int j=0; j<10; j++) {
                            ofVec3f r1 = ofVec2f( ofRandomf(), ofRandomf() );
                            ofVec3f r2 = ofVec2f( ofRandomf(), ofRandomf() );
                            r1 *= 1.0;
                            r2 *= 1.0;
                            
                            prep_lines.addVertex( p +r1 );
                            prep_lines.addVertex( edge + r2 );
                            prep_lines.addColor( c );
                            prep_lines.addColor( c );
                        }
                    }
                }
            }
        }
    }
#pragma mark PREP_LINE_B
    
    {
        
        
        
        
    }
    
#pragma Branch
    {
        int np = points.getNumVertices()-1;

        {
            int n = 12;
            ofFloatColor c(0,0.3);
            for (int i=0; i<n; i++) {
                ofVec3f onLine = st + (end-st)*ofNoise( i, ofGetFrameNum()*0.3 );
                for (int j=0; j<6; j++) {
                    int id = np - ofNoise(i, j, ofGetFrameNum()*0.001)*700;
                    if( id < 0)
                        continue;
                    
                    ofVec3f p = points.getVertex(id);
                    float len = p.distance(onLine);
                    if( 50<len && len<1200){
                        int t = round( ofRandom(0,5) );
                        for (int k=0; k<5; k++) {
                            ofVec2f r1( ofRandomf(), ofRandomf() );
                            ofVec2f r2( ofRandomf(), ofRandomf() );
                            r1 *= 1.1;
                            r2 *= 1.1;
                            ad_graph::add_branch( branchs, onLine+r1, p+r2, c, t, ofRandomuf()>0.5 );
                        }
                    }
                }
            }
        }

#pragma FLOWER
        {
            ofFloatColor c(0,0.9);
            int n = 800;
            for (int i=0; i<n; i++) {
                int id = ofRandomuf() * np;
                ofVec3f p = points.getVertex(id);
                ofVec3f prep = ad_geom_util::vec_pl(p, st, end );
                ofVec3f onLine = p + prep;
                float len = prep.length();
                float limit = 100;
                bool longl = ofRandomuf()>0.92;
                if( longl ) limit *= 3;

                int bt = ofRandom(0, 4);
                int ft = ofRandom(0, 15);

                if( i%4 == 1){
                    ft = 12;
                }

                if( i%5 == 1){
                    ft = 13;
                }

                if( 5 <len && len<limit ){
                    if( ad_geom_util::isOnline(onLine, st, end ) ){
                
                        for (int j=0; j<2; j++) {
                            ofVec2f r1( ofRandomf(), ofRandomf() );
                            ofVec2f r2( ofRandomf(), ofRandomf() );
                            r1 *= 1.0;
                            r2 *= 1.0;

                            if( i%2 == 0){
                                ad_graph::add_flower( flowers, p+r1, onLine+r2, c, ft );
                                ad_graph::add_branch( branchs, p+r1, onLine+r2, c, bt );
                            }else{
                                ad_graph::add_flower( flowers, onLine+r1, p+r2, c, ft );
                                ad_graph::add_branch( branchs, onLine+r1, p+r2, c, bt );
                            }
                        }
                    }
                }
            }
        }
#pragma CIRCLES
        {
            int np = points.getNumVertices();
            int n = (float)np*0.01;
            
            ofFloatColor c(0.1,0.3);

            const vector<ofVec3f> &cv = circle.getVertices();

            for (int i=0; i<n; i++) {
                int id = ofNoise(i, ofGetFrameNum()*0.3 ) * np;
                
                ofVec3f p = points.getVertex(id);
                if( center.distance(p) > 400)
                    continue;
                
                float rad = ofSignedNoise(i, p.x*0.1, p.y*0.1)*100 + 10;

                for (int j=0; j<100; j++) {
                    ofVec2f r( ofSignedNoise(p.x, j*1.3), ofSignedNoise(p.y, j*1.3) );
                    r *= ofSignedNoise(i*j*p.x*p.y);
                    
                    for (int k=0; k<cv.size(); k++) {
                        ofVec2f v = p + cv[k]*rad + r;
                        circles.addVertex( v );
                        float noise = ofNoise(v.x,v.y,k*2);
                        if ( noise < 0.3) {
                            circles.addColor( ofFloatColor(1,0.8) );
                        }else if( noise < 0.7){
                            circles.addColor( c );
                        }else{
                            circles.addColor( ofFloatColor(0,0,0,0) );
                        }
                    }
                }
            }
            
        }
    }
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
        ofBackground(255);

        ofPushMatrix(); {
            //ofTranslate(center);
            //ofRotateZ( -gAngle );
            
            glPointSize(1);
            points.draw();
            
        } ofPopMatrix();
        
        if( bDebugDraw )
            svg.draw();

        ofPushMatrix(); {
            //ofTranslate(center);
            //ofRotateZ( -gAngle );
            {
                glLineWidth(1);
                prep_lines.setMode( OF_PRIMITIVE_LINES );
                prep_lines.draw();
                
                glPointSize(3);
                prep_lines.setMode( OF_PRIMITIVE_POINTS );
                prep_lines.draw();

            
                glLineWidth(1);
                prep_lines_b.draw();
            }
            
            {
                glLineWidth(1);
                circles.draw();
                branchs.draw();
                flowers.draw();
            }
        } ofPopMatrix();
        
    } exps[0].end();
}

void ofApp::draw_preview(){

    ofPushMatrix(); {
        ofBackground(110);
        ofSetColor(255);
        exps[0].draw(0, 0);
    } ofPopMatrix();
}

void ofApp::draw_info(){
    if( !bDrawInfo ) return;
    stringstream ss;
    ss << ad_util::getFrameInfoString();
    ss << "num p : " << points.getNumVertices() << "\n";
    
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
                exps[i].startExport();
            }
            break;
            
        default:
            break;
    }
}
