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

    img.loadImage( ad_util::data_path + "img/rk/Mx80_1_org.jpg" );
    
    cps.setMode(OF_PRIMITIVE_POINTS);
    cps.setUsage(GL_DYNAMIC_DRAW);
    
    win.x = 1920;
    win.y = 1080;
    setup_export_layer( win.x, win.y, layer_num );
	setup_spline();
}

void ofApp::setup_export_layer( int w, int h, int num ){

    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    exporter.setup(w, h, 25, GL_RGB, 0 );
    exporter.setFilePattern(  dir_name + "/F_%05i.png");
    exporter.setFrameRange( 1, 3000 );
    exporter.setAutoExit( true );
    
    ofSetWindowShape(w, h);
    ofSetWindowPosition(0, 0);
}

void ofApp::setup_spline(){

	int n = 3000;
	
	for( int i=0; i<n; i++ ){
		vector<ofVec3f> vs;
		ofVec3f v( 0,0,0 );
		vs.push_back( v );

		v.x = 220 + ofRandom(0, 10);
		vs.push_back( v );

		for (int j=2; j<6; j++) {
			ofVec3f v( vs[j-1].x + ofRandomuf() + j*10, 0 );
			vs.push_back( v );
		}
		
		points.push_back(vs);
		
		sps.push_back( ofxSimpleSpline() );
		sps[i].setSubdivisions(10);
		sps[i].setControlVertices( points[i] );

        list<ofxSimpleSpline> listsp;
        sps_p.push_back(listsp);
    }
}

void ofApp::add_spline(){
    
    if( sps[0].controlVertices->size() > 750 ){
        return;
    }
    
    for (int i=0; i<sps.size(); i++) {

        int ncv = sps[i].controlVertices->size();
        const ofVec3f &vp = sps[i].controlVertices->at(ncv-1);
        ofVec3f add = ofVec3f( 10, 0, 0) ;
        
        float n = ofNoise( 1, ofGetFrameNum()*0.1 );
        if( n > 0.75) {
            add *= 6;
        }
        
        sps[i].controlVertices->push_back( vp + add );
    }
}

void ofApp::update(){
	
    float frame = ofGetFrameNum();
    
	for (int i=0; i<points.size(); i++) {

		float r = ofNoise( i, (float)i*0.01 ) + 0.1;
		float freq = ofNoise( i*2, (float)i*0.02 )*0.1 + 0.1;
		r *= 30;
		
		for (int j=1; j<points[i].size(); j++) {
            
            const ofVec3f & past = points[i][j-1];
            points[i][j].x += ofNoise(i, points[i][j].y, frame*0.1) * 0.1;
            points[i][j].y = past.y;
            points[i][j].y += ofSignedNoise(i, j, frame*0.01) * r;
		}
        
        points[i][1].y *= 0.3;
        points[i][2].y *= 0.4;
        points[i][3].y *= 0.5;
        points[i][4].y *= 0.6;
        points[i][5].y *= 0.7;
	}

    
    for (int i=0; i<sps.size(); i++) {
        sps[i].setControlVertices( points[i] );
    }
    
    
    // Color
    int imgW = img.getWidth();
    int imgH = img.getHeight();
    for (int i=0; i<sps.size(); i++) {
        vector<ofFloatColor> cols;
        for (int j=0; j<sps[i].lineVbo.getNumVertices(); j++) {
            int x = i*120 % imgW;
            int y = j / imgW;
            ofFloatColor c = img.getColor(x, y);
            c.a = 0.001 + j*0.00005;
            cols.push_back( c );
        }
        
        sps[i].lineVbo.setColorData(&cols[0], cols.size(), GL_DYNAMIC_DRAW );
        //sps[i].lineVbo.updateColorData(&cols[0], cols.size());
    }
    
    add_spline();
    
    
    // History
    hisnum = 50;
    for (int i=0; i<sps.size(); i++) {

        sps_p[i].push_back( sps[i] );

        // size of each history
        int nh = sps_p[i].size();

        // keep only 10 history
        if( nh>hisnum ) {
            sps_p[i].pop_front();
        }
        
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
    exporter.begin(); {
        ofClear(0);
		ofBackground(0);

        ofTranslate( 10, win.y/2 );
		ofRotateZ( -30.0 );
        float s = 0.5;
        ofScale(s,s,s);

        ofNoFill();

        // history
        ofSetColor(255);
        ofPushMatrix();
        for (int i=0; i<sps_p.size(); i++) {
            ofRotateZ( 60.0/sps_p.size() );

            list<ofxSimpleSpline>::iterator itr = sps_p[i].begin();
            for (; itr!=sps_p[i].end(); itr++) {
                itr->draw();
            }
        }
        ofPopMatrix();

        // Present
        ofPushMatrix();
        for (int i=0; i<sps.size(); i++) {
            ofSetColor( 255 );
            ofRotateZ( 60.0/sps.size() );
            sps[i].draw();

            ofSetColor( 200, 5);
            cps.clearVertices();
            cps.addVertices( *sps[i].controlVertices );
            cps.draw();
        }
        
        ofPopMatrix();

        
    } exporter.end();
}

void ofApp::draw_preview(){
    
    ofPushMatrix(); {
        ofBackground(255);
        ofSetColor(255);
        exporter.draw(0, 0);
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
            exporter.startExport();
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
