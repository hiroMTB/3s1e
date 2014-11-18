#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){
	ofSetWindowPosition( 0, 0 );
	bInit = false;
	bStart = false;
	bDraw_info = true;
	bZeroDepth = false;
	bOrtho = false;
	
    ofSetVerticalSync( true );
	ofSetFrameRate(30);
	
	ofBackground( 255 );
	ofEnableAlphaBlending();
	ofEnableSmoothing();
	ofEnableAntiAliasing();
	
	cam.setTarget( ofVec3f(0,0,0) );
	cam.setDistance( 40 );
	
	fov = cam.getFov();
	cam.setNearClip( 0.00001 );
    cam.setFarClip( 10000 );
	
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );

    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

	frame=0;
	
	vector<ofVec3f> v;
	v.assign(1000000, ofVec3f(0,0,0) );
	points.addVertices(v);

	rot = 0;
}

void ofApp::update(){
	if( !bStart )
		return;
	
	frame++;
	frame = (int)frame % 479;
	
	string path = ad_util::data_path + "sim/line_flow_2/Linear01_" + ofToString( frame,0,5,'0' )+ ".abc";
	
	ofxAlembic::Reader abc;
	abc.open(path);
//	abc.dumpNames();
	
    vector<ofVec3f> pos;
    abc.get( 0, pos );
	
    points.clear();
	int size = pos.size();
    for( int i=0; i<size; i++ ){
		ofVec3f p(pos[i].z, pos[i].y, pos[i].x);
        ofVec3f xA( i*0.00003,0,0 );
		if(bZeroDepth ){
			p.z = 0;
		}

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

	ofPushMatrix();
	if(bOrtho){
		ofSetupScreenOrtho();
		ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
		ofScale(24, -24);
	}else{
		cam.begin();

	}

	ofRotateY(rot);
	glPointSize( 1 );
    points.draw();
	
	ofSetColor(0);
	ofLine(ofPoint(0, -50, 0), ofPoint(0,50,0));

	ofSetColor(0, 0, 255);
	ofLine(ofPoint(0, 0, -0.1), ofPoint(0,0,0.1));

	if(!bOrtho){
		cam.end();
	}
	ofPopMatrix();
	
	saver.save();
	
	draw_info();
}

void ofApp::draw_info(){
	ofPushMatrix();
	ofSetupScreenOrtho();
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

	ss << "particles : " << points.getNumVertices() << "\n";
	ss << "depth=0   : " << bZeroDepth << "\n";

	ss << "\ncam\n";
	ss << "ortho     : " << bOrtho << "\n";
	ss << "pos       : " << cam.getPosition().x << ", " << cam.getPosition().y << "," << cam.getPosition().z << "\n";
	ss << "target pos: " << cam.getTarget().getPosition().x << ", " << cam.getTarget().getPosition().y << "," << cam.getTarget().getPosition().z << "\n";
	ss << "dist      : " << cam.getDistance() << "\n";
	ss << "fov       : " << fov << "\n";

	ss << "\n";
	
	ss << "space key : start genetic calculation\n";
	ss << "I     key : draw info\n";
	ss << "S     key : save image\n";
	ss << "f     key : toggle fullscreen\n";
	ss << "d     key : toggle zero depth \n";
	ss << "o     key : toggle ortho\n";
	ss << "up / down key : fov +-10 \n";
	
	ofDrawBitmapString( ss.str(), 20, 20 );
	ofPopMatrix();
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
			
		case 'd':
			bZeroDepth = !bZeroDepth;
			break;
			
		case 'r':
			rot += 90;
			break;

		case 'o':
			bOrtho = !bOrtho;
			break;
			
		case OF_KEY_UP:
			fov+=10;
			cam.setFov(fov);
			break;
			
		case OF_KEY_DOWN:
			fov-=10;
			cam.setFov(fov);
			break;

		default:
			break;
	}
}
