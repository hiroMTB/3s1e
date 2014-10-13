#pragma once

#include "ofMain.h"
#include "ofxDna.h"

class LineAgent {
public:		
	ofxDna dna;
	ofVec3f pos;
    ofVboMesh trail;
    ofFloatColor color;
    float noise_amp;
    float g[10];
    
	void setup(ofVec3f &loc) {
		pos = loc;
		dna.setup( 10 );
        trail.setUsage( GL_DYNAMIC_DRAW );
        trail.setMode( OF_PRIMITIVE_LINE_STRIP );

        color.set( 0 );
        noise_amp = 100.0;
	}
    
	void add_result() {
        for(int i=0; i<10; i++ ){
            g[i] = dna.getGene( i );
        }

        ofVec3f new_pos = pos + ofVec3f( g[0]*8.0, (g[1]-0.5)*4.0, (g[2]-0.5)*3 );
        pos = pos*0.6 + new_pos*0.4;
//		pos.rotate( g[7]*0.25, ofVec3f(1, 0, 0) );
        trail.addVertex( pos );
        color.setHsb( g[3]*0.7+0.3, g[4]*0.5+0.1, g[5]*0.7 );
        color.a = g[5];
        trail.addColor( color );
    }

    void animate() {
        float t = ofGetFrameNum();
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );
            //p.y += ofSignedNoise(p.x*100.0, p.y*100.0, t) * noise_amp;

            float rate = (float)i * i * 0.00001;
            rate = 1;
            p.x += ( g[0]-0.5 ) * rate;
            p.y += ( g[1]-0.5 ) * rate;
            p.z += ( g[2]-0.5 ) * rate;;
            trail.setVertex( i, p );
            
            ofFloatColor c = trail.getColor( i );
            c.r += ( g[3]-0.5 ) * 0.01;
            c.g += ( g[4]-0.5 ) * 0.01;
            c.b += ( g[5]-0.5 ) * 0.01;
            c.a += ( g[6]-0.5 ) * 0.01;
            c.setSaturation( 0.1 );
            trail.setColor( i, c );
        }
    }
    
    
    void animate_noise( int agent_id ){
        float t = ofGetFrameNum()*0.003;
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );
            float fbm = 0;
            int octave = 4;
            float amp = 0.5;
            ofVec2f pos( p.x, p.y );
            pos *= 0.001;
            for( int i=0; i<octave; i++ ){
                fbm += ofSignedNoise( pos.x, pos.y, t ) * amp;
                pos *= 2.0;
                amp *= 0.5;
            }
            
            fbm *= 2.0;
            
            p.y += fbm;
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
    bool bAnimate;
    bool bRotate;
    bool bOrtho;
    int num_agent;
	vector<LineAgent> la;
};
