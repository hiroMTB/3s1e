#include "ofApp.h"
#include "ofxModifierKeys.h"

void ofApp::setup(){

    bMove = false;
    res = 1;
    extrusion = 50;
    threthold = 8;

	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    cam.setDistance( 1070 );
    cam.setFov( 40 );

    gn.setup();
    gn.create( 1024, 1024 );
    gn.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn.setFreq( 0.01 );
    gn.setOctaves( 4 );

	
	if ( !mask.loadImage( "mask1.png" ) ){
		cout << "can not load mask" << endl;
	}
	
	// gui
	gui.setup( 12345 );
	gui.addPatcher(0, 0);
	gui.addOscOut(45, 10);
	gui.addFloat("extrusion", 45, 100, &extrusion);
	gui.write( "_gui_main.maxpat" );
	gui.open( "_gui_main.maxpat" );
	
	img.loadImage("pink.png");
	load_mesh(img);
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
            c.a = 0.5;
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
        //gn.setFrame( ofGetFrameNum()*0.001 );
        //gn.update();

        int n = mesh.getNumVertices();
		
		float * maskf = mask.getPixels();
		float mw = mask.getWidth();
		float mh = mask.getHeight();
		int mn = mw * mh;
		
        if( 0 < n ){
            int w = img.getWidth()/res;
            int h = img.getHeight()/res;
            vector<ofVec3f> &vs = mesh.getVertices();
            vector<ofFloatColor> &cols = mesh.getColors();
            for( int y=0; y<h; y++){
				for( int x=0; x<w; x++){
					int index = x + y*w;
					int maskIndex = x*res + y*res*mask.getWidth();
					float r =  maskf[ maskIndex*3 + 0];
					float g =  maskf[ maskIndex*3 + 1];
					float b =  maskf[ maskIndex*3 + 2];
					float avg = (r + g + b) * 0.3333;

					if( avg>0.2 ){
						vs[index].z -= avg*4.0;
						cols[index] *= 0.999;
					}
				}
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
	mask.draw( 10, 300, 200, 100);
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
