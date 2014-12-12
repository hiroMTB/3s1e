//
//  ad_attract_line.cpp
//  constraint_2
//
//  Created by mtb on 10/12/14.
//
//

#include "ad_attract_line.h"

ad_attract_line::ad_attract_line( vector<ofxBulletBaseShape*> * _shapes, ofVec3f _start, ofVec3f _end, int _num, float _length, float _power, float _speed )
:
shapes(_shapes),
start(_start),
end(_end)
{
    points.setUsage( GL_DYNAMIC_DRAW );
	points.setMode( OF_PRIMITIVE_POINTS );
	
    ofVec3f dir = end - start;
    ofVec3f adder = dir / (float)_num;
    norm = dir.getCrossed(ofVec3f(20,20,1110) );
    norm.normalize();

    attrs.assign(_num, Attr() );
    
    for (int i=0; i<_num; i++) {
        
        attrs[i].power = _power;
        attrs[i].speed = _speed;
        attrs[i].length = _length;
        attrs[i].angle =  ofSignedNoise(norm.x, i)*120;
        
        // calc position
        attrs[i].pivot = start + adder*i;
        
        ofVec3f pos = norm * attrs[i].length;
        pos.rotate( attrs[i].angle , dir.normalized());
        pos += attrs[i].pivot;
        points.addVertex( pos );
        points.addColor( ofFloatColor(0,1,1) );
    }
}

void ad_attract_line::update(){
    vector<ofVec3f> & ps = points.getVertices();
    
    for(int a=0; a<ps.size(); a++ ){
        
        Attr & at = attrs[a];
        at.angle += at.speed;

        // Rotate
        ofVec3f dir = end - start;
        ofVec3f pos = norm * at.length;
        pos.rotate( at.angle, dir.normalized() );
        pos += at.pivot;
        ps[a] = pos;
        
        // Attract
        for(int i=0; i<shapes->size(); i++ ){
            ofVec3f p = shapes->at(i)->getPosition();
            
            // pull
            ofVec3f dir = (ps[a] - p);
            float dist2 = dir.lengthSquared();
            
            if( 30 < dist2 || dist2<100000 ){
                ofVec3f impl = dir.normalized() * (at.power / (dist2+0.0000000001) );
                shapes->at(i)->getRigidBody()->applyCentralImpulse( btVector3(impl.x, impl.y, impl.z) );
            }
        }
    }
}

void ad_attract_line::draw(){
    glPointSize(5);
	points.draw();
}

vector<ofVec3f> & ad_attract_line::getPositions(){
	return points.getVertices();
}

