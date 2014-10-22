#pragma once

#include "ofMain.h"
#include "ofxDna.h"
#include "ad_image_saver.h"
#include "ofxVectorGraphics.h"

class LineAgent {
public:		
	ofxDna dna;
    float g[10];
    float angle;
    float length;
    float noise_amp;

    ofVec3f pos;
    
    ofFloatColor color;
    ofVboMesh trail;
    ofVboMesh drawer;
        
	void setup( float _angle, float _length ) {
        angle = _angle;
        length = _length;

        pos.set( length, 0, 0 );
        
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

        float rate = ofRandom( 0.1, 0.3 );
        pos.x += g[0] * rate * 2.0;
        pos.y += ( g[1]-0.5 ) * 2.0 * rate;
        pos.z += ( g[2]-0.5 ) * 2.0 * rate;
        
        ofVec3f new_pos = pos.rotated( angle, ofVec3f(0,0,1) );
        new_pos += ofVec3f( g[2], g[3], g[4] )*0.5;

        if( ofRandom(1.0)>0.99 ){
            color.r += g[2] * 0.1;
            color.g += g[3] * 0.1;
            color.b += g[4] * 0.1;
            color.a = 1;
            trail.addVertex( new_pos );
            trail.addColor( color );
        }
    }

    void animate() {
        float t = ofGetFrameNum();
        for( int i=0; i<trail.getNumVertices(); i++ ){
            ofVec3f p = trail.getVertex( i );

            float rate = rate = 1;
            p.x += ( g[0]-0.5 ) * rate;
            p.y += ( g[1]-0.5 ) * rate;
            p.z += ( g[2]-0.5 ) * rate;
            trail.setVertex( i, p );
            
            ofFloatColor c = trail.getColor( i );
            c.r += ( g[3]-0.5 ) * 0.002;
            c.g += ( g[4]-0.5 ) * 0.002;
            c.b += ( g[5]-0.5 ) * 0.002;
            
            if( i==0 ){
                trail.setColor( i, ofFloatColor(1,0,0, ofRandom(0.7,1.0)) );
            }else if( i==trail.getNumVertices()-1 ){
                trail.setColor( i, ofFloatColor(0.3, ofRandom(0.7,1.0)) );
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
            ofVec2f pos( agent_id%200, (int)(agent_id/200) );
            pos *= 0.001;
            for( int i=0; i<octave; i++ ){
                fbm1 += ofSignedNoise( pos.x, t ) * amp;
                fbm2 += ofSignedNoise( pos.y, t ) * amp;
                pos *= 2.0;
                amp *= 0.5;
            }
            
            ofVec3f xaxis( 1, 0, 0 );
            xaxis.rotate( angle, ofVec3f(0,0,1) );
            p += xaxis*fbm1 + ofVec3f( fbm1, fbm2, 0 );
            trail.setVertex( i, p );
        }
    }
    
    void draw() {
        trail.draw();
	}
    
    void draw_connection_inside_of_agent(){
        drawer.clear();
        int num_lines = 20;
        for( int i=0; i<num_lines; i++ ){
            int index1 = ofRandom(0, trail.getNumVertices()-1 );
            int index2 = ofRandom(0, trail.getNumVertices()-1 );
            
            if( trail.getNumVertices()>100 ){
                ofVec3f p1 = trail.getVertex( index1 );
                ofVec3f p2 = trail.getVertex( index2 );
                float d = p1.distance(p2);
                if( 5<d && d<15 ){
                    drawer.addVertex( p1 );
                    drawer.addVertex( p2 );
                    drawer.addColor( trail.getColor(index1)*0.1 );
                    drawer.addColor( trail.getColor(index2)*0.1 );
                }
            }
        }
        glLineWidth( 1 );
        drawer.draw();
    }
};

class ofApp : public ofBaseApp{

public:

	ofApp();
	void setup();
	void update();
	void draw();
    void draw_info();
    void draw_connection_between_agnet();
    void change_settings();
    void draw_vector_graphics();
    
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
    bool bSequencial_add;
    
    int num_agent;
    int current_setting_start_frame;
    float in_angle;
    float out_angle;
    float initial_radius;
    float sequencial_add_speed;
    vector<LineAgent> la;
    
    ofVec3f center;
    ofVec3f pivot;
    ofVboMesh connection_between_agent;
    ad_image_saver saver;
    ofVboMesh connection_between_pivot;
    
};
