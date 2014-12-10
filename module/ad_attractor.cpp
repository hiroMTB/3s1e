//
//  ad_attractor.cpp
//  constraint_2
//
//  Created by mtb on 10/12/14.
//
//

#include "ad_attractor.h"

bool ad_attractor::init = false;
ofVboMesh ad_attractor::points;

ad_attractor::ad_attractor( ofVec3f _pos, float _power, float _speed )
:
init_pos( _pos ),
power( _power ),
speed( _speed )
{
	if( !init ){
		points.setUsage( GL_DYNAMIC_DRAW );
		points.setMode( OF_PRIMITIVE_POINTS );
		init = true;
	}
	
	points.addVertex( _pos );
	id = points.getNumVertices()-1;
}


void ad_attractor::draw_all(){
	ofSetColor( 0,255,0 );
	points.draw();
}

ofVec3f ad_attractor::getPosition(){
	return points.getVertex( id );
}


const vector<ofVec3f> & ad_attractor::getPositions() const {
	return points.getVertices();
}

vector<ofVec3f> & ad_attractor::getPositions(){
	return points.getVertices();
}

