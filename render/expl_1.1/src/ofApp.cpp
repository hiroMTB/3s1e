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

    n_points.setMode( OF_PRIMITIVE_POINTS );
    n_points.setUsage( GL_DYNAMIC_DRAW );

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

    
    // img
    img.loadImage("img/fixed_point/lg/losglaciares12.jpg");
    img2.loadImage("img/fixed_point/lg/losglaciares05.jpg");
    
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
        exps[i].setFrameRange( 1, 751 );
        exps[i].setAutoExit( true );
    }

    ofSetWindowPosition(0, 0);
    ofSetWindowShape(w/2, h/2);
    //exps[0].startExport();
}

void ofApp::update(){

    frame++;
  	frame = frame%750;
    string path_L = "sim/v3/L/pL_" + ofToString( frame,0,5,'0' )+ ".abc";
    string path_R = "sim/v3/R/pR_" + ofToString( frame,0,5,'0' )+ ".abc";

    points.clearVertices();
    points.clearColors();
    
    n_points.clearVertices();
    n_points.clearColors();
    
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
        n_points.addVertices( pos );
    }
    
    {
        ofxAlembic::Reader abc;
        abc.open(path_R);
        //abc.dumpNames();
        vector<ofVec3f> pos;
        abc.get( 0, pos );
        points.addVertices( pos );
        n_points.addVertices( pos );
    }
    
    
    /*
    vector<ofFloatColor> cs;
    cs.assign(points.getNumVertices(), ofFloatColor(0.1, 0.8) );
    points.addColors( cs );
    n_points.addColors( cs );
    */

    int w = img.getWidth();
    int h = img.getHeight();
    vector<ofFloatColor> vc;
    for (int i=0; i<points.getNumVertices(); i++) {
        int x = i%w;
        int y = i/w;
        ofFloatColor c = img.getPixelsRef().getColor(x, y);
        c.setBrightness( ofNoise(i)*0.3 + 0.1 );
        c.setHueAngle( c.getHueAngle() - 20 );
        c.a = 0.8;
        vc.push_back(c);
    }
    points.addColors( vc );
    
    // adjust
    vector<ofVec3f> & vs = points.getVertices();
    vector<ofVec3f> & n_vs = n_points.getVertices();

    for( int i=0; i<vs.size(); i++){
        vs[i] *= scale;
        vs[i].z = 0;
        vs[i].rotate( -gAngle, ofVec3f(0,0,1) );
        vs[i] += center;
        
        n_vs[i] *= scale;
        n_vs[i].z = 0;
    }

    
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
                        edge += rf_dir * ofRandomuf()*0.1;
                    }else{
                        edge = end;
                        edge -= rf_dir * ofRandomuf()*0.1;
                    }
                    
                    if( MIN(l1, l2) < limit*1.1 ){
                        for (int j=0; j<1; j++) {
                            ofVec3f r1 = ofVec2f( ofRandomf(), ofRandomf() );
                            ofVec3f r2 = ofVec2f( ofRandomf(), ofRandomf() );
                            
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
     
        for (int i=0; i<bLines.size(); i++) {

            const bLine & b = bLines[i];
            ofFloatColor c(0, 0.5);
            
            int np = points.getNumVertices()-1;
            
            for (int j=0; j<np; j++) {
                int id = j;
                ofVec3f p = points.getVertex( id );
                ofVec3f prep = ad_geom_util::vec_pl(p, b.st, b.end );
                ofVec3f onLine = p + prep;
                float len = prep.length();
                
                if( 38<len && len<42 ){
                    if( ad_geom_util::isOnline(onLine, b.st, b.end ) ){

                        for (int k=0; k<2; k++) {
                            ofVec3f r1 = ofVec2f( ofRandomf(), ofRandomf() );
                            prep_lines.addVertex( p + r1 );
                            prep_lines.addVertex( onLine + r1 );
                            prep_lines.addColor( c );
                            prep_lines.addColor( c );
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
        
        ofFloatColor c(0,0.8);
        
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
                    if ( noise < 0.2) {
                        circles.addColor( ofFloatColor(1,0.9) );
                    }else if( noise < 0.7){
                        circles.addColor( c );
                    }else{
                        circles.addColor( ofFloatColor(0,0,0,0) );
                    }
                }
            }
        }
        
    }

    
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    

    float line_y = 4.4 * scale;
    ofVec2f rf_st = ofVec3f(0,-line_y,0);
    ofVec2f rf_end = ofVec3f(0,line_y,0);
    ofVec2f rf_center = (rf_st + rf_end) * 0.5;
    
#pragma Branch
    {
        int np = n_points.getNumVertices();

        {
            if( branchIds.size() > 125 ){
                branchIds.pop_front();
            }
            int id = ofNoise( ofGetFrameNum()*0.01)*100;
            branchIds.push_back( id );
            
            list<int>::iterator itr = branchIds.begin();
            for (int i=0; itr!=branchIds.end(); i++, itr++) {
                
                int id = *itr;
                
                ofVec3f p = n_points.getVertex( np-id );
                ofVec3f onLine = rf_st + (rf_end-rf_st)*ofNoise( i, ofGetFrameNum()*0.3 );
                float len = p.distance( onLine );
                int x = ofNoise(p.x) * img2.getWidth();
                int y = ofNoise(p.y) * img2.getHeight();
                ofFloatColor c = img2.getColor(x, y);

                //if( 300<len ){
                //    ofVec3f dir = p - onLine;
                //    p = onLine + dir/2;
                //}
                
                for (int k=0; k<10; k++) {
                    int t = round( ofRandom(0,3) );
                    ofVec2f r1( ofRandomf(), ofRandomf() );
                    ofVec2f r2( ofRandomf(), ofRandomf() );
                    r1 *= 0.3;
                    r2 *= 0.3;
                    ad_graph::add_branch( branchs, onLine+r1, p+r2, c, t, ofRandomuf()>0.5 );
                }
                
                
                
                //ofVec3f prep = ad_geom_util::vec_pl(p, rf_st, rf_end );
                ofVec3f dir = p - onLine;
                float angle = (rf_end-rf_st).angle( dir );
                if( 70<angle && angle<110 ){
                    branchs.addVertex( p * 1.3 );
                    branchs.addVertex( onLine );
                    branchs.addColor( c );
                    branchs.addColor( c );
                }
                
            }
        }


#pragma FLOWER
        {
            ofFloatColor c(0, 0.9);
            int n = 1200;
            for (int i=0; i<n; i++) {
                int id = ofRandomuf() * np;
                ofVec3f p = n_points.getVertex(id);
                ofVec3f prep = ad_geom_util::vec_pl(p, rf_st, rf_end );
                ofVec3f onLine = p + prep;
                float len = prep.length();
                float limit = 70;
                bool longl = ofRandomuf()>0.91;
                if( longl ) limit *= 5;

                int bt = round( ofRandom(0, 5) );
                int ft = round( ofRandom(1, 14) );

                if( i%5 == 1){
                    ft = 12;
                }

                if( i%6 == 1){
                    ft = 13;
                }
                                
                if( 70 <len && len<limit ){
                    if( ad_geom_util::isOnline(onLine, rf_st, rf_end ) ){
                        ad_graph::add_flower( flowers, p, onLine, c, ft );
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
            glPointSize(2);
            points.draw();
        } ofPopMatrix();
        
        if( bDebugDraw )
            svg.draw();

        ofPushMatrix(); {
            {
                glLineWidth(1);
                prep_lines.setMode( OF_PRIMITIVE_LINES );
                prep_lines.draw();
                
                glPointSize(3);
                prep_lines.setMode( OF_PRIMITIVE_POINTS );
                prep_lines.draw();
            
                glLineWidth(1);
                prep_lines_b.draw();
            
                glLineWidth(1);
                circles.draw();
            }
            
            {
                ofTranslate(center);
                ofRotateZ( -gAngle );
                
                glLineWidth(1);
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
    ss << "num vert  : " << points.getNumVertices() << "\n";
    ss << "num color : " << points.getNumColors() << "\n";
    
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
