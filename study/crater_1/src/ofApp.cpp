#include "ofApp.h"

void ofApp::setup(){

	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    extrusionAmount = 80.0;
    res = 4;
    make_quad( "crater1.jpg" );
    cam.setDistance( 1000 );
}

void ofApp::make_line( string file_name ){
    img.loadImage( file_name );

    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    mainMesh.setMode( OF_PRIMITIVE_LINES );

    int width = img.getWidth() / res;
    int height = img.getHeight() / res;
    
    for( int y=0; y<height-1; y++ ){
        for( int x=0; x<width; x++ ){
            ofVec2f index1( x*res, y*res );
            ofVec2f index2( (x+1)*res, y*res );
            ofFloatColor c1 = img.getPixelsRef().getColor( index1.x, index1.y );
            ofFloatColor c2 = img.getPixelsRef().getColor( index2.x, index2.y );
    
            float b1 = c1.getBrightness();
            float b2 = c2.getBrightness();
            float z1 = b1 * extrusionAmount;
            float z2 = b2 * extrusionAmount;
            mainMesh.addVertex( ofVec3f(index1.x, index1.y, z1) );
            mainMesh.addVertex( ofVec3f(index2.x, index2.y, z2) );

            float a1 = b1>0.3 ? 1.0 : 0.7;
            float a2 = b2>0.3 ? 1.0 : 0.7;
            mainMesh.addColor( ofFloatColor(0.7-b1, a1) );
            mainMesh.addColor( ofFloatColor(0.7-b2, a2) );
        }
    }
}

void ofApp::make_quad( string file_name ){
    img.loadImage( file_name );
    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    mainMesh.setMode( OF_PRIMITIVE_TRIANGLES );

    int width = img.getWidth() / res;
    int height = img.getHeight() / res;
    
    for( int y=0; y<height; y++ ){
        for( int x=0; x<width; x++ ){

            ofFloatColor c = img.getPixelsRef().getColor( x*res, y*res );
            float b = c.getBrightness();
            float z = b * extrusionAmount;
            mainMesh.addVertex( ofVec3f(x*res, y*res, z) );
            
            float a = b>0.3 ? 1.0 : 0;
            mainMesh.addColor( ofFloatColor(1.0-b, a) );

            speed.push_back( ofVec3f(0,0,0) );
        }
    }
    
    // index
    for( int y=0; y<height-1; y++ ){
        for( int x=0; x<width-1; x++ ){
            mainMesh.addIndex(x+y*width);				// 0
            mainMesh.addIndex((x+1)+y*width);			// 1
            mainMesh.addIndex(x+(y+1)*width);			// 10
            
            mainMesh.addIndex((x+1)+y*width);			// 1
            mainMesh.addIndex((x+1)+(y+1)*width);		// 11
            mainMesh.addIndex(x+(y+1)*width);			// 10
        }
    }
}

void ofApp::update(){
    float frame = ofGetFrameNum();
    
    for( int i=0; i<mainMesh.getNumVertices(); i++ ){
        ofVec3f p = mainMesh.getVertex( i );

        if( p.z < 0 )
            continue;
        
        int w = img.getWidth() / res;
        ofVec2f sp( i%w, i/w );
        
        ofVec3f noise;
        int octave = 4;
        float amp = 0.5;
        sp *= 0.01;
        
        for( int i=0; i<octave; i++ ){
//            noise.x += ofSignedNoise( sp.x, frame*0.001 ) * amp * 0.1;
//            noise.y += ofSignedNoise( sp.y, frame*0.001 ) * amp * 0.1;
            noise.z += ofNoise( sp.x, sp.y, frame*0.001 ) * amp;
            sp *= 2.0;
            amp *= 0.5;
        }

        p -= noise*2.0;
        mainMesh.setVertex( i, p );
        
        float a = p.z>40 ? 1 : 0.7;
        mainMesh.setColor( i , ofFloatColor(p.z/extrusionAmount, a) );
    }
}

void ofApp::draw(){
	ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground( 255 );

//    cam.enableOrtho();
    
    cam.begin();
    ofTranslate( -img.getWidth()/2, - img.getHeight()/2 );
    glPointSize( 1 );
    glLineWidth( 1 );
    mainMesh.drawWireframe();
	cam.end();
	
	ofSetColor( 255 );
	string msg = "fps: " + ofToString(ofGetFrameRate(), 2 );
	ofDrawBitmapString( msg, 10, 20 );
	
}

void ofApp::keyPressed( int key ){
	switch(key) {
        case 'a':
        {
            int i = ofRandom(1, 7);
            make_quad("crater" + ofToString(i)+ ".jpg");
            break;
        }
		case 'f':
			ofToggleFullscreen();
			break;
            
        case OF_KEY_UP:
            extrusionAmount += 10;
            break;

        case OF_KEY_DOWN:
            extrusionAmount -= 10;
            break;
	}
}
