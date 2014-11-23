//
//  ofApp2.h
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#pragma once

#include "ofMain.h"
#include "ad_grav_line.h"
#include "ad_image_saver.h"
#include "ofxGpuNoise.h"

class ofApp2 : public ofBaseApp{
    
public:
	
	static ofApp2 * app;
	static ofApp2 * init(){ app = new ofApp2(); return app; }
	
    void setup(){
        bDraw_info = true;
        cam.setDistance(1000);
        cam.setLensOffset( ofVec2f(0, 1.2) );

		gpu_noise.setup();
		gpu_noise.setOctaves( 4 );
		gpu_noise.setFreq( 0.05 );
		gpu_noise.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
		gpu_noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );

		int nsize = 128;
		gpu_noise.create( nsize, nsize);
		noise_size = nsize * nsize;
		gpu_noise.setFrame( 0.01 );
		gpu_noise.update();
		noise = gpu_noise.getNoiseData();
		
		gravline.setup( &cam );
		
    };
    
    void update(){
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
        
        if( frame == 300){
            gravline.create_line(p0, p1, 0.1 + ofRandomuf()*0.75 );
            gravline.create_line(p0, p2, 0.1 + ofRandomuf()*0.75 );
            gravline.create_line(p0, p3, 0.1 + ofRandomuf()*0.75 );
            gravline.create_line(p0, p4, 0.1 + ofRandomuf()*0.75 );
            gravline.create_line(p1, p2, 0.1 + ofRandomuf()*0.75 );
            gravline.create_line(p2, p3, 0.1 + ofRandomuf()*0.75 );
            gravline.create_line(p3, p4, 0.1 + ofRandomuf()*0.75 );
        }
    }
    
    void draw(){
        ofEnableAlphaBlending();
        ofEnableAntiAliasing();
        ofEnableSmoothing();
        ofBackground( 255 );
        cam.begin();
        
        gravline.draw();
        cam.end();

        saver.save();
//        draw_info();
//		gpu_noise.draw(300, 10,1);

	}

    void draw_info(){
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

        ss << "\nGpu Noise\n";
        ss << "type      : " << gpu_noise.getShaderType() << "\n";
        ss << "deriv     : " << gpu_noise.getShaderDerivType() << "\n";
        ss << "Freq      : " << gpu_noise.getFreq() << "\n";
        ss << "\n";

        ofSetColor( 0 );
        ofDrawBitmapString( ss.str(), 10, 20 );
    };
    
    void keyPressed( int key ){
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
	
	float getNoise( int index, int ch=0 ){
		index %= noise_size;
		return noise[index*3 + ch]/255.0;
	}

	float getSignedNoise( int index, int ch=0 ){
		index %= noise_size;
		return (noise[index*3 + ch]-128)*2.0/255.0;
	}

	bool bDraw_info;
    float frame;
    
    ofEasyCam cam;
    ad_grav_line gravline;

    ad_image_saver saver;
	
	ofxGpuNoise gpu_noise;
	int noise_size;
	unsigned char * noise;
};
