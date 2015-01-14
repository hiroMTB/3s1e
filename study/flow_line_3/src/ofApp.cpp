#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){
	
//	ofSetLogLevel( OF_LOG_VERBOSE );
	
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
	
	
	
	cam.setTarget( ofVec3f(0,0,0) );
	cam.setDistance( 50 );
	cam.lookAt( ofVec3f(0,0,0), ofVec3f(1, 0, 0) );
			   
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
	
	bool ok = img.loadImage( ad_util::data_path  + "img/gns3.jpg");	// 2333 * 1477px
	if( !ok ) cout << "image load failer" << endl;
	
	gn.setup();
	gn.create( 256, 256 );
	gn.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
	gn.setFreq( 0.005 );
	
	int w = 5000;
	int h = 2500;
	
	exporter.setup(w, h, 25, GL_RGB, 0);
    exporter.setFilePattern( ofGetTimestampString() + "/F_%05i.png");
    exporter.setFrameRange(0, 500);
	exporter.setAutoExit(true);
	exporter.setOutputDir( ofGetTimestampString() );
	
}

void ofApp::update(){
	if( !bStart )
		return;
	
	gn.setFrame( ofGetFrameNum()*0.001 );
	gn.update();
	
	frame++;
	frame = (int)frame % 479;
	
	string path = ad_util::data_path + "sim/line_flow_2/Linear01_" + ofToString( frame,0,5,'0' )+ ".abc";
	
	ofxAlembic::Reader abc;
	abc.open(path);
//	abc.dumpNames();
	
    vector<ofVec3f> pos;
    abc.get( 0, pos );
	
    points.clear();

	unsigned char * pix = img.getPixels();
	int n = pos.size();
	
	for(int i=0; i<n; i++ ){

		// inverse walksthrough
		int index  = (n-1-i) % (int)(img.getWidth() * img.getHeight() );
		ofVec3f &p = pos[index];
		if( bOrtho ) p.z = 0;
		
		points.addVertex( p );
		
		/*
		 *	color animation
		 */
		ofFloatColor c( pix[ index*3 + 0] / 255.0,
					    pix[ index*3 + 1] / 255.0,
						pix[ index*3 + 2] / 255.0
					   );
		c.setHue( c.getHue()*0.25 + gn.getNoiseuf(index, 0)*0.6 + 0.2);
		c.setSaturation( c.getSaturation()*0.5 + gn.getNoisef( index, 1 )*0.5 );
		c.setBrightness( c.getBrightness()*0.5 + gn.getNoisef( index, 2 )*0.5 -0.2);
		c.a = gn.getNoiseuf( i+1000, 2 )*0.3 + 0.5;

		points.addColor( c );
	}

	abc.close();
}

void ofApp::draw(){
	exporter.begin();{
		
		ofEnableAlphaBlending();
		
		
		
		ofBackground( 255 );

		ofPushMatrix();{
			if(!bOrtho){
				cam.begin();
			}else{
				ofSetupScreenOrtho();
				ofTranslate(exporter.getFbo().getWidth()/2, exporter.getFbo().getHeight()/2);
				ofScale(80, -80);
				ofRotate(90, 0, 0, 1);
			}
			
			ofRotateY(rot);
			glPointSize( 1 );
			points.draw();
			
			//ofSetColor(0);
			//ofLine(ofPoint(0, -500, 0), ofPoint(0,-550,0));

			//ofSetColor(0, 0, 255);
			//ofLine(ofPoint(0, 0, -0.1), ofPoint(0,0,0.1));

			if(!bOrtho){
				cam.end();
			}
		}ofPopMatrix();
	
	} exporter.end();
	
	ofBackground(255);
	ofSetColor(255);
	exporter.draw(0, 0);
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
	ss << "exporter frame : " << exporter.getFrameNum() << "\n";
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
	
	ss << "\n" << gn.getNoiseParamString() << "\n";
	
	ofDrawBitmapString( ss.str(), 20, 20 );
	gn.draw( 300, 10, 1 );
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
			exporter.startExport();
			ofSetWindowShape(exporter.getFbo().getWidth()/4, exporter.getFbo().getHeight()/4 );
			ofSetWindowPosition(0, 0);
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
			
//		case OF_KEY_UP:
//			fov+=10;
//			cam.setFov(fov);
//			break;
//			
//		case OF_KEY_DOWN:
//			fov-=10;
//			cam.setFov(fov);
//			break;

		case OF_KEY_RIGHT:
		{
			int i = gn.getShaderType();
			gn.setShaderType((ofxGpuNoise::ShaderType)++i);
			break;
		}
		case OF_KEY_LEFT:
		{
			int i = gn.getShaderType();
			gn.setShaderType((ofxGpuNoise::ShaderType)--i);
			break;
		}
			
		case OF_KEY_UP:
			gn.setFreq( gn.getFreq()*2.0 );
			break;
			
		case OF_KEY_DOWN:
			gn.setFreq( gn.getFreq()*0.5 );
			break;
			
		default:
			break;
	}
}
