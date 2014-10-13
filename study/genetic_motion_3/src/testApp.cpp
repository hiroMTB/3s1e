#include "testApp.h"

/*
 *
 *		line agent class
 *
 */

int LineAgent::anim_type = 0;

/*
 *
 *		testApp class
 *
 */
bool testApp::bStart = false;

testApp::testApp(){
}

void testApp::setup(){
	ofSetFrameRate( 60 );

	amp = 0.2;
	
	points.setUsage( GL_DYNAMIC_DRAW );
	points.setMode( OF_PRIMITIVE_LINE_STRIP );
	
	for ( int i=0; i<num_agent; i++ ) {
		la[i].setup();
	}
}

void testApp::update(){
//	if(bStart){
		points.clear();
		
		for ( int i=0; i<num_agent;i++ ) {
			
			bool mutate = ofGetKeyPressed() || i==0 || ofRandom(1.0)<0.03;
			
			if( mutate ){
				la[i].dna.setBoundsMode( i%3 );
				la[i].dna.mutate(  ofMap(mouseX, 0, ofGetWidth(), 0, 0.5) );
			}else{
				la[i].dna.setBoundsMode(i%3);
				la[i].dna.setMateMode(floor(ofRandom(0,4)));
				la[i].dna.mate( la[i-1].dna, ofMap(mouseX, 0, ofGetWidth(), 0, 1.0) );
			}

			la[i].update( amp );
			
			points.addVertex( ofVec2f( i, la[i].posy*ofGetHeight()) );
			points.addColor( la[i].color );
            
            
            snd_buffer.push_back( la[i].posy );
		}
//	}
}

void testApp::draw(){
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
	ofBackground( 0 );

//	ofSetupScreenOrtho();
    ofSetColor( 255 );
	glPointSize( 1 );
	glLineWidth( 1 );
	ofPushMatrix();
	ofTranslate( 0, ofGetHeight()/2 );
	points.draw( OF_MESH_POINTS );
	ofPopMatrix();
}

void testApp::keyPressed( int key ){
	
	if( key == ' ' ) {
		
		bStart = !bStart;
		
	}else if( key == 'f') {
		
		ofToggleFullscreen();

	}else if( key == '0' ){
		
		LineAgent::anim_type = 0;
		amp = 0.1;
		
	}else if( key == '1' ){

		LineAgent::anim_type = 1;
		amp = 0.0001;
		
	}else if( key == '2' ){
		
		LineAgent::anim_type = 2;
		amp = 0.0000003;

    }else if ( key == 's' ){
        /*
         *
         *      SOUND REC
         *
         */
        // "wb" = open binary file for writing
        string path = ofToDataPath( "sound.raw" );
        FILE * file = fopen( path.c_str(), "wb" );
        //Write the buffer into file
        fwrite( &snd_buffer[0], snd_buffer.size(), sizeof( snd_buffer[0] ), file );
        
        //Close the file
        fclose( file );
    }
}

void testApp::resized( int w, int h ){

}
