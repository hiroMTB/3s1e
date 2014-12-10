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
    
    sABC.load( ad_util::data_path +  "svg/v1/ABC.svg" );
    ofSetCircleResolution( 6 );

    gpu_noise.setup();
    gpu_noise.setOctaves( 4 );
    gpu_noise.setFreq( 0.0021 );
    gpu_noise.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gpu_noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
    int nsize = 512;
    gpu_noise.create( nsize, nsize);
    noise_size = nsize * nsize;
    gpu_noise.setFrame( 0.01 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    
    gravline.setup();
    
    int n = sABC.getNumPath();
    cout << "load SVG success, found " << n << " paths" << endl;
    for( int i=0; i<n; i++ ){
        ofPath &p = sABC.getPathAt(i);
        ofFloatColor c = p.getStrokeColor();
        
        vector<ofPolyline>& lines = p.getOutline();
        for(int j=0;j<(int)lines.size();j++){
            ofPoint st = ( lines[j].getVertices()[0] );
            ofPoint end = ( lines[j].getVertices()[1]);
            
            gravline.create_line(st, end, 0.4 );
        }
    }
    
    ofSetVerticalSync(true);
    float w = sABC.getWidth()  + 1;
    float h = sABC.getHeight() + 1;
    canvas.set(0, 0, w, h);
    exporter.setOutputDir( "ex2" );
//    exporter.setFrameRange(1, 1600);
    exporter.setup(w, h, 30, GL_RGBA, 8);


    ofSetWindowShape(w/2, h/2);
//    exporter.startExport();
};

void ofApp::update(){
    
    frame = ofGetFrameNum();
    gpu_noise.setFrame( frame *0.001 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    gravline.update();
}

void ofApp::draw(){
    exporter.begin();{
        ofEnableAlphaBlending();
        ofEnableAntiAliasing();
        ofEnableSmoothing();

        ofBackground( 255, 255, 255, 255 );
        gravline.draw();
        if(!exporter.isExporting()) gravline.draw_attr();
        
    }exporter.end();

    ofPushMatrix();{
        ofBackground(0,0,0,255);
        ofSetColor(255);
        ofSetupScreenOrtho();
        exporter.draw(0, 0);

        draw_info();
//        gpu_noise.draw(300, 10, 0.2);
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
    ss << "Collision Shape : " << gravline.world.world->getNumCollisionObjects() << endl;
    ss << "\n" << gpu_noise.getNoiseParamString() << "\n";
    ss << "joints    : " << gravline.joints.size() << "\n";
    
    ofSetColor( 0 );
    ofDrawBitmapString( ss.str(), 10, 40 );
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
    };
}
