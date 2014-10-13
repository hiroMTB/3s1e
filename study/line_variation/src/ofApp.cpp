#include "ofApp.h"

void ofApp::setup(){
    ofSetFrameRate( 60 );
    ofSetWindowPosition( 0, 0 );
    
    gap = -1.000001;
    rand_y = 100;
    
    bDraw_underline = false;
    
    for( int i=0; i<3; i++ ){
        ofVboMesh vbo;
        vbo.setUsage( GL_DYNAMIC_DRAW );
        vbo.setMode( OF_PRIMITIVE_LINES );
        lines.push_back( vbo );

        vbo.setMode( OF_PRIMITIVE_POINTS );
        points.push_back( vbo );
    }
    
    color.set( 1 );

    make_line();
    make_points();
}

void ofApp::update(){
    make_line();
    make_points();
}

void ofApp::make_line(){

    for( int i=0; i<lines.size(); i++ ){
        lines[i].clear();
    }
    
    float posx = 0;
    float posy = 0;
    int n = 0;
    static ofFloatColor c;

    while( posx < ofGetWidth()-100 ){
        c = color;
        int type = floor( ofRandom(0, lines.size()-0.01) );
        float l = ofRandom( 0.3, 5.0 ) / (float)(type+1.0);
        float ry = ofRandom( -rand_y, rand_y );
        
        lines[type].addVertex( ofVec3f(posx, posy+ry, 0) );
        posx += l;
        lines[type].addVertex( ofVec3f(posx, posy+ry, 0) );
    
        c.setBrightness( c.getBrightness()+ofRandom(-0.1, 0.1) );
        c.setSaturation( c.getSaturation()+ofRandom(-0.1, 0.1) );
        c.a = ofRandom( 0.5 );
        lines[type].addColor( c );

        c.setBrightness( c.getBrightness()+ofRandom(-0.1, 0.1) );
        c.setSaturation( c.getSaturation()+ofRandom(-0.1, 0.1) );
        c.a = ofRandom( 0.5 );
        //c.a = 1;
        lines[type].addColor( c );
        
        float g = ofRandom( gap, gap*2 );
        posx += g;
        n++;
    }
}

void ofApp::make_points(){
    for( int i=0; i<points.size(); i++ ){
        points[i].clear();
    }
    
    float posx = 0;
    float posy = 0;
    int n = 0;
    static ofFloatColor c;
    while( posx < ofGetWidth()-100 ){
        
        c = color;
        float l = ofRandom( 1, 5 );
        int type = floor( ofRandom(0, points.size()-0.01) );
        float ry = ofRandom( -rand_y, rand_y );

        points[type].addVertex( ofVec3f(posx, posy+ry, 0) );
        posx += l;
        
        c.setBrightness( c.getBrightness()+ofRandom(-0.1, 0.1) );
        c.setSaturation( c.getSaturation()+ofRandom(-0.1, 0.1) );
        c.a = ofRandom( 0.5 );
        //c.a = 1;
        points[type].addColor( c );
        
        float g = ofRandom( gap, gap*2);
        posx += g;
        n++;
    }
}

void ofApp::draw(){
    
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();

    ofBackground( 0, 0, 0, 0 );
    ofPushMatrix(); {
        ofTranslate( 50, ofGetHeight()/2 - 200 );
        ofSetColor( 200, 255 );
        
        if( bDraw_underline ){
            glLineWidth( 1 );
            ofSetColor( 0, 0, 200, 250 );
            ofLine( 0, 0, ofGetWidth(), 0 );
        }
        for( int i=0; i<lines.size(); i++ ){
            glLineWidth( i );
            lines[i].draw( OF_MESH_WIREFRAME );
        }
    }ofPopMatrix();

    ofPushMatrix(); {
        ofTranslate( 50, ofGetHeight()/2 + 200 );
        ofSetColor( 200, 355 );
        
        if( bDraw_underline ){
            ofSetColor( 0, 0, 200, 250 );
            glLineWidth( 1 );
            ofLine( 0, 0, ofGetWidth(), 0 );
        }
        for( int i=0; i<lines.size(); i++ ){
            glPointSize( 1+i );
            points[i].draw( OF_MESH_POINTS );
        }
    } ofPopMatrix();
    
    saver.save();    
}

void ofApp::keyPressed( int key ){
    switch( key ){
        case 'f':
            ofToggleFullscreen();
            break;
        
        case 'u':
            bDraw_underline = !bDraw_underline;
            break;

        case 'c':
            color.setHsb( ofRandom(1), 0.8, 0.8);
            break;
        
        case 's':
            bSmooth = !bSmooth;
            break;
            
        case OF_KEY_RIGHT:
            gap *= 1.1;
            break;
            
        case OF_KEY_LEFT:
            gap *= 0.9;
            break;
        
        case OF_KEY_UP:
            rand_y *= 1.1;
            break;
            
        case OF_KEY_DOWN:
            rand_y *= 0.9;
            break;
            
        case 'S':
            saver.start( ofGetTimestampString(), "line_variation_", 1000 );
            break;
    }
}
