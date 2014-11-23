#include "ofApp.h"

void ofApp::setup(){

    bMove = false;
    res = 1;
    extrusion = 45;
    threthold = 8;

	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    cam.setDistance( 1470 );
    cam.setFov( 40 );

    gn.setup();
    gn.create( 512, 512 );
    gn.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
}

void ofApp::dragEvent( ofDragInfo info ){
    if( info.files.size() > 0 ){
        img.clear();
        img.loadImage(info.files[0]);
        load_mesh( img );
    }
}

void ofApp::load_mesh( ofFloatImage &img ){
    
    mesh.clear();
    mesh.setUsage( GL_DYNAMIC_DRAW );
    mesh.setMode( OF_PRIMITIVE_TRIANGLES );

    int ch = img.getPixelsRef().getNumChannels();
    int w = img.getWidth() / res;
    int h = img.getHeight() / res;
    
    int index = 0;
    for( int y=0; y<h; y++ ){
        for( int x=0; x<w; x++ ){
            
            ofVec2f sampling_point( x*res, y*res );
            ofFloatColor c = img.getPixelsRef().getColor( sampling_point.x, sampling_point.y );
            float z = c.getBrightness() * extrusion;
            mesh.addVertex( ofVec3f( sampling_point.x, sampling_point.y, z) );
            mesh.addColor( c );
            index++;
        }
    }
    
    index = 0;
    for( int y=0; y<h-1; y++ ){
        for( int x=0; x<w-1; x++ ){
            mesh.addIndex( index );
            mesh.addIndex( index + 1 );
            mesh.addIndex( index + w );
            
            mesh.addIndex( index + 1 );
            mesh.addIndex( index + w + 1);
            mesh.addIndex( index + w );
            index++;
        }
    }
}


void ofApp::update(){
    if( bMove ){
        gn.setFrame( ofGetFrameNum()*0.001 );
        gn.update();
        
        int w = img.getWidth() / res;
        int h = img.getHeight() / res;
        int index = 0;
        
        vector<ofVec3f> &vs = mesh.getVertices();
        unsigned char * nd = gn.getNoiseData();
        for( int y=0; y<h; y++ ){
            for( int x=0; x<w; x++ ){
                
                vs[index].x += gn.getNoiseData();
                
                index++;
            }
        }
    }
}

void ofApp::draw(){
	ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground( 255 );
    
    int w = img.getWidth();
    int h = img.getHeight();

    cam.begin(); {
        ofTranslate( -w/2, h/2 );
        ofScale( 1, -1, -1);
        glPointSize( 1 );
        glLineWidth( 1 );
        mesh.drawWireframe();
    } cam.end();
    
    gn.draw(10,10,0.5);
}

void ofApp::keyPressed( int key ){

    bool needUpdate = false;
    
    switch(key) {
        case 'f':
            ofToggleFullscreen();
            break;
            
        case OF_KEY_UP:
            extrusion+=10;
            needUpdate = true;
            break;
        case OF_KEY_DOWN:
            extrusion-=10;
            needUpdate = true;
            break;
        case OF_KEY_RIGHT:
            res++;
            needUpdate = true;
            break;
        case OF_KEY_LEFT:
            res--;
            res = max(res, 1);
            needUpdate = true;
            break;
            
        case ' ':
            bMove = !bMove;
            break;
        default:
            break;
	}
    
    if( needUpdate )
        load_mesh( img );
}
