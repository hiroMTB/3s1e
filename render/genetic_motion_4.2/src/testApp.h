#pragma once

#include "ofMain.h"
#include "ofxDna.h"
#include "ad_image_saver.h"

class LineAgent {
public:		
	ofxDna dna;
    float g[10];
    float noise_amp;

    ofVec3f pos;
    ofVec3f center_point;
    
    ofFloatColor color;
    ofVboMesh trail;
    ofVboMesh drawer;
    
	void setup( ofVec3f _center_point ) {
        center_point = _center_point;
        pos = center_point;
        
        dna.setup( 10 );
        color.set( 0 );
        noise_amp = 100.0;
        
        trail.setUsage( GL_DYNAMIC_DRAW );
        trail.setMode( OF_PRIMITIVE_POINTS );
        
        drawer.setUsage( GL_DYNAMIC_DRAW );
        drawer.setMode( OF_PRIMITIVE_LINES );
	}
    
	void add_result() {
        for(int i=0; i<10; i++ ){
            g[i] = dna.getGene( i );
        }

        float rate = 1;
        pos.x += g[0] * rate;
        pos.y += g[1] * rate;
        ofVec3f new_pos = pos + center_point;
        
        trail.addVertex( new_pos );
        color.setHsb( g[3]*0.5+0.3, g[4]*0.5, g[5]*0.5 );
        color.a = g[6] + 0.1;
        trail.addColor( color );
    }

    void animate() {
        float t = ofGetFrameNum();
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );

            p.x += g[0];
            p.y += g[1];
//            p.z += g[2]-0.5 ) * rate;
            trail.setVertex( i, p );
            
            ofFloatColor c = trail.getColor( i );
            c.r += g[3] * 0.002;
            c.g += g[4] * 0.002;
            c.b += g[5] * 0.002;
            c.a += g[6] * 0.002;
			
            if( i==0 ){
                trail.setColor( i, ofFloatColor( ofRandom(0.9,1.0),0,0, ofRandom(0.9,1.0)) );
            }else{
                trail.setColor( i, c );
            }
        }
    }
    
    
    void animate_noise( int agent_id ){
        float t = ofGetFrameNum()*0.003 * 3.0;
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );
            float fbm1 = 0;
            float fbm2 = 0;
            int octave = 4;
            float amp = 0.5;
            ofVec2f pos( p.x, p.y );
            pos *= 0.01;
            for( int i=0; i<octave; i++ ){
                fbm1 += ofSignedNoise( pos.x+ agent_id*0.001, pos.y, t ) * amp;
                fbm2 += ofSignedNoise( pos.x, pos.y, t*0.1 ) * amp;
                pos *= 2.0;
                amp *= 0.5;
            }
            
            ofVec3f noise(fbm1, fbm2, 0 );
            p += (noise * 6.0);
            trail.setVertex( i, p );
        }
    }
    
    void draw() {
        glLineWidth( 1 );
		glPointSize( 1 );
        trail.draw( OF_MESH_POINTS );
	}
    
    void draw_connection_inside_of_agent(){
        drawer.clear();
        int num_lines = 20;
        for( int i=0; i<num_lines; i++ ){
            int index1 = floor( ofRandom(0, trail.getNumVertices()) );
            int index2 = floor( ofRandom(0, trail.getNumVertices()) );
            
            if( trail.getNumVertices()>100 ){
                ofVec3f p1 = trail.getVertex( index1 );
                ofVec3f p2 = trail.getVertex( index2 );
                float d = p1.distance(p2);
                if( 10<d && d<40 ){
                    drawer.addVertex( p1 );
                    drawer.addVertex( p2 );
                    drawer.addColor( trail.getColor(index1)*0.1 );
                    drawer.addColor( trail.getColor(index2)*0.1 );
                }
            }
        }
        glLineWidth( 1 );
        drawer.drawWireframe();
    }
};

class testApp : public ofBaseApp{

public:

	testApp();
	void setup();
	void update();
	void draw();
    void draw_info();
    void draw_connection_between_agnet();
    void change_settings();
    
    void keyPressed ( int key );
	void resized( int w, int h );

    bool bDraw_info;
	bool bStart;
    bool bAnimate;
    bool bRotate;
    bool bOrtho;
    bool bDraw_connection_inside_of_agent;
    bool bDraw_connection_between_agnet;
    bool bDraw_agent;
    
    vector<LineAgent> la;
    
    ofVec3f center;
    ofVboMesh connection_between_agent;
    
    ad_image_saver saver;
};
