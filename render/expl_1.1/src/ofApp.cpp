#include "ofApp.h"
#include "ad_util.h"
#include "ad_geom_util.h"
#include "ofxAlembic.h"
#include "ad_graph.h"

void ofApp::setup(){

    ofSetVerticalSync( true );
    ofSetFrameRate(25);
    
    ofEnableAlphaBlending();
    
    
    bStart = false;
    bDrawInfo = true;
    bDebugDraw = true;
    layer_num = 1;
    setup_scene();
    setup_export_layer( win.x, win.y, layer_num );
    
//    int res = 24;
//    float adder = TWO_PI/res;
//    for( int i=0; i<res; i++ ){
//        int id = i;
//        float x = cos(id * adder);
//        float y = sin(id * adder);
//        circle.addVertex( ofVec2f(x, y) );
//
//        id++;
//        x = cos(id * adder);
//        y = sin(id * adder);
//        circle.addVertex( ofVec2f(x, y) );
//    }
//
//    circles.setMode( OF_PRIMITIVE_LINES );
//    circles.setUsage( GL_DYNAMIC_DRAW );
}

void ofApp::setup_scene(){
    points.setMode( OF_PRIMITIVE_POINTS );
    points.setUsage( GL_DYNAMIC_DRAW );

//    n_points.setMode( OF_PRIMITIVE_POINTS );
//    n_points.setUsage( GL_DYNAMIC_DRAW );

    lines.setMode( OF_PRIMITIVE_LINES );
    lines.setUsage( GL_DYNAMIC_DRAW );

//    prep_lines.setMode( OF_PRIMITIVE_LINES );
//    prep_lines.setUsage( GL_DYNAMIC_DRAW );
//
//    prep_lines_b.setMode( OF_PRIMITIVE_LINES );
//    prep_lines_b.setUsage( GL_DYNAMIC_DRAW );
//
//    branchs.setMode( OF_PRIMITIVE_LINES );
//    branchs.setUsage( GL_DYNAMIC_DRAW );
//
//    flowers.setMode( OF_PRIMITIVE_LINES );
//    flowers.setUsage( GL_DYNAMIC_DRAW );

    
    // img
    string img_name_1 = "img/photo/4.jpg";
    string img_name_2 = "img/fixed_point/lg/losglaciares04.jpg";

    img.loadImage( img_name_1 );
    img2.loadImage( img_name_2 );
    
    // svg
    svg.load("svg/expl/A_G.svg");
    svg_r.load("svg/expl/A_G_render.svg");

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
    
    sim_frame = 300;
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    for (int i=0; i<num; i++) {
        exps.push_back( ofxExportImageSequence() );
        exps[i].setup(w, h, 25, GL_RGB, 0 );
        exps[i].setFilePattern(  dir_name + "/F_%05i.png");
        exps[i].setFrameRange( sim_frame, 750 );
        exps[i].setAutoExit( true );
    }

    ofSetWindowPosition(0, 0);
    ofSetWindowShape(win.x/2, win.y/2);

    //exps[0].startExport();
}

void ofApp::update(){

  	sim_frame = sim_frame%751;
    string path_L = "sim/v4/L/pL_" + ofToString( sim_frame,0,5,'0' )+ ".abc";
    string path_R = "sim/v4/R/pR_" + ofToString( sim_frame,0,5,'0' )+ ".abc";

    points.clearVertices();
    points.clearColors();
    
//    n_points.clearVertices();
//    n_points.clearColors();
    
    lines.clearColors();
    lines.clearVertices();

//    prep_lines.clearColors();
//    prep_lines.clearVertices();
//    
//    branchs.clearColors();
//    branchs.clearVertices();
//    
//    flowers.clearVertices();
//    flowers.clearColors();
//    
//    circles.clearVertices();
//    circles.clearColors();
    
#pragma mark LOAD_PARTICLE
    
    
    {
        ofxAlembic::Reader abc;
        abc.open(path_L);
        //abc.dumpNames();
    
        vector<ofVec3f> pos;
        abc.get( 0, pos );
        
        for (int i=0; i<pos.size(); i++) {
            points.addVertex( pos[i] );
            // n_points.addVertex( pos[i] );
        }
    }
    
    if(0){
        ofxAlembic::Reader abc;
        abc.open(path_R);
        //abc.dumpNames();
        vector<ofVec3f> pos;
        abc.get( 0, pos );
        for (int i=0; i<pos.size(); i++) {
            points.addVertex( pos[i] );
            // n_points.addVertex( pos[i] );
        }
    }
    
    float scale = 72;

    // adjust
    vector<ofVec3f> & vs = points.getVertices();
//    vector<ofVec3f> & n_vs = n_points.getVertices();
    
    for( int i=0; i<vs.size(); i++){
        vs[i] *= scale;
        // vs[i].z = 0;
        vs[i].rotate( 90, ofVec3f(0,1,0) );
        vs[i].rotate( -gAngle, ofVec3f(0,0,1) );
        vs[i] += center;
        
//        n_vs[i] *= scale;
//        n_vs[i].z = 0;
    }
    

    int w = img.getWidth();
    int h = img.getHeight();
    vector<ofFloatColor> vc;
    int np = points.getNumVertices();
    vc.assign(np, ofFloatColor(0));
    points.addColors( vc );
    for (int i=0; i<np; i++) {
        int x = i%w;
        int y = i/w;
        ofFloatColor c = img.getPixelsRef().getColor(x, y);
//        c.setBrightness( ofNoise(i)*0.3 + 0.1 );
//        c.setHueAngle( c.getHueAngle() - 20 );
        c.a = 0.8;
        points.setColor( np-i-1, c );
    }

    
#pragma mark NEAR_LINE
    if( 1 ){
        int num_line = 10;
        int num_dupl = 3;
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
        
//        for (int i=0; i<vs.size(); i++) {
//
//            ofFloatColor c = ofFloatColor(0.8, 0.8) - vc[i];
//            float alpha = c.a;
//            for (int j=0; j<num_line; j++) {
//
//                int outid = i*num_line + j;
//                if( output[outid].x != -12345){
//                    
//                    c.a = alpha;
//                    
//                    const ofVec3f &p1 = vs[i];
//                    ofVec3f &p2 = output[outid];
//                    lines.addVertex( p1 );
//                    lines.addVertex( p2 );
//                    lines.addColor( c );
//                    lines.addColor( c );
//                    
//                    c.a = 0.06;
//                    
//                    int n = ofNoise( i, j ) * 5.0 + 2;
//                    for( int k=0; k<n; k++ ){
//                        float rate = 1.0 + k/2;
//                        
//                        ofVec3f d1 = p1;
//                        ofVec3f d2 = p2;
//                        d1.x += ofRandomf() * rate;
//                        d1.y += ofRandomf() * rate;
//                        d2.x += ofRandomf() * rate;
//                        d2.y += ofRandomf() * rate;
//                        
//                        lines.addVertex( d1 );
//                        lines.addVertex( d2 );
//                        lines.addColor( c );
//                        lines.addColor( c );
//                    }
//                }
//            }
//        }
    }

    return;

    
    if(0){
        int np = points.getNumVertices()-1;
        const vector<ofVec3f> &vs = points.getVertices();
        const vector<ofFloatColor> &vc = points.getColors();
        
        for( int i=0; i<np; i++ ){
            
            const ofVec3f &pos1 = vs[np-i];
            
            float dist2center1 = ad_geom_util::dist_pl(pos1, st, end);
            float area_limit = 0; // + ofSignedNoise(i, ofGetFrameNum()*0.1) * 100;
            
            if( dist2center1<area_limit )
                continue;
            
            {
                int num_line = ofNoise(i*0.1, ofGetFrameNum()*0.5)*30 + 5;
                
                multimap<float, ofVec3f> near_p;
                pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
                for( int line=0; line<num_line; line++ ){
                    near_p.insert( pair1 );
                }
                
                for( int j=i+1; j<np; j++ ){
                    if( i==j ) continue;
                    
                    const ofVec3f &pos2 = vs[j];
                    
                    float dist = pos1.distance( pos2 );
                    if( dist>2 ){
                        multimap<float, ofVec3f>::iterator itr = near_p.end();
                        
                        itr--;
                        if( dist < itr->first ){
                            std::pair<float, ofVec3f> pair2( dist, pos2 );
                            near_p.insert( pair2 );
                            
                            multimap<float, ofVec3f>::iterator end = near_p.end();
                            near_p.erase( --end );
                        }
                    }
                }
                multimap<float, ofVec3f>::iterator itr = near_p.begin();
                
                for(; itr!=near_p.end(); itr++ ){
                    ofVec3f &pos2 = itr->second;
                    if(pos2.x == -12345) continue;
                    
                    float d = pos1.distance(pos2);
                    float limit = 600;
                    
                    if( d<10 || limit<d  ) continue;
                    
                    ofFloatColor c = ofFloatColor(0.8, 0.8) - vc[i];
                    
                    lines.addVertex( pos1 );
                    lines.addVertex( pos2 );
                    lines.addColor( c );
                    lines.addColor( c );
                    
                    c.a = 0.1;
                    
                    int n = ofNoise( i, ofGetFrameNum() ) * 20.0 + 2;
                    for( int k=0; k<n; k++ ){
                        float rate = 1.0 + k/2;
                        
                        ofVec3f d1 = pos1;
                        ofVec3f d2 = pos2;
                        d1.x += ofRandomf() * rate;
                        d1.y += ofRandomf() * rate;
                        d2.x += ofRandomf() * rate;
                        d2.y += ofRandomf() * rate;
                        
                        lines.addVertex( d1 );
                        lines.addVertex( d2 );
                        lines.addColor( c );
                        lines.addColor( c );
                    }
                }
            }
        }
    }
    
#pragma mark RANDOM_LINE

    if(0){
        const vector<ofVec3f> &vs = points.getVertices();
        const vector<ofFloatColor> &vc = points.getColors();

        int np = vs.size()-1;
        int n1 = 60000;
        
        for (int i=0; i<n1; i++) {
            
            int id1 = ofRandomuf() * np;
            int id2 = id1 + ofRandomf() * 300;
            
            id2 = abs(id2);
            id2 %= np;
            
            const ofVec3f &v1 = vs[id1];
            const ofVec3f &v2 = vs[id2];
            
            float dist2center1 = ad_geom_util::dist_pl(v1, st, end);
            float dist2center2 = ad_geom_util::dist_pl(v2, st, end);
            
            float area_limit = 50 + ofSignedNoise(i, ofGetFrameNum()*0.1) * 100;
            
            if( dist2center1<area_limit )// || dist2center2<area_limit)
                continue;
            
            float len = v1.distance(v2);
            
            if( 5<len && len<100 ){
                
                ofFloatColor c = vc[id1];
                c *= 0.8;
                c.a = 0.3;
                
                for (int k=0; k<10; k++) {
                    
                    ofVec3f r1( ofRandomf(), ofRandomf() );
                    ofVec3f r2( ofRandomf(), ofRandomf() );
                    lines.addVertex( v1 + r1 );
                    lines.addVertex( v2 + r2 );
                    lines.addColor( c );
                    lines.addColor( c );
                }
            }
        }
    }
    
#pragma mark PREP_LINE
    if(0){
        int n = 2000;
        int np = points.getNumVertices()-1;
        ofFloatColor c(0,0.75);
        
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
    
    if(0){
     
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
    if(0){
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
    if(0){
        int np = n_points.getNumVertices();

        {
            if( branchIds.size() > 125 ){
                //branchIds.pop_front();
            }else{
                int id = ofNoise( ofGetFrameNum()*0.01)*100;
                branchIds.push_back( id );
            }
            
            list<int>::iterator itr = branchIds.begin();
            for (int i=0; itr!=branchIds.end(); i++, itr++) {
                
                int id = *itr;
                
                ofVec3f p = n_points.getVertex( np-id );
                ofVec3f onLine = rf_st + (rf_end-rf_st)*ofNoise( i, ofGetFrameNum()*0.3 );
                float len = p.distance( onLine );
                int x = ofNoise(p.x) * img2.getWidth();
                int y = ofNoise(p.y) * img2.getHeight();
                ofFloatColor c = img2.getColor(x, y);

                if( 10<len && len < 3000){
                    ofVec3f dir = p - onLine;
                    float angle = (rf_end-rf_st).angle( dir );
                    if( 30<angle && angle<150 ){
                        //branchs.addVertex( p * 1.3 );
                        //branchs.addVertex( onLine );
                        //branchs.addColor( c );
                        //branchs.addColor( c );
                        
                        for (int k=0; k<10; k++) {
                            //int t = round( ofRandom(0,3) );
                            int t = 2;
                            ofVec2f r1( ofRandomf(), ofRandomf() );
                            ofVec2f r2( ofRandomf(), ofRandomf() );
                            r1 *= 0.3;
                            r2 *= 0.3;
                            ad_graph::add_branch( branchs, onLine+r1, p+r2, c, t, ofRandomuf()>0.5 );
                        }
                    }
                }
            }
        }


#pragma FLOWER
        if(0){
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
    draw_layer_0();
    draw_preview();
    draw_info();
    
    sim_frame++;
}

void ofApp::draw_layer_0(){
    
    ofDisableAntiAliasing();
    ofDisableSmoothing();
    ofEnableAlphaBlending();

    exps[0].begin(); {
        ofClear(0);
        ofBackground(255);
        
        //svg_r.draw();

        //if( bDebugDraw )
//        if( ofGetFrameNum()==0)
            svg.draw();

//        ofPushMatrix(); {
            {
                
                //ofEnableAntiAliasing();
                //ofEnableSmoothing();
//                glPointSize(4);
//                points.draw();
                
                glLineWidth(1);
                lines.draw();

//                glLineWidth(2);
//                prep_lines.setMode( OF_PRIMITIVE_LINES );
//                prep_lines.draw();
                
//                glPointSize(3);
//                prep_lines.setMode( OF_PRIMITIVE_POINTS );
//                prep_lines.draw();
            
//                glLineWidth(1);
//                prep_lines_b.draw();
            
//                glLineWidth(1);
//                circles.draw();
                
            }
            
            {
//                ofTranslate(center);
//                ofRotateZ( -gAngle );
                
//                glLineWidth(1);
//                branchs.draw();
//                flowers.draw();
                
            }
//        } ofPopMatrix();
        
//        ofPushMatrix(); {
//            glPointSize(2);
//            points.draw();
//        } ofPopMatrix();
        
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
                exps[i].startExport();
            }
            break;
            
        default:
            break;
    }
}
