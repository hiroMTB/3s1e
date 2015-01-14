#include "ofApp.h"

void ofApp::setup(){

    ofSetWindowTitle( "ofxGpuPerlinSample" );
    ofSetFrameRate( 60 );
    
    w = 512;
    h = 512;

	noise.setup();
    noise.setOctaves( 4 );
    noise.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    noise.setSendSamplingPoints( true );
    noise.setSamplingPointsScale( 0.001 );
    noise.create( w, h );

    mesh.clear();
    vector<ofVec3f> ps;
    ps.assign(w*h, ofVec3f(0,0,0) );
    vector<ofFloatColor> cs;
    cs.assign(w*h, ofFloatColor(0) );
    mesh.addVertices( ps );
    mesh.addColors( cs );
    mesh.setMode( OF_PRIMITIVE_POINTS );
    mesh.setUsage( GL_DYNAMIC_DRAW );
}

void ofApp::update(){
    
    noise.setFrame( ofGetFrameNum() * 0.01 );
    noise.update();

    vector<ofVec3f> &vs = mesh.getVertices();
    vector<ofFloatColor> &cs = mesh.getColors();
    unsigned char * pix = noise.getNoiseData();

    /*
     *      this loop could be around 262,144( when you use 512*512 noise)
     *      Be carefull how to get/set all pixel data
     */
	int index = 0;
    for( int y=0; y<h; y++ ){
        for( int x=0; x<w; x++ ){
			unsigned char r = pix[index*3 + 0];
			unsigned char g = pix[index*3 + 1];
			unsigned char b = pix[index*3 + 2];
			unsigned char avg = (r + g + b) / 3.0;
			vs[index].set(x, avg+128+y-h,0);
			ofVec2f &sp = noise.getSamplingPoints( index );
            cs[index].set( sp.x, sp.y, 0 );
            index++;
        }
    }
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    
    
    
    ofBackground( 0 );
    ofSetColor( 255 );
	
    ofPushMatrix(); {
        ofTranslate(20, ofGetHeight()/2-h/2 );
        
        ofPushMatrix(); {
            noise.draw( 0, 0, 1);
        } ofPopMatrix();
        
        ofPushMatrix(); {
            ofTranslate( w + 20, 0 );
            noise.draw_samplingPointTexture( 0, 0, 1 );
        }ofPopMatrix();
        
        ofPushMatrix();{
            ofTranslate( w*2+40, h/2 );
            ofSetColor( 255 );
            glPointSize( 1 );
            mesh.draw();
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
            noise.toggleUseFastFboReader();
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

