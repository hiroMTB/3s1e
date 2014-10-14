#include "testApp.h"

void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	
	string path = "HY_Emitter01.abc";
	abc.open(path);
    abc.dumpNames();
    
    cam.setDistance( 10 );
    cam.setNearClip( 0.01 );
    cam.setFarClip( 1000 );

    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );

    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
    time = 0;
}

void testApp::update(){
	time += 0.1;
    if( abc.getMaxTime() < abc.getTime() ){
        time = 0;
    }
    abc.setTime( time );

    vector<ofVec3f> pos;
    abc.get( 0, pos );

    points.clear();
    
    for( int i=0; i<pos.size(); i++ ){
        ofVec3f &p = pos[i];
        if( p.y>2.68 ){
            points.addVertex( p );
            points.addColor( ofFloatColor(ofRandom(0.3,0.8), 0.2) );
        }
    }
    
    lines.clear();
    int num_p = points.getNumVertices();
    int line_num = 1000;
    if( pos.size()!=0 && num_p>2 ){
        for( int i=0; i<line_num; i++ ){
            
            int index1 = floor( ofRandom( 0, num_p ));
            int index2 = floor( ofRandom( 0, num_p ));
            
            ofVec3f p1 = points.getVertex( index1 );
            ofVec3f p2 = points.getVertex( index2);
            
            float dist = p1.distance( p2 );
            if( ofRandom(1.0)>0.9){
                dist *= 0.4;
            }
            
            if( 0.03<dist && dist<0.5 ){
                
                lines.addVertex( p1 );
                lines.addVertex( p2 );
                
                ofFloatColor c1, c2;
                c1.white; c2.white;
                c1.setHsb( ofRandom(1.0), 0.3, 0.6, 0.6 );
                c2.setHsb( ofRandom(1.0), 0.3, 0.6, 0.6 );
                lines.addColor( c1 );
                lines.addColor( c2 );
            }
        }
    }
}

void testApp::draw(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    ofBackground( 255 );
	cam.begin();
    
	glPointSize(1);
    
    ofPushMatrix();
    ofSetColor( 20 );
    glPointSize( 2 );
    points.draw( OF_MESH_POINTS );

    glLineWidth( 1 );
    lines.drawWireframe();
    ofPopMatrix();
    
    cam.end();
    
    ofSetColor( 0 );
    ofDrawBitmapString( "point size: " + ofToString( points.getNumVertices() ), 10, 20 );
    ofDrawBitmapString( "time : " + ofToString( abc.getTime() ), 10, 40 );
}

void testApp::keyPressed( int key ){
    
}

void testApp::exit(){
    abc.close();
}
