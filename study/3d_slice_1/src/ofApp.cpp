#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    
    cam.setDistance( 31 );
    cam.setNearClip( 1 );
    cam.setFarClip( 10000 );

    mesh.setUsage( GL_DYNAMIC_DRAW );
    points.setUsage( GL_DYNAMIC_DRAW );
    lines.setUsage( GL_DYNAMIC_DRAW );

    lines.setMode( OF_PRIMITIVE_LINES );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    abc.open( ad_util::data_path + "3d/pine_tree.abc");
    abc.get(1, mesh);
}

void ofApp::update(){
    float frame = ofGetFrameNum();
    int num_p = points.getNumVertices();
    int add_num = 300;
    
    while( num_p<mesh.getNumVertices() && add_num>=0){
        int index = num_p;
        ofVec3f v = mesh.getVertex( index );
        points.addVertex( v );
        points.addColor( ofFloatColor(ofRandom(0.4,0.8), ofRandom(0.4,0.8)) );
        num_p = points.getNumVertices();
        add_num--;
    }
    
    
    /*
     *      animation
     */
    if( 1 ){
        // perlin
        
        for( int i=0; i<points.getNumVertices(); i++ ){
            ofVec3f p = points.getVertex( i );
            ofVec3f sp = p * 0.001;
            
            ofVec3f noise; noise.zero();
            float amp = 0.5;
            for( int j=0; j<4; j++ ){
//                noise.x += ofNoise( sp.x, i, frame*0.01 ) * amp;
//                noise.y += ofNoise( sp.y, i, frame*0.01 ) * amp;
                noise.z += ofSignedNoise( sp.z*100.0, i, frame*0.01 ) * amp;
                amp *= 0.5;
                sp *= 2.0;
            }
        
            noise = noise * 0.0000002*(points.getNumVertices()-i);
            points.setVertex( i, p+noise );
//            points.setColor( i, ofFloatColor(ofRandom(0.1,0.8), ofRandom(0.1,0.8)) );
        }
    }
    
    
    if( 1 ){
        for( int i=0; i<points.getNumVertices(); i++ ){
            ofVec3f p = points.getVertex( i );
            //p += sin( frame*10.0 ) * 0.01;
            mesh.setVertex( i, p );
        }
    }
    
    /*
     *      lines
     */
    lines.clear();
    for( int i=0; i<3000; i++ ){
        int index1 = ofRandom( 0, num_p-1 );
        int index2 = ofRandom( 0, num_p-1 );
        ofVec3f p1 = points.getVertex( index1 );
        ofVec3f p2 = points.getVertex( index2 );
        
        float dist = p1.distance( p2 );
        float min = 1.0;
        float max = 10.0;
        
        if( min<dist && dist<max ){
            lines.addVertex( p1 );
            lines.addVertex( p2 );
            lines.addColor( ofFloatColor(ofRandom(0.1,0.8), ofRandom(0.1,0.8)) );
            lines.addColor( ofFloatColor(ofRandom(0.1,0.8), ofRandom(0.1,0.8)) );
        }
    }
}

void ofApp::draw(){
    ofBackground(0);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    cam.begin(); {
        glPointSize( 1 );
        glLineWidth( 1 );
        ofSetColor( 255, 5 );
        mesh.draw( OF_MESH_WIREFRAME );

        ofSetColor( 255, 255 );
        lines.drawWireframe();
        points.drawWireframe();
        
    } cam.end();

    draw_info();
}

void ofApp::draw_info(){
    
    ofSetColor( 255 );
    stringstream ss;
    ss << "fps : " << ofGetFrameRate() << "\n";
    ofDrawBitmapString( ss.str().c_str(), 10, 20);
}

void ofApp::keyPressed( int key ){
	
}

void ofApp::exit(){
    abc.close();
}
