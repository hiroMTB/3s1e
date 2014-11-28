#include "ofApp.h"

void ofApp::setup(){
	
	ofSetWindowTitle( "ofxGpuPerlinSample" );
	ofSetFrameRate( 60 );
	
	w = 256;
	h = 256;
	
	noise1.setup();
	noise1.setOctaves( 1 );
	noise1.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
	noise1.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
	noise1.setSendSamplingPoints( false );
	noise1.setSamplingPointsScale( 0.001 );
	noise1.create( w, h );
	mesh1.clear();
	vector<ofVec3f> ps1;
	ps1.assign(w*h*3, ofVec3f(0,0,0) );
	vector<ofFloatColor> cs1;
	cs1.assign(w*h*3, ofFloatColor(0.5, 1) );
	mesh1.addVertices( ps1 );
	mesh1.addColors( cs1 );
	mesh1.setMode( OF_PRIMITIVE_POINTS );
	mesh1.setUsage( GL_DYNAMIC_DRAW );

	noise2.setup();
	noise2.setOctaves( 8 );
	noise2.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
	noise2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
	noise2.setSendSamplingPoints( false );
	noise2.setSamplingPointsScale( 0.001 );
	noise2.create( w, h );
	mesh2.clear();
	vector<ofVec3f> ps2;
	ps2.assign(w*h*3, ofVec3f(0,0,0) );
	vector<ofFloatColor> cs2;
	cs2.assign(w*h*3, ofFloatColor(0.5, 1) );
	mesh2.addVertices( ps2 );
	mesh2.addColors( cs2 );
	mesh2.setMode( OF_PRIMITIVE_POINTS );
	mesh2.setUsage( GL_DYNAMIC_DRAW );
}

void ofApp::update(){
	{
		noise1.setFrame( ofGetFrameNum() * 0.01 );
		noise1.update();
		
		vector<ofVec3f> &vs = mesh1.getVertices();
		vector<ofFloatColor> &cs = mesh1.getColors();
		
		/*
		 *      this loop could be around 262,144( when you use 512*512 noise)
		 *      Be carefull how to get/set all pixel data
		 */
		int index = 0;
		for( int y=0; y<h; y++ ){
			for( int x=0; x<w; x++ ){
				float r = noise1.getNoisef( index, 0 );
				float g = noise1.getNoisef( index, 1 );
				float b = noise1.getNoisef( index, 2 );
				//float avg = (r + g + b ) * 0.3333;
				
				vs[index*3+0].set( index, r*100.0,0 );
				vs[index*3+1].set( index, g*100.0,0 );
				vs[index*3+2].set( index, b*100.0,0 );
				
				cs[index*3+0].set( 1,0,0 );
				cs[index*3+1].set( 0,1,0 );
				cs[index*3+2].set( 0,0,1 );
				
				index++;
			}
		}
	}

	{
		noise2.setFrame( ofGetFrameNum() * 0.01 );
		noise2.update();
		
		vector<ofVec3f> &vs = mesh2.getVertices();
		vector<ofFloatColor> &cs = mesh2.getColors();
		
		/*
		 *      this loop could be around 262,144( when you use 512*512 noise)
		 *      Be carefull how to get/set all pixel data
		 */
		int index = 0;
		for( int y=0; y<h; y++ ){
			for( int x=0; x<w; x++ ){
				float r = noise2.getNoisef( index, 0 );
				float g = noise2.getNoisef( index, 1 );
				float b = noise2.getNoisef( index, 2 );
				float avg = (r + g + b ) * 0.3333;
				
				vs[index*3+0].set( index, r*100.0,0 );
				vs[index*3+1].set( index, g*100.0,0 );
				vs[index*3+2].set( index, b*100.0,0 );
				
				cs[index*3+0].set( 1,0,0 );
				cs[index*3+1].set( 0,1,0 );
				cs[index*3+2].set( 0,0,1 );
				
				index++;
			}
		}
	}
}

void ofApp::draw(){
	ofEnableAlphaBlending();
	ofEnableAntiAliasing();
	ofEnableSmoothing();
	
	ofBackground( 0 );
	ofSetColor( 255 );
	
	ofPushMatrix(); {
		ofTranslate(20, 40 );
		
		ofPushMatrix(); {
			noise1.draw( 0, 0, 1 );
			noise2.draw( noise1.getWidth()+ 20, 0, 1 );
		} ofPopMatrix();
		
		ofPushMatrix(); {
			ofTranslate( w + 20, 0 );
			noise1.draw_samplingPointTexture( 0, 0, 0.5 );
		}ofPopMatrix();
		
		ofPushMatrix();{
			ofTranslate( 20, ofGetHeight()/2 - 100 );
			ofSetColor( 255 );
			glPointSize( 1 );
            mesh1.draw();
		}ofPopMatrix();

		ofPushMatrix();{
			ofTranslate( 20, ofGetHeight()/2 + 200 );
			ofSetColor( 255 );
			glPointSize( 1 );
			mesh2.draw();
		}ofPopMatrix();
	}ofPopMatrix();
	
	ofPushMatrix();{
		ofSetColor( 255 );
		stringstream ss;
		ss << "fps: " << ofGetFrameRate() << "\n";
		ofDrawBitmapString( ss.str(), 20, 20 );
	}ofPopMatrix();
}

void ofApp::keyPressed(int key){
	switch ( key){
		case 'p':
			noise1.toggleUseFastFboReader();
			break;
			
		case 'f':
			ofToggleFullscreen();
			break;
		case OF_KEY_RIGHT:
		{
			int i1 = noise1.getShaderType();
			noise1.setShaderType((ofxGpuNoise::ShaderType)++i1);

			int i2 = noise2.getShaderType();
			noise2.setShaderType((ofxGpuNoise::ShaderType)++i2);
			break;
		}
		case OF_KEY_LEFT:
		{
			int i1 = noise1.getShaderType();
			noise1.setShaderType((ofxGpuNoise::ShaderType)--i1);

			int i2 = noise2.getShaderType();
			noise2.setShaderType((ofxGpuNoise::ShaderType)--i2);
			break;
		}
			
		case OF_KEY_UP:
			noise1.setFreq( noise1.getFreq()*2.0 );
			noise2.setFreq( noise2.getFreq()*2.0 );
			break;
			
		case OF_KEY_DOWN:
			noise1.setFreq( noise1.getFreq()*0.5 );
			noise2.setFreq( noise2.getFreq()*0.5 );
			break;
			
		case 'd':
		{
			ofxGpuNoise::ShaderDerivType deriv1 = noise1.getShaderDerivType();
			noise1.setShaderDerivType( deriv1==0 ? ofxGpuNoise::SHADER_DERIV_TYPE_YES : ofxGpuNoise::SHADER_DERIV_TYPE_NO );
			ofxGpuNoise::ShaderDerivType deriv2 = noise2.getShaderDerivType();
			noise2.setShaderDerivType( deriv2==0 ? ofxGpuNoise::SHADER_DERIV_TYPE_YES : ofxGpuNoise::SHADER_DERIV_TYPE_NO );
		}
		default:
			break;
	}
}


