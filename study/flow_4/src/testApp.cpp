#include "testApp.h"

void testApp::setup(){
    bInit = false;
    ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
	
	string path = "HY_Emitter01.abc";
	abc.open(path);
    abc.dumpNames();
    
    cam.setDistance( 5 );
    cam.setNearClip( 0.00001 );
    cam.setFarClip( 10000 );

    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );

    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

}

void testApp::update(){

    float frame = ofGetFrameNum();
    float time = frame * 0.03333333;
    abc.setTime( time );

    vector<ofVec3f> pos;
    abc.get( 0, pos );


    if( !bInit && pos.size()>0 ) {
        for( int i=0; i<pos.size(); i++ ){
            initial_pos.push_back( pos[i] );
        }
        bInit = true;
    }

    points.clear();
    for( int i=0; i<pos.size(); i++ ){
        ofVec3f p = pos[i] - initial_pos[i];
        ofVec3f xA( i*0.000003,0,0 );
//        xA.rotate( (float)i*0.2, ofVec3f(0,0,1) );
        
        points.addVertex( p + xA );
        points.addColor( ofFloatColor(ofNoise(i, frame*0.001), MAX(p.z, 0.1)+ofRandom(0.1,0.5)) );
    }

    if( 0 ){
        lines.clear();
        int num_p = points.getNumVertices();
        int line_num = 100;
        if( pos.size()!=0 && num_p>2 ){
            for( int i=0; i<line_num; i++ ){
                
                int index1 = floor( ofRandom( 0, num_p ));
                int index2 = floor( ofRandom( 0, num_p ));
                
                ofVec3f p1 = points.getVertex( index1 );
                ofVec3f p2 = points.getVertex( index2 );
                
                float dist = p1.distance( p2 );
                
                if( 0.01<dist && dist<0.05 ){
                    
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
    stringstream ss;
    ss << "frame : " << ofGetFrameNum() << "\n";
    ss << "point size: " << points.getNumVertices() << "\n";
    ss << "time : " <<  abc.getTime() << "\n";
    ofDrawBitmapString( ss.str(), 20, 20 );
}

void testApp::keyPressed( int key ){
    
}

void testApp::exit(){
    abc.close();
}
