//
//  constraint_line.cpp
//  constraint_1
//
//  Created by hiroshi matoba on 12/3/14.
//
//

#include "constraint_line.h"

constraint_line::constraint_line( ofxBulletWorldRigid * _world, ofVec3f _start, ofVec3f _end, int _num, float _length, float _random_factor )
:
world(_world),
start(_start),
end(_end),
num(_num),
length(_length),
random_factor(_random_factor)
{
    points.setUsage(GL_DYNAMIC_DRAW);
    points.setUsage( OF_PRIMITIVE_POINTS );
    lines.setUsage(GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

    ofVec3f dir = end - start;
    ofVec3f adder = dir / (float)num;
    norm = dir.cross(ofVec3f(0,0,0) ).normalized();
    
    for (int i=0; i<num; i++) {
        
        // calc position
        ofVec3f pivot = start + adder;
        pivot_pos.push_back( pivot );
        
        ofVec3f particle = pivot + norm*length;
        particle.rotate( 30, dir.normalized() );
        particle_pos.push_back( particle );
        
        // collision shape
        float mass = ofRandom(1, 10);
        float size = ofRandom(1, 10);
        ofxBulletSphere * s = new ofxBulletSphere();
        s->create(world->world, particle, mass, size);
        s->add();
        shapes.push_back( s );
        
        
        // constraint
        ofxBulletJoint * j = new ofxBulletJoint();
        j->create(world->world, s, pivot );
        j->add();
    }
    
}

void constraint_line::update(){
    
    for (int i=0; i<num; i++) {
        ofVec3f p = shapes[i]->getPosition();
        particle_pos[i] = p;
        // pivot_pos[i] = ??
        joints[i]->updatePivotPos( pivot_pos[i], 1);
    }
}

void constraint_line::draw(){
    
    ofSetColor(255, 0, 0);
    ofLine(start, end);

    ofVec3f mid = (end-start)*0.5;
    ofLine(mid, mid+norm);
 
    points.clear();
    points.addVertices( particle_pos );
    points.draw();
    
    lines.clear();
    for( int i=0; i<num; i++){
        lines.addVertex( pivot_pos[i ]);
        lines.addVertex( particle_pos[i] );
    }
    lines.draw();
}
