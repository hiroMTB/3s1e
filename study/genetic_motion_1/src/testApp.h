#pragma once

#include "ofMain.h"
#include "ofxDna.h"

class LineAgent {
public:		
	ofxDna dna;
	ofPoint origin;
	float rad, rad2;
	
	void setup( ofPoint &loc, float radius ) {
		origin = loc;
		dna.setup( 10*2 + 4 );
		rad = radius; 
		rad2 = rad/2;
	}	
	void display() {
        float r,g,b,a;
        
		for( int i=0; i<dna.getNum()-1; i+=2 ) {

            if( i==0 ){
                r = dna.getGene( i );
                g = dna.getGene( i+1 );
            }else if( i==2 ){
                b = dna.getGene( i );
                a = dna.getGene( i+1 );
                ofSetColor( ofFloatColor(r,g,b,a) );
            }else{
                float x = dna.getGene( i ) * rad + origin.x - rad2;
                float y = dna.getGene( i+1 ) * rad + origin.y - rad2;
                ofLine( origin.x, origin.y, x, y );
                ofRect( x, y, 2, 2 );
            }
		}
	}
};

class testApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();
	void keyPressed( int key );
    
    static const int num_agent = 400;
	LineAgent la[num_agent];
};
