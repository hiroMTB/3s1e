//
//  lineAgent.cpp
//  genetic_motion_4.3
//
//  Created by mtb on 08/12/14.
//
//

#include "LineAgent.h"
#include "ofApp.h"

void LineAgent::setup( float _angle, float _length , int _group) {
	angle = _angle + ofSignedNoise(_angle*0.01, ofGetFrameNum()*0.001)*30.0;
	length = _length + ofSignedNoise(_angle*0.01, ofGetFrameNum()*0.02)*10.0;
	group = _group;
	
	pos.set( length, 0, 0 );
	
	dna.setup( 10 );
	color.set( 0 );
	noise_amp = 100.0;
	amp = 1.0;
	trail.setUsage( GL_DYNAMIC_DRAW );
	trail.setMode( OF_PRIMITIVE_POINTS );
	
	drawer.setUsage( GL_DYNAMIC_DRAW );
	drawer.setMode( OF_PRIMITIVE_LINES );
}

void LineAgent::add_result() {
	for(int i=0; i<10; i++ ){
		g[i] = dna.getGene( i );
	}
	
	float rate = ofNoise(ofGetFrameNum() * 0.01)*3.0;
	pos.x += g[0] * rate * 2.0;
	pos.y += ( g[1]-0.5 ) * 2.0 * rate;
	pos.z += ( g[2]-0.5 ) * 2.0 * rate;
	
	color.r += g[2] * 0.1;
	color.g += g[3] * 0.1;
	color.b += g[4] * 0.1;
	color.a = 1;
	
	if( ofRandomuf() > 0.95){
		float tl = ofMap(length, 400, 4000, 0.0, 1.0, true);
		float add_rate = tl;
		if( ofRandomuf() < add_rate*0.2 ){
			ofVec3f new_pos = pos.rotated( angle, ofVec3f(0,0,1) );
			new_pos += ofVec3f( g[2], g[3], g[4] )*0.5;
			trail.addVertex( new_pos );
			trail.addColor( color );
			speed.push_back(ofVec3f(0,0,0) );
		}
	}
}

void LineAgent::animate() {
	float t = ofGetFrameNum();
	for( int i=0; i<trail.getNumVertices(); i++ ){
		ofVec3f p = trail.getVertex( i );
		
		float rate = 0.7;
		p.x += ( g[0]-0.5 ) * rate;
		p.y += ( g[1]-0.5 ) * rate;
		p.z += ( g[2]-0.5 ) * rate;
		trail.setVertex( i, p );
		
		ofFloatColor c = trail.getColor( i );
		c.r += ( g[3]-0.5 ) * 0.01;
		c.g += ( g[4]-0.5 ) * 0.01;
		c.b += ( g[5]-0.5 ) * 0.01;
		
		if( i==0 ){
			trail.setColor( i, ofFloatColor( ofRandom(0.8, 1.0),0, ofRandom(0.01, 0.1), ofRandom(0.8,1.0)) );
		}else if( i==trail.getNumVertices()-1 ){
			trail.setColor( i, ofFloatColor(0.3, ofRandom(0.7,1.0)) );
		}else{
			trail.setColor( i, c );
		}
	}
}

void LineAgent::animate_noise_gpu( int agent_id ){
	float t = ofGetFrameNum()*0.003 * 3.0;
	for( int i=0; i<trail.getNumVertices(); i++ ){
		ofVec3f p = trail.getVertex( i );
		float fbm1 = ofApp::app->noise.getNoisef(1, i, 0);
		float fbm2 = ofApp::app->noise.getNoisef(1, i, 1);
		
		ofVec3f xaxis( 1, 0, 0 );
		xaxis.rotate( angle, ofVec3f(0,0,1) );
		speed[i] += xaxis*fbm1*0.0005 + ofVec3f( fbm1, fbm2, 0 ) * 0.0005;
		p += speed[i];
		trail.setVertex( i, p );
	}
}

void LineAgent::animate_noise( int agent_id ){
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

void LineAgent:: draw() {
	trail.draw();
}

void LineAgent::draw_connection_inside_of_agent(){
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

