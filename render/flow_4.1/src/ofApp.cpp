#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){
	ofSetWindowPosition( 0, 0 );
	bInit = false;
	bStart = false;
	bDraw_info = true;
	
    ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 255 );
	ofEnableAlphaBlending();
	ofEnableSmoothing();
	ofEnableAntiAliasing();
	
    cam.setDistance( 15.5 );
	cam.setFov( 20 );

	cam.setNearClip( 0.00001 );
    cam.setFarClip( 10000 );
	cam.setLensOffset( ofVec2f(0, 0.98) );
    
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );

    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

	frame=0;
}

void ofApp::update(){
	if( !bStart )
		return;
	
	frame++;

    ofxAlembic::Reader abc;
	string path = ad_util::data_path + "/sim/flow_4.1/HY_Emitter02_" + ofToString( frame,0,5,'0' )+ ".abc";
    abc.open(path);
    
    vector<ofVec3f> pos;
    abc.get( 0, pos );

//    if( !bInit && pos.size()>0 ) {
//        for( int i=0; i<pos.size(); i++ ){
//            initial_pos.push_back( pos[i] );
//        }
//        bInit = true;
//    }

    points.clear();
	int size = pos.size();
    for( int i=0; i<size; i++ ){
		if( size>10000 && i>size-70000 )
			break;
		ofVec3f p = pos[i];
        ofVec3f xA( i*0.00003,0,0 );
        points.addVertex( p ); //+ xA );
        points.addColor( ofFloatColor(ofRandom(0,0.15), ofRandom(0.75,0.85)) );
    }

    if( 0 ){
        lines.clear();
        int num_p = points.getNumVertices();
        int line_num = 10000;
        if( pos.size()!=0 && num_p>2 ){
            for( int i=0; i<line_num; i++ ){
                
                int index1 = floor( ofRandom( 0, num_p ));
                int index2 = floor( ofRandom( 0, num_p ));
                
                ofVec3f p1 = points.getVertex( index1 );
                ofVec3f p2 = points.getVertex( index2 );
                
                float dist = p1.distance( p2 );
                
                if( 0.01<dist && dist<0.15 ){
                    
                    lines.addVertex( p1 );
                    lines.addVertex( p2 );
                    
                    ofFloatColor c1, c2;
                    c1.white; c2.white;
                    c1.setHsb( ofRandom(1.0), 0.3, 0.6, 1 );
                    c2.setHsb( ofRandom(1.0), 0.3, 0.6, 1 );
                    lines.addColor( c1 );
                    lines.addColor( c2 );
                }
            }
        }
    }
}

void ofApp::draw(){
    
    ofBackground( 255 );
	cam.begin();
	glPointSize( 3 );
    points.draw();
    
    cam.end();
	
	saver.save();
	
	draw_info();
}

void ofApp::draw_info(){
	if( !bDraw_info) return;
	ofSetColor( 200 );
	ofRect( 5, 5, 600, 300 );
	int y = 20;
	ofSetColor( 0 );
	stringstream ss;
	ss << "fps       : " << (int)ofGetFrameRate() << "\n";
	ss << "cur frame : " << saver.frame_cur << "\n";
	ss << "end frame : " << saver.frame_end << "\n";
	ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
	ss << "space key : start genetic calculation\n";
	ss << "I     key : draw info\n";
	ss << "S     key : save image\n";
	ss << "f     key : toggle fullscreen\n";
	ofDrawBitmapString( ss.str(), 20, 20 );
}

void ofApp::keyPressed( int key ){
	
	switch( key ) {
		case ' ':
			bStart = !bStart;
			break;
			
		case 'f':
			ofSetWindowShape( 1920*2, 1080*2 );
			break;
			
		case 'I':
			bDraw_info = !bDraw_info;
			break;
			
		case 'S':
			bStart = true;
			saver.start( ofGetTimestampString(), "", 3000, frame );
			break;
			
		default:
			break;
	}
}
