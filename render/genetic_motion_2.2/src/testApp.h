#pragma once

#include "ofMain.h"
#include "ofxDna.h"
#include "ad_image_saver.h"

class LineAgent {
public:		

    int agent_id;
    float angle;
    float radius;
    float max_x;
    ofxDna dna;
	ofVec3f pos;
    ofVboMesh trail;
    ofFloatColor color;
    float g[10];
    
	void setup(ofVec3f &center, float _radius, int _agent_id, float _max_x ) {
        radius = _radius;
		pos = center;
        agent_id = _agent_id;
        max_x = _max_x;
        pos += ofVec3f( agent_id, 0, 0 );
        float percentile = pos.x / max_x;
        angle = 360.0 * percentile;

        dna.setup( 10 );
        trail.setUsage( GL_DYNAMIC_DRAW );
        trail.setMode( OF_PRIMITIVE_LINE_STRIP );
        color.set( 0 );
	}
    
    void update_agent(){
        for(int i=0; i<10; i++ ){
            g[i] = dna.getGene( i );
        }
    }
    
	void add_result() {

        ofVec3f past_pos( 0,0,0 );
        if( trail.getNumVertices() !=0 )
            past_pos = trail.getVertex( trail.getNumVertices()-1 );
        
        pos = pos + ofVec3f( g[0]*6.0, (g[1]-0.5), 0 );

        float percentile = pos.x / max_x;
        float height = pos.y;
        ofVec3f bend_pos( radius,0,0 );
        bend_pos.rotate( angle, ofVec3f(0,0,1) );
        bend_pos += bend_pos.normalized() * height;
        
        bend_pos = past_pos*0.05 + bend_pos*0.95;
        
        trail.addVertex( bend_pos );
        color.setHsb( g[3]*0.7+0.3, g[4]*0.5+0.1, g[5]*0.7 );
        color.a = g[5]*0.5;
        trail.addColor( color );
    }

    void animate() {
        float t = ofGetFrameNum();
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );

            float pos_rate = 1;
            p.x += ( g[0]-0.5 ) * pos_rate;
            p.y += ( g[1]-0.5 ) * pos_rate;
            trail.setVertex( i, p );
            
            float col_rate = 0.001;
            ofFloatColor c = trail.getColor( i );
            c.r += ( g[3]-0.5 ) * col_rate*0.3;
            c.g += ( g[4]-0.5 ) * col_rate;
            c.b += ( g[5]-0.5 ) * col_rate;
            c.a += ( g[5]-0.5 ) * col_rate;
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
                fbm2 += ofSignedNoise( pos.y, pos.x, t ) * amp;
                pos *= 2.0;
                amp *= 0.5;
            }
            
            fbm1 *= 2.0;
            fbm2 *= 2.0;
            p.x += fbm1;
            p.y += fbm2;
            
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

    vector<LineAgent> la;
    
    ofVboMesh lines;
    
    ad_image_saver saver;
};
