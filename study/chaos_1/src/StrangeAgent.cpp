//
//  StrangeAgent.cpp
//  spiroLive
//
//  Created by mtb on 4/30/14.
//
//

#include "StrangeAgent.h"
#include "ofApp.h"

StrangeAgent::StrangeAgent()
:
mute( false ),
head( 0 )
{
    a = b = c = d = e = f = 0;
}

void StrangeAgent::init( float _a, float _b, float _c, float _d, float _e, float _f ){
    a = _a; b = _b; c = _c; d = _d; e = _e; f = _f;
    float x, y, z;
    x = y = z = 0.1;
    
    points.clear();
    
    for(int i=0; i<dotNumMax; i++){
        float newx = z*sin(a*x) + cos(b*y);
        float newy = x*sin(c*y) + cos(d*z);
        float newz = y*sin(e*z) + cos(f*z);
        points.addVertex( ofVec3f(newx, newy, newz) );
        points.addColor( ofFloatColor(0) );
    }
    
    int sampleRate = 44100;
    phase = ofRandom(-TWO_PI, TWO_PI);
    freq = ofRandom(0.0001, 20000.0);
    phaseAdder = (freq / (float)sampleRate) * TWO_PI;
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
}

void StrangeAgent::setRandom(){
    float scale = 1;
    init(ofRandom(-1,1)*scale, ofRandom(-1,1)*scale,ofRandom(-1,1)*scale,ofRandom(-1,1)*scale,ofRandom(-1,1)*scale,ofRandom(-1,1)*scale );
}

void StrangeAgent::update( float * ret ){
    if( mute) return;
    
    //head = points.getNumVertices() - 1;
    ofVec3f p = points.getVertex( head );
    
    phase += phaseAdder;
    
    // starange attractor 1
    float newx = p.z*sin(a*p.x) + cos(b*p.y); // + sin1+sin(phase2);
    float newy = p.x*sin(c*p.y) + cos(d*p.z); // + cos1+cos(phase2);
    float newz = p.y*sin(e*p.z) + cos(f*p.x);
    
    // low pass filter
    float rate = 0.0;
    float rate2 = 1.0 - rate;
    newx = rate*p.x + rate2*newx;
    newy = rate*p.y + rate2*newy;
    newz = rate*p.z + rate2*newz;
    
    if( ret != NULL ){
        ret[0] = newx;
        ret[1] = newy;
        ret[2] = newz;
    }

    if( 1 ){
        head++;
        if( head>=dotNumMax )
            head=0;
        points.setVertex( head, ofVec3f(newx, newy, newz) );
        points.setColor( head, ofFloatColor( 0 ) );
    }
    
    //points.addVertex( ofVec3f(newx, newy, newz) );
    //points.addColor( ofFloatColor( 0 ) );
}

void StrangeAgent::draw(){
    if(mute) return;

    points.draw();

}

