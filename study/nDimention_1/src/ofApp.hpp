#pragma once

#include "ofMain.h"
#include "ofxGpuNoise.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
    
	vector<ofVec3f> units;
    ofVboMesh mesh;
    vector<ofVec3f> speed;
    vector<ofVec3f> accel;
    ofxGpuNoise gn1, gn2;
};


void ofApp::setup(){
	
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel( OF_LOG_VERBOSE );
    
    mesh.setUsage( GL_DYNAMIC_DRAW );
    mesh.setMode( OF_PRIMITIVE_POINTS );
	units.assign( 6, ofVec3f(0,0,0) );
	units[0].set(  10,  -30, 0 ); units[0].normalize();
	units[1].set( -60,   -3, 0 ); units[1].normalize();
	units[2].set( 112, -110, 0 ); units[2].normalize();
	units[3].set( -10,  -90, 0 ); units[3].normalize();
    units[4].set( 162, -2, 0 );  units[4].normalize();
    units[5].set( 145,  -30, 0 ); units[5].normalize();

    int n = 12;
    int numv = n*n*n*n*n*n;
    int size = ceil( sqrt(numv) );
    gn1.create(size, size );
    gn1.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    gn1.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn1.setFreq( 0.003 );

    gn2.create( size, size );
    gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn2.setFreq( 0.001 );

    for( int a0=0; a0<n; a0++){
        for( int a1=0; a1<n; a1++){
            for( int a2=0; a2<n; a2++){
                for( int a3=0; a3<n; a3++){
                    for( int a4=0; a4<n; a4++){
                        for( int a5=0; a5<n; a5++){

                            ofVec3f v(0,0,0);
                            v += units[0] * a0;
                            v += units[1] * a1;
                            v += units[2] * a2;
                            v += units[3] * a3;
                            v += units[4] * a4;
                            v += units[5] * a5;
                            mesh.addVertex( v * 40.0 );
                        }
                    }
                }
            }
        }
    }

    vector<ofVec3f> vs;
    vs.assign(size*size, ofVec3f(0,0,0) );
    accel = vs;
    speed = vs;
    
    vector<ofFloatColor> cs;
    cs.assign(size*size, ofFloatColor(0.2, 0.5) );
    mesh.addColors( cs );
}


void ofApp::update(){
    float frame = ofGetFrameNum();
    gn1.setFrame( frame * 0.0006 );
    gn2.setFrame( frame * 0.003 );
    
    gn1.update();
    gn2.update();
    unsigned char * noise1 = gn1.getNoiseData();
    unsigned char * noise2 = gn2.getNoiseData();
    
    vector<ofVec3f>      &vs = mesh.getVertices();
    vector<ofFloatColor> &cs = mesh.getColors();
    for( int i=0; i<vs.size(); i++ ){
        int index = i*3;
        float r1 = noise1[index + 0]/255.0 - 0.5;
        float g1 = noise1[index + 1]/255.0 - 0.5;
        float b1 = noise1[index + 2]/255.0 - 0.5;
        float r2 = noise2[index + 0]/255.0 - 0.5;
        float g2 = noise2[index + 1]/255.0 - 0.5;
        float b2 = noise2[index + 2]/255.0 - 0.5;
        
        accel[i] =  units[0] * r1;
        accel[i] += units[1] * g1;
        accel[i] += units[2] * b1;
        accel[i] += units[3] * r2;
        accel[i] += units[4] * g2;
        accel[i] += units[5] * b2;

        speed[i] = accel[i];
        vs[i] += speed[i];
        
        cs[i].r += r1 * 0.01;
        cs[i].g += g2 * 0.01;
        cs[i].b += b2 * 0.01;
        
        gn1.setSamplingPoint( ofVec2f(r1, b2)*1001, i );
    }
}


void ofApp::draw(){
	ofBackground(255);
	
    ofSetupScreenOrtho();
	ofPushMatrix();
	ofTranslate( ofGetWidth()/2, ofGetHeight()-50 );

    mesh.draw();
    
    ofSetColor(255, 0, 0);
	ofSetLineWidth(1);
	ofVec3f zero(0,0,0);
	for( int i=0; i<units.size(); i++ ){
		ofLine(zero, units[i] * 1000.0);
	}
	
	ofPopMatrix();
    
    gn1.draw(10,  10, 0.2);
    gn2.draw(10, 150, 0.2);
    
}


void ofApp::keyPressed(int key){
	
}
