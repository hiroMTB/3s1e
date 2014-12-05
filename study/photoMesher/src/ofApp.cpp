#include "ofApp.h"
#include "ofxModifierKeys.h"
#include "ad_util.h"

void ofApp::setup(){

    bMove = false;
	
    res = 8;
    extrusion = -300;
    threthold = 8;

	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    cam.setDistance( 3000 );
    cam.setFov( 40 );

    gn.setup();
    gn.create( 2428,1378 );
    gn.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn.setFreq( 0.001 );
    gn.setOctaves( 4 );


	gn2.setup();
	gn2.create( 2428,1378 );
	gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
	gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
	gn2.setFreq( 0.0001 );
	gn2.setOctaves( 2 );
	
    if ( !mask.loadImage( ad_util::data_path + "img/crater_Marculy/mMask4.jpg" ) ){
		cout << "can not load mask" << endl;
	}
	
	// gui
	gui.setup( 12345 );
	gui.addPatcher(0, 0);
	gui.addOscOut(45, 10);
	gui.addToggle("useMask", 45, 100, &bUseMask);
	gui.addFloat("extrusion", 45, 130, &extrusion);
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
    
    speed.assign(w*h, ofVec3f(0,0,0) );
    accel.assign(w*h, ofVec3f(0,0,0) );
    
}

void ofApp::update(){
	gui.update();
	
    if( bMove ){
        gn.setFrame( ofGetFrameNum()*0.01 );
        gn.update();

		gn2.setFrame( ofGetFrameNum()*0.02 );
		gn2.update();

        int n = mesh.getNumVertices();
        if( 0 < n ){
            
            float * maskf = mask.getPixels();
            float mw = mask.getWidth();
            float mh = mask.getHeight();
            int mn = mw * mh;
            
            int w = img.getWidth()/res;
            int h = img.getHeight()/res;
            vector<ofVec3f> &vs = mesh.getVertices();
            vector<ofFloatColor> &cols = mesh.getColors();

            vector<ofVec3f>::iterator vitr = vs.begin();
            vector<ofFloatColor>::iterator citr = cols.begin();
            
            for( int y=0; y<h; y++){
				for( int x=0; x<w; x++){
                    vitr++;
                    citr++;
                    
                    int real_index = x*res + y*res*img.getWidth();
					int index = x + y*w;
					float rate = 1;
					
					if( bUseMask ){
						int maskIndex = x*res + y*res*mask.getWidth();
						if( mw<=x || mh<=y )
							continue;
						
						float r =  maskf[ maskIndex*3 + 0];
						float g =  maskf[ maskIndex*3 + 1];
						float b =  maskf[ maskIndex*3 + 2];
						float avg = (r + g + b) * 0.3333;
						
						rate = avg + 0.1;
					}else{
						
						float r = cols[index].r;
						float g = cols[index].g;
						float b = cols[index].b;
						
						rate = (r+g+b) * 0.333;
					}
					
					float nR = gn.getNoisef( x, y, 0);
                    float nG = gn.getNoisef( x, y, 1);
                    float nB = gn.getNoisef( x, y, 2);

					float nR2 = gn2.getNoisef( x, y, 0);
					float nG2 = gn2.getNoisef( x, y, 1);
					float nB2 = gn2.getNoisef( x, y, 2);
					
					nR *= nR2;
					nG *= nG2;
					nB *= nB2;
					
                    accel[index].x = nR * rate * 0.3;
                    accel[index].y = nG * rate * 0.3;
                    accel[index].z = nB * rate;
					
                    speed[index] += accel[index];
                    vs[index] += speed[index];
					
					vs[index] += ofVec3f( ofRandomf(),ofRandomf(),ofRandomf() );
                    
                    rate *= 0.003;
                    cols[index].r += (nR * rate);
                    cols[index].g += (nG * rate);
                    cols[index].b += (nB * rate);
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
	//mask.draw( 10, 300, 200, 100);
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
				break;
				
			case 'u':
				load_mesh( img );
                break;
        }
    }
    
    if( needUpdate )
        load_mesh( img );
}
