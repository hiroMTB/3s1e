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
    
    sABC.load( "ABC.svg" );
    ofSetCircleResolution( 6 );

    gpu_noise.setup();
    gpu_noise.setOctaves( 4 );
    gpu_noise.setFreq( 0.002 );
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
        
//        p.setPolyWindingMode(OF_POLY_WINDING_ODD);
        vector<ofPolyline>& lines = p.getOutline();
        for(int j=0;j<(int)lines.size();j++){
            ofPoint st = ( lines[j].getVertices()[0] );
            ofPoint end = ( lines[j].getVertices()[1]);
            
            for(int k=0; k<1; k++)
                gravline.create_line(st, end, 0.15 );
            
            cout << "crate line" << endl;
        }
    }
    
    ofSetVerticalSync(true);
    int w = sABC.getWidth();
    int h = sABC.getHeight();
    exporter.setFrameRange(0, 6000);
    exporter.setup(w, h, 25, GL_RGB, 0);
    exporter.setFilePattern( ofGetTimestampString() + "/F_%05i.png");
    exporter.setAutoExit(true);

    //exporter.startExport();
};

void ofApp::update(){
    
    frame = ofGetFrameNum();
    gpu_noise.setFrame( frame *0.001 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    gravline.update();
    
    if( frame != 0 && (int)frame%8 == 0) {
//        gravline.add_random_particle( 3 );
    }
}

void ofApp::draw(){
    exporter.begin();{
        ofEnableAlphaBlending();
        
        

        ofBackground( 255 );
        gravline.draw();
            if(!exporter.isExporting())
                gravline.draw_attr();
    }exporter.end();

    ofPushMatrix();{
        ofBackground(0);
        ofSetColor(255);
        ofSetupScreenOrtho();
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
    ss << "Collision Shape : " << gravline.world.world->getNumCollisionObjects() << endl;
    ss << "\n" << gpu_noise.getNoiseParamString() << "\n";
    ss << "joints    : " << gravline.joints.size() << "\n";
    
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
    };
}
