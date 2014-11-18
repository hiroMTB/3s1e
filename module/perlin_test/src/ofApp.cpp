#include "ofApp.h"

void ofApp::setup(){
    ofSetWindowTitle( "ofxGpuPerlinSample" );
    
    w = 512;
    h = 512;
    
    noise.setup();
    noise.setOctaves( 4 );
    noise.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_NO );
    noise.setSendSamplingPoints( true );
    noise.setSamplingPointsScale( 0.001 );
    noise.create( w, h );
}

void ofApp::update(){
    
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_POINTS );
    mesh.setUsage( GL_DYNAMIC_DRAW );
    
    noise.setFrame( ofGetFrameNum() * 0.01 );
    noise.update();

    ofPixels * pix = noise.getNoiseData();

	int w = pix->getWidth();
    int h = pix->getHeight();
    int index = 0;
    for( int y=0; y<h; y++ ){
        for( int x=0; x<w; x++ ){
            ofColor c1 = pix->getColor( x, y );
            mesh.addVertex( ofVec2f(x, c1.r+128+y-h) );
			ofFloatColor cf;
			ofVec2f sp = noise.getSamplingPoints( index );
            cf.set( sp.x, sp.y, 0 );
            mesh.addColor( cf );
            index++;
        }
    }
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    
    ofBackground( 0 );
    ofSetColor( 255 );
	
	ofPushMatrix();
	ofTranslate(20, ofGetHeight()/2-h/2 );
	
	ofPushMatrix();
    noise.draw( 0, 0, 1);
	ofPopMatrix();
	
	ofPushMatrix();
    ofTranslate( w + 20, 0 );
	noise.draw_samplingPointTexture( 0, 0, 1 );
	ofPopMatrix();
	
	ofPushMatrix();{
		ofTranslate( w*2+40, h/2 );
		ofSetColor( 255 );
		glPointSize( 1 );
		mesh.draw();
	}ofPopMatrix();
}

void ofApp::keyPressed(int key){
    switch ( key){
        case 'p':
            noise.toggleUseFboReader();
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case OF_KEY_RIGHT:
        {
            int i = noise.getShaderType();
            noise.setShaderType((ofxGpuNoise::ShaderType)++i);
            break;
        }
        case OF_KEY_LEFT:
        {
            int i = noise.getShaderType();
            noise.setShaderType((ofxGpuNoise::ShaderType)--i);
            break;
        }
        
        case OF_KEY_UP:
            noise.setFreq( noise.getFreq()*2.0 );
            break;
            
        case OF_KEY_DOWN:
            noise.setFreq( noise.getFreq()*0.5 );
            break;

        case 'd':
        {
            ofxGpuNoise::ShaderDerivType deriv = noise.getShaderDerivType();
            noise.setShaderDerivType( deriv==0 ? ofxGpuNoise::SHADER_DERIV_TYPE_YES : ofxGpuNoise::SHADER_DERIV_TYPE_NO );
        }
        default:
            break;
    }
}

void ofApp::windowResized(int w, int h){

}

