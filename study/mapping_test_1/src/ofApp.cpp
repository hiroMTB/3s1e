#include "ofApp.h"


void ofApp::setup(){

    ofSetLogLevel( OF_LOG_VERBOSE );

    
    img.loadImage("img/f1.tif");

    min = GL_LINEAR;
    max = GL_LINEAR;
    img.getTextureReference().setTextureMinMagFilter( min, max );
    
    w = img.getWidth();
    h = img.getHeight();
    
}


void ofApp::update(){
}


void ofApp::draw(){
    
    ofBackground( 255 );
    
    cam.begin(); {
        
        ofSetColor(255);
        img.draw( -w/4, -h/4, w/2, h/2 );
        
    } cam.end();


    ofSetColor(155);
    stringstream ss;
//    ss << "d : " << cam.getDistance() << "\n";
    ss << "pos :" << cam.getPosition() << "\n";
    ss << "up :" << cam.getUpDir() << "\n";
    ss << "mat : \n" << cam.getModelViewMatrix() << "\n";
    
    ofDrawBitmapString(ss.str(), 20, 900);
    
}


void ofApp::keyPressed(int key){

    /*
     
            of default settings are
     
            min = GL_LINEAR
            mag = GL_LINEAR
     
     */
    
    switch (key) {
        case '1':
            min = GL_NEAREST_MIPMAP_NEAREST;
            break;
            
        case '2':
            min = GL_LINEAR_MIPMAP_NEAREST;
            break;
        case '3':
            min = GL_NEAREST_MIPMAP_LINEAR;
            break;
        case '4':
            min = GL_LINEAR_MIPMAP_LINEAR;
            break;

        case '5':
            min = GL_NEAREST;
            break;

        case '6':
            min = GL_LINEAR;
            break;
            
        case 'n':
            max = GL_NEAREST;
            break;
            
        case 'l':
            max = GL_LINEAR;
            break;            
            
        default:
            break;
    }
    
    ofSetMinMagFilters( min, max);
    img.getTextureReference().setTextureMinMagFilter( min, max );
    
}

