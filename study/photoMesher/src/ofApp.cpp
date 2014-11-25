#include "ofApp.h"
#include "ofxModifierKeys.h"

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
    gn.create( 1024, 1024 );
    gn.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn.setFreq( 0.01 );
    gn.setOctaves( 4 );

    gn2.setup();
    gn2.create( 512, 512 );
    gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn2.setFreq( 0.0033 );
    gn2.setOctaves( 2 );

	
	// gui
	gui.setup( 12345 );
	gui.addPatcher(0, 0);
	gui.addOscOut(45, 10);
	gui.addFloat("extrusion", 45, 100, &extrusion);
	gui.write( "_gui_main.maxpat" );
	gui.open( "_gui_main.maxpat" );
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
            c.a = 0.7;
            float z = c.getBrightness() * extrusion;
            mesh.addVertex( ofVec3f( sampling_point.x, sampling_point.y, z) );
            mesh.addColor( c );
            index++;
        }
    }
    

    
    for( int y=0; y<h-1; y++ ){
        for( int x=0; x<w-1; x++ ){
            mesh.addIndex(x+y*w);				// 0
            mesh.addIndex((x+1)+y*w);			// 1
            mesh.addIndex(x+(y+1)*w);			// 10

            mesh.addIndex((x+1)+y*w);			// 1
            mesh.addIndex((x+1)+(y+1)*w);		// 11
            mesh.addIndex(x+(y+1)*w);			// 10
        }
    }
}

void ofApp::update(){
	gui.update();
	
    if( bMove ){
        gn.setFrame( ofGetFrameNum()*0.001 );
        gn.update();

        gn2.setFrame( ofGetFrameNum()*0.0001 );
        gn2.update();

        int n = mesh.getNumVertices();
        if( 0 < n ){
            int w = img.getWidth()/res;
            int h = img.getHeight()/res;
            vector<ofVec3f> &vs = mesh.getVertices();
            vector<ofFloatColor> &cols = mesh.getColors();
            for( int i=0; i<n; i++){
                int x = i%w;
                int y = y/h;
                
                float h = cols[i].getHue();
                float s = cols[i].getSaturation();
                float b = cols[i].getBrightness();

                vs[i].x -= gn.getNoisef(x,0) * gn2.getNoisef(y,0)  * (1.0-b) * 2.0;
                vs[i].y -= gn.getNoisef(y,1) * gn2.getNoisef(x,0 ) * (1.0-b) * 2.0;
                vs[i].z -= gn.getNoisef(i,2) * gn2.getNoisef(i,2)  * (1.0-b*1.3) * 10.0;
                
                cols[i].setHue( h + gn.getNoisef(i) * 0.1 );
                cols[i].setSaturation( s + gn.getNoisef(i) * gn2.getNoisef(i) *0.01 );
                cols[i].setBrightness( MIN(b,0.7) );
                cols[i].a = b - gn.getNoisef( i, 2 ) * 0.1;
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
    
    gn.draw(10,10,0.1);
}

void ofApp::keyPressed( int key ){

    bool needUpdate = false;
    
    bool shift = ofGetModifierPressed( OF_KEY_SHIFT );
    bool alt = ofGetModifierPressed( OF_KEY_ALT );

    if( shift ){
        switch(key) {
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
                

            default:
                break;
        }
    }else{
        switch(key){
            case 'f':
                ofToggleFullscreen();
                break;
                
            case OF_KEY_RIGHT:
                {
                    int i = gn.getShaderType();
                    gn.setShaderType((ofxGpuNoise::ShaderType)++i);
                    break;
                }
            case OF_KEY_LEFT:
                {
                    int i = gn.getShaderType();
                    gn.setShaderType((ofxGpuNoise::ShaderType)--i);
                    break;
                }
                
            case OF_KEY_UP:
                gn.setFreq( gn.getFreq()*2.0 );
                break;
                
            case OF_KEY_DOWN:
                gn.setFreq( gn.getFreq()*0.5 );
                break;
                
            case 'd':
                {
                    ofxGpuNoise::ShaderDerivType deriv = gn.getShaderDerivType();
                    gn.setShaderDerivType( deriv==0 ? ofxGpuNoise::SHADER_DERIV_TYPE_YES : ofxGpuNoise::SHADER_DERIV_TYPE_NO );
                }
            case ' ':
                bMove = !bMove;
				
			case 'u':
				load_mesh( img );
                break;
        }
    }
    
    if( needUpdate )
        load_mesh( img );
}
