#pragma once

#include "ofMain.h"
#include "ofxDna.h"
#include "ad_image_saver.h"

class LineAgent {
public:		

    float radius;
    
    ofxDna dna;
	ofVec3f pos;
    ofVboMesh trail;
    ofFloatColor color;
    float noise_amp;
    float g[10];
    
	void setup(ofVec3f &center, float _radius ) {
        radius = _radius;
		pos = center + ofVec3f( radius+ofRandom(-30,30), ofRandom(-30,30), 0 );
		dna.setup( 10 );
        trail.setUsage( GL_DYNAMIC_DRAW );
        trail.setMode( OF_PRIMITIVE_LINE_STRIP );
        color.set( 0 );
        noise_amp = 100.0;
	}
    
    void update_agent(){
        for(int i=0; i<10; i++ ){
            g[i] = dna.getGene( i );
        }
    }
    
	void add_result() {
        ofVec3f new_pos = pos + ofVec3f( g[0]*30, (g[1]-0.5)*5.0, 0 );
        pos = pos*0.3 + new_pos*0.7;

        float percentile = pos.x / ofGetWidth();
        float height = pos.y;
        float angle = 360.0 * percentile;
        ofVec3f bend_pos( radius,0,0 );
        bend_pos.rotate( angle, ofVec3f(0,0,1) );
        bend_pos += bend_pos.normalized() * height;
        
        trail.addVertex( bend_pos );
        color.setHsb( g[3]*0.7+0.3, g[4]*0.5+0.1, g[5]*0.7 );
        color.a = g[5]*0.5 + 0.05;
        trail.addColor( color );
    }

    void animate() {
        float t = ofGetFrameNum();
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );

            float rate = 0.5;
            p.x += ( g[0]-0.5 ) * rate;
            p.y += ( g[1]-0.5 ) * rate;
//            p.z += ( g[2]-0.5 ) * rate;;
            trail.setVertex( i, p );
            
            ofFloatColor c = trail.getColor( i );
            c.r += ( g[3]-0.5 ) * 0.01;
            c.g += ( g[4]-0.5 ) * 0.01;
            c.b += ( g[5]-0.5 ) * 0.01;
//            c.setSaturation( ofRandom(0.1, 0.2) );
            trail.setColor( i, c );
        }
    }
    
    
    void animate_noise( int agent_id ){
        float t = ofGetFrameNum()*0.003;
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );
            float fbm1 = 0;
            float fbm2 = 0;
            
            int octave = 4;
            float amp = 0.5;
            ofVec2f pos( p.x, p.y );
            pos *= 0.001;
            for( int i=0; i<octave; i++ ){
                fbm1 += ofSignedNoise( pos.x, pos.y, t ) * amp;
                fbm2 += ofSignedNoise( pos.x*0.1, pos.y*0.1, t ) * amp;
                pos *= 2.0;
                amp *= 0.5;
            }
            
            p.x += ( fbm1 );
            p.y += ( fbm2 );
            trail.setVertex( i, p );
        }
    }
    
    void draw() {
        glLineWidth( 1 );
		glPointSize( 1 );
        trail.draw( OF_MESH_WIREFRAME );
	}
};

class testApp : public ofBaseApp{

public:

	testApp();
	void setup();
	void update();
	void draw();
    void draw_info();
	void keyPressed ( int key );
	void resized( int w, int h );

    bool bDraw_info;
	bool bStart;
    bool bAdd_agent;
    int num_agent;
	vector<LineAgent> la;
    ad_image_saver saver;
};
