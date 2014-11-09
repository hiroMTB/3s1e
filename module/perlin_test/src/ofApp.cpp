#include "ofApp.h"

void ofApp::setup(){
    ofSetWindowTitle( "ofxGpuPerlinSample" );
    
    int w = 512;
    int h = 512;
    
    noise1.setup();
    noise1.setOctaves( 4 );
    noise1.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    noise1.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_NO );
    noise1.setSendSamplingPoints( true );
    noise1.setSamplingPointsScale( 0.001 );
    noise1.create( 256, 256 );
    
    noise2.setup();
    noise2.setOctaves( 4 );
    noise2.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    noise2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_NO );
    noise2.setSendSamplingPoints( true );
    noise2.setSamplingPointsScale( 0.01 );
    noise2.create( 256, 256 );

    noise3.setup();
    noise3.setOctaves( 2 );
    noise3.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    noise3.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_NO );
    noise3.setSendSamplingPoints( true );
    noise3.setSamplingPointsScale( 0.001 );
    noise3.create( 256, 256 );

    noise4.setup();
    noise4.setOctaves( 4 );
    noise4.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    noise4.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_NO );
    noise4.create( 256, 256 );
}

void ofApp::update(){
    
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_POINTS );
    mesh.setUsage( GL_DYNAMIC_DRAW );
    
    noise1.setFrame( ofGetFrameNum() * 0.0001 );
    noise1.update();

    noise2.setFrame( ofGetFrameNum() * 0.0011 );
    noise2.update();

    noise3.setFrame( ofGetFrameNum() * 0.00103 );
    noise3.update();

    noise4.setFrame( ofGetFrameNum() * 0.00301 );
    noise4.update();

    ofPixels * pix1 = noise1.getNoiseData();
    ofPixels * pix2 = noise2.getNoiseData();
    ofPixels * pix3 = noise3.getNoiseData();
    ofPixels * pix4 = noise4.getNoiseData();

    int w = pix1->getWidth();
    int h = pix1->getHeight();
    int index = 0;
    for( int y=0; y<h; y++ ){
        for( int x=0; x<w; x++ ){
            ofColor c1 = pix1->getColor( x, y );
            mesh.addVertex( ofVec3f(x, c1.r-256, 0) * 2);
            ofFloatColor cf;
            cf.set( 1.0-(float)y/h*0.6, 0.7 );
            cf.setHsb( (float)y/h , 0.5, 0.8 );
            mesh.addColor( cf );

            ofColor c2 = pix2->getColor( x, y );
            noise1.setSamplingPoint( ofVec2f(c2.r, c2.g)*0.001, index );

            ofColor c3 = pix3->getColor( x, y );
            noise2.setSamplingPoint( ofVec2f(c3.r, c3.g)*0.0025, index );

            ofColor c4 = pix4->getColor( x, y );
            noise3.setSamplingPoint( ofVec2f(c4.r, c4.g)*0.51, index );

            index++;
        }
    }
}

void ofApp::draw(){
//    ofEnableAlphaBlending();
//    ofEnableAntiAliasing();
//    ofEnableSmoothing();
    
    ofBackground( 0 );
    ofSetColor( 255 );

    ofTranslate( 10, 0 );

    ofTranslate( 0, 10 );
    noise1.draw( 0, 0, 1);

    ofTranslate( 0, 300 );
    noise2.draw( 0, 0, 1 );

    ofTranslate( 0, 300 );
    noise3.draw( 0, 0, 1 );

    ofTranslate( 0, 300 );
    noise4.draw( 0, 0, 1 );

    cam.begin(); {
        ofPushMatrix();
        ofTranslate( 0, 0 );
        ofSetColor( 255 );
        glPointSize( 1 );
        mesh.draw();
        ofPopMatrix();
    } cam.end();
}

void ofApp::keyPressed(int key){
    switch ( key){
        case 'p':
            noise1.toggleUseFboReader();
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case OF_KEY_RIGHT:
        {
            int i = noise1.getShaderType();
            noise1.setShaderType((ofxGpuNoise::ShaderType)++i);
            break;
        }
        case OF_KEY_LEFT:
        {
            int i = noise1.getShaderType();
            noise1.setShaderType((ofxGpuNoise::ShaderType)--i);
            break;
        }
        
        case '2':
        {
            int i = noise2.getShaderType();
            noise2.setShaderType((ofxGpuNoise::ShaderType)++i);
            break;
        }
        case '1':
        {
            int i = noise2.getShaderType();
            noise2.setShaderType((ofxGpuNoise::ShaderType)--i);
            break;
        }

        case '4':
        {
            int i = noise3.getShaderType();
            noise3.setShaderType((ofxGpuNoise::ShaderType)++i);
            break;
        }

        case '3':
        {
            int i = noise3.getShaderType();
            noise3.setShaderType((ofxGpuNoise::ShaderType)--i);
            break;
        }

        
        case OF_KEY_UP:
            noise1.setFreq( noise1.getFreq()*2.0 );
            break;
            
        case OF_KEY_DOWN:
            noise1.setFreq( noise1.getFreq()*0.5 );
            break;

        case 'd':
        {
            ofxGpuNoise::ShaderDerivType deriv = noise1.getShaderDerivType();
            noise1.setShaderDerivType( deriv==0 ? ofxGpuNoise::SHADER_DERIV_TYPE_YES : ofxGpuNoise::SHADER_DERIV_TYPE_NO );
        }
        default:
            break;
    }
}

void ofApp::windowResized(int w, int h){

}

