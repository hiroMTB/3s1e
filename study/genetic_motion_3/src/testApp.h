#pragma once

#include "ofMain.h"
#include "ofxDna.h"

class LineAgent {

public:
	ofxDna dna;
	float posy, speed, accel;
	ofFloatColor color;
	static int anim_type;
	
	void setup() {
		dna.setup( 6 );
        color.set(0);
		posy = speed = accel = 0;
	}
	
	void update( float amp ) {
        float g0 = dna.getGene( 0 );
        float g1 = dna.getGene( 1 );
        float g2 = dna.getGene( 2 );
        float g3 = dna.getGene( 3 );
        float g4 = dna.getGene( 4 );
		
		g0 = (g0-0.5) * 2.0 * amp;
		
		switch ( anim_type ) {
			case 0:
//				posy = posy*0.9 + g0*0.1;
                posy = g0;
				break;
			case 1:
			{
				speed = g0;
				float newy = posy + speed*12.0;
				posy = 0.5*posy + 0.5*newy;
				break;
			}
			case 2:
			{
				accel = g0;
				speed += accel;
				float newy = posy + speed*33.0;
				posy = 0.5*posy + 0.5*newy;
				break;
			}
		}
		
        color.setHsb( g1*0.7+0.3, g2*0.5+0.1, g3*0.9 );
		color.a = 1;
	}
};

class testApp : public ofBaseApp{

public:

	testApp();
	void setup();
	void update();
	void draw();

	void keyPressed  ( int key );
	void resized( int w, int h );
	
	static bool bStart;
    static const int num_agent = 1440;

	float amp;
	LineAgent la[num_agent];
	ofVboMesh points;
    
    
    vector<float> snd_buffer;
    
//    const int sampleRate = 44100;           //Sample rate of sound
//    const float duration = 0.25;            //Duration of the recorded sound in seconds
//    const int N = duration * sampleRate;    //Size of the PCM buffer
	
};
