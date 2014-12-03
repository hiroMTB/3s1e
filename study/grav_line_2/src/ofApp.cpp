//
//  ofApp.cpp
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#include "ofApp.h"

ofApp * ofApp::app = NULL;

void ofApp::setup(){
    bDraw_info = true;
    bStart = true;
    bInit = false;
    
    ofSetCircleResolution( 6 );
    
    cam.setDistance(1000);
    cam.setLensOffset( ofVec2f(0, 1.2) );
    
    gpu_noise.setup();
    gpu_noise.setOctaves( 4 );
    gpu_noise.setFreq( 0.05 );
    gpu_noise.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gpu_noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
    int nsize = 128;
    gpu_noise.create( nsize*nsize, 1);
    noise_size = nsize * nsize;
    gpu_noise.setFrame( 0.01 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    
    gravline.setup( &cam );

};

void ofApp::update(){
    frame = ofGetFrameNum();
    gpu_noise.setFrame( frame *0.01 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    gravline.update();
    
    ofVec2f p0(0,0);
    ofVec2f p1(-900,1000);
    ofVec2f p2(-100,800);
    ofVec2f p3(600,1200);
    ofVec2f p4(800,600);
    
    if( bStart && !bInit && frame != 0){
        bInit = true;
        gravline.create_line(p0, p1, 0.1 + ofRandomuf()*0.75 );
        gravline.create_line(p0, p2, 0.1 + ofRandomuf()*0.75 );
        gravline.create_line(p0, p3, 0.1 + ofRandomuf()*0.75 );
        gravline.create_line(p0, p4, 0.1 + ofRandomuf()*0.75 );
        gravline.create_line(p1, p2, 0.1 + ofRandomuf()*0.75 );
        gravline.create_line(p2, p3, 0.1 + ofRandomuf()*0.75 );
        gravline.create_line(p3, p4, 0.1 + ofRandomuf()*0.75 );
    }
    
    if( frame != 0 && (int)frame%10 == 0) {
        gravline.add_random_particle( 2 );
    }
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofBackground( 255 );
    ofSetColor(255);
    cam.begin();
    
    gravline.draw();
    cam.end();
    
    saver.save();
    draw_info();
    // gpu_noise.draw(300, 10,1);
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
    ss << "\nCam\n";
    ss << "pos       : " << cam.getPosition().x << ", " << cam.getPosition().y << "," << cam.getPosition().z << "\n";
    ss << "target pos: " << cam.getTarget().getPosition().x << ", " << cam.getTarget().getPosition().y << "," << cam.getTarget().getPosition().z << "\n";
    ss << "dist      : " << cam.getDistance() << "\n";
    ss << "fov       : " << cam.getFov() << "\n";
    
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
            saver.start(ofGetTimestampString(), "", 3000);
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
