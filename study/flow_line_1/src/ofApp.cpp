#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){
	ofSetWindowPosition( 0, 0 );
	bInit = false;
	bStart = false;
	bDraw_info = true;
	
    ofSetVerticalSync( true );

	ofBackground( 255 );
	ofEnableAlphaBlending();
	ofEnableSmoothing();
	ofEnableAntiAliasing();
	
	
	cam.setTarget( ofVec3f(0,0,0) );
	cam.setDistance( 40 );
	
//	cam.setFov( 20 );
	cam.setNearClip( 0.00001 );
    cam.setFarClip( 10000 );
	
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );

    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

	frame=0;
	
	vector<ofVec3f> v;
	v.assign(100000, ofVec3f(0,0,0) );
	points.addVertices(v);
}

void ofApp::update(){
	if( !bStart )
		return;
	
	frame++;
	frame = (int)frame % 500;
	
	string path = ad_util::data_path + "sim/line_flow_1/Linear01_" + ofToString( frame,0,5,'0' )+ ".abc";
	
	ofxAlembic::Reader abc;
	abc.open(path);
//	abc.dumpNames();
	
    vector<ofVec3f> pos;
    abc.get( 0, pos );
	
    points.clear();
	int size = pos.size();
    for( int i=0; i<size; i++ ){
		ofVec3f p = pos[i];
        ofVec3f xA( i*0.00003,0,0 );
		p.x = 0;
		points.addVertex( p ); //+ xA );
        points.addColor( ofFloatColor(ofRandom(0,0.3), ofRandom(0.5,0.8)) );
    }
	abc.close();
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
	
    ofBackground( 255 );
	cam.begin();
	ofRotateY(90);
	glPointSize( 1 );
    points.draw();
	
	ofSetColor(0);
	ofLine(ofPoint(0, -50, 0), ofPoint(0,50,0));

	ofSetColor(0, 0, 255);
	ofLine(ofPoint(0, 0, -0.1), ofPoint(0,0,0.1));

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
	ss << "frame     : " << (int)frame << "\n";
	ss << "cur frame : " << saver.frame_cur << "\n";
	ss << "end frame : " << saver.frame_end << "\n";
	ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
	ss << "cam pos   : " << cam.getPosition().x << ", " << cam.getPosition().y << "," << cam.getPosition().z << "\n";
	ss << "cam target pos : " << cam.getTarget().getPosition().x << ", " << cam.getTarget().getPosition().y << "," << cam.getTarget().getPosition().z << "\n";

	ss << "cam dist  : " << cam.getDistance() << "\n";
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
