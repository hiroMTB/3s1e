//
//  ofApp.cpp
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#include "ofApp.h"
#include "ad_util.h"

ofApp * ofApp::app = NULL;

void ofApp::setup(){
    bDraw_info = true;
    bStart = true;
    
    sABC.load( "svg_r/v2/ABC_r.svg" );
    ofSetCircleResolution( 4 );

    gpu_noise.setup();
    gpu_noise.setOctaves( 4 );
    gpu_noise.setFreq( 0.01 );
    gpu_noise.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gpu_noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
    int nsize = 512;
    gpu_noise.create( nsize, nsize);
    noise_size = nsize * nsize;
    gpu_noise.setFrame( 0.01 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    
    bool imgload = img.loadImage( "img/fixed_point/lg/losglaciares02.jpg");
	if( imgload ){
		cout << "load image ok " << endl;
		grav_wall.setup( &img );
	}else{
		cout << " ! load image fail" << endl;
		grav_wall.setup( NULL );
	}
	
	
	
    int n = sABC.getNumPath();
    cout << "load SVG success, found " << n << " paths" << endl;
    for( int i=0; i<n; i++ ){
        ofPath &p = sABC.getPathAt(i);
        ofFloatColor c = p.getStrokeColor();
        
        vector<ofPolyline>& lines = p.getOutline();
        for(int j=0;j<(int)lines.size();j++){
            ofPoint st = ( lines[j].getVertices()[0] );
            ofPoint end = ( lines[j].getVertices()[1]);
            grav_wall.create_line(st, end, 1.5 );
            cout << "crate line" << endl;
        }
    }
    
    ofSetVerticalSync( true );
    int w = sABC.getWidth();
    int h = sABC.getHeight();
    exporter.setup(w, h, 25, GL_RGBA, 8);
    exporter.setAutoExit(true);
    exporter.setFrameRange(1, 3000);
    exporter.setOutputDir( ofGetTimestampString() );
    ofSetWindowPosition(0, 0);
    ofSetWindowShape(w*0.5, h*0.5);
    //exporter.startExport();
};


void ofApp::update(){
    
    frame = ofGetFrameNum();
    gpu_noise.addFrame( 0.03 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    grav_wall.update();
    
    if( exporter.getFrameNum() > 300 ){
        grav_wall.impulse = -2.0*0.2;
    }
    
}

void ofApp::draw(){
    exporter.begin();{
        ofEnableAlphaBlending();
        ofEnableAntiAliasing();
        ofEnableSmoothing();

        ofBackground( 255 );
        grav_wall.draw();

    }exporter.end();

    ofPushMatrix();{
        ofSetupScreenOrtho();
        ofBackground(0);
        ofSetColor(255);
        exporter.draw(0, 0);

        draw_info();
        gpu_noise.draw(300, 10, 0.2);
        

    }ofPopMatrix();
}

void ofApp::draw_info(){
    if ( !bDraw_info)
        return;
    
    stringstream ss;
    ss << "fps: " << ofToString( ofGetFrameRate(),0 ) << "\n";
    ss << "frame     : " << ofGetFrameNum() << "\n";
    ss << "cur frame : " << saver.frame_cur << "\n";
    ss << "end frame : " << saver.frame_end << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
    //        ss << "num shapes: " << shapes.size() << "\n";
    ss << "I key:   toggle info " << "\n";
    ss << "Collision Shape : " << grav_wall.world.world->getNumCollisionObjects() << endl;
    ss << "\n" << gpu_noise.getNoiseParamString() << "\n";
    
    ofSetColor( 0 );
    ofDrawBitmapString( ss.str(), 10, 20 );
};

void ofApp::keyPressed( int key ){
    switch( key ) {
        case 'f':
            ofToggleFullscreen();
            break;

        case 'S':
            exporter.startExport();
            break;
            
        case OF_KEY_RIGHT:
        {
            int i = gpu_noise.getShaderType();
            gpu_noise.setShaderType((ofxGpuNoise::ShaderType)++i);
            break;
        }
        case OF_KEY_LEFT:
        {
            int i = gpu_noise.getShaderType();
            gpu_noise.setShaderType((ofxGpuNoise::ShaderType)--i);
            break;
        }
            
        case OF_KEY_UP:
            gpu_noise.setFreq( gpu_noise.getFreq()*2.0 );
            break;
            
        case OF_KEY_DOWN:
            gpu_noise.setFreq( gpu_noise.getFreq()*0.5 );
            break;
            
            
        case 'g':
            grav_wall.impulse = -grav_wall.impulse*0.2;
            break;
    };
}
