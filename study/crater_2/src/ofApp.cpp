#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    bDraw_info = true;
    bStart = true;
    
	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    cam.setDistance( 233 );
//    cam.setLensOffset( ofVec2f(0, 0.2) );
    
    // Master mesh
    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    
    dots.clear();
    dots.setUsage( GL_DYNAMIC_DRAW );
    dots.setMode( OF_PRIMITIVE_POINTS );
    
    mesh_w = 1000;
    mesh_h = 1000;
    for( int y=0; y<mesh_h; y++ ){
        for( int x=0; x<mesh_w; x++ ){
            mainMesh.addVertex( ofVec3f(x,y,0) );
            mainMesh.addColor( ofFloatColor(0.5, 0.5) );
            
            dots.addVertex( ofVec3f(x,y,0) );
            dots.addColor( ofFloatColor(0.5, 0.5) );
        }
    }
    
    set_indices();
}

void ofApp::set_indices(){

    mainMesh.clearIndices();
    
    // index
    for( int y=0; y<mesh_h-1; y++ ){
        for( int x=0; x<mesh_w-1; x++ ){
            bool tri = true;
            if( tri ){
                mainMesh.setMode( OF_PRIMITIVE_TRIANGLES );
                mainMesh.addIndex( x+y*mesh_w );				// 0
                mainMesh.addIndex( (x+1)+y*mesh_w );			// 1
                mainMesh.addIndex( x+(y+1)*mesh_w );			// 10
                
                mainMesh.addIndex( (x+1)+y*mesh_w );			// 1
                mainMesh.addIndex( (x+1)+(y+1)*mesh_w );		// 11
                mainMesh.addIndex( x+(y+1)*mesh_w );			// 10
            }else{
                mainMesh.setMode( OF_PRIMITIVE_QUADS );
                mainMesh.addIndex( x+y*mesh_w );				// 0
                mainMesh.addIndex( (x+1)+y*mesh_w );			// 1

                if( ofRandom(1.0)>0 ){
                    mainMesh.addIndex( x+(y+1)*mesh_w );			// 10
                    mainMesh.addIndex( (x+1)+(y+1)*mesh_w );		// 11
                }else{
                    mainMesh.addIndex( (x+1)+(y+1)*mesh_w );		// 11
                    mainMesh.addIndex( x+(y+1)*mesh_w );			// 10
                }
            }
        }
    }

}

void ofApp::load_mesh( ofImage &img, ofVec2f start, int resolution, float extrusion, int size ){
    int w = img.getWidth() / resolution;
    int h = img.getHeight() / resolution;
    
    for( int y=0; y<h; y++ ){
        for( int x=0; x<w; x++ ){
            
            int mx = start.x + x*size * resolution;
            int my = start.y + y*size* resolution;

            if( mx>=mesh_w || my>=mesh_h )
                continue;
            
            if( mx<0 || my<0 )
                continue;
            
            int index = mx + my*mesh_w;
            ofFloatColor c = img.getPixelsRef().getColor( x*resolution, y*resolution );
            float b = c.getBrightness();
            float z = b * extrusion;
//            ofVec3f p = mainMesh.getVertex( index );
//            z = p.z*0.5 + z*0.5;
            mainMesh.setVertex( index, ofVec3f( mx, my, z) );
            mainMesh.setColor( index, ofFloatColor(0.7-b, 0.8) );
        }
    }
    
}

void ofApp::update(){
    
    float frame = ofGetFrameNum();
 
    for( int i=0; i<mainMesh.getNumVertices(); i++ ){
        ofVec3f p = mainMesh.getVertex( i );
        ofVec2f sp( i%mesh_w, i/mesh_w );
        ofVec3f noise;
        int octave = 4;
        float amp = 0.5;
        sp *= 0.01;

//        for( int i=0; i<octave; i++ ){
//            noise.z += ofSignedNoise( sp.x, sp.y, frame*0.001 ) * amp;
//            sp *= 2.0;
//            amp *= 0.5;
//        }
//        
//        amp = 0.5;
//        sp = sp * 2.0 + octave;
        if( p.z > 0 ){
            for( int i=0; i<octave; i++ ){
                noise.z += ofNoise( sp.x, sp.y, frame*0.001 ) * amp;
                sp *= 2.0;
                amp *= 0.5;
            }
        }

        p -= noise * 1.5;
        mainMesh.setVertex( i, p );
        
        float a = 0.8;
        mainMesh.setColor( i , ofFloatColor(0.8-p.z/100.0, a) );
        
        dots.setVertex( i, p );
        dots.setColor( i, ofFloatColor(ofRandom(0.5, 0.7), 0, 0, a) );
    }
}

void ofApp::draw(){
	ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground( 200 );
    
    cam.begin();
    ofTranslate( -mesh_w/2, -mesh_h/2 );
    glPointSize( 1 );
    glLineWidth( 1 );
    mainMesh.drawWireframe();

    if( 0 ){
        ofPushMatrix();
        ofScale(1, 1, 1.7 );
        dots.draw();
        ofPopMatrix();
    }
    cam.end();
	
    draw_info();
}

void ofApp::draw_info(){
    if( !bDraw_info) return;
//    ofSetColor( 200 );
//    ofRect( 5, 5, 600, 300 );
//    int y = 20;
    ofSetColor( 0 );
    stringstream ss;
    ss << "fps       : " << (int)ofGetFrameRate() << "\n";
//    ss << "cur frame : " << saver.frame_cur << "\n";
//    ss << "end frame : " << saver.frame_end << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
    ss << "space key : start genetic calculation\n";
    ss << "I     key : draw info\n";
    ss << "S     key : save image\n";
    ss << "f     key : toggle fullscreen\n";
    ss << "cam dist  ; " << cam.getDistance() << "\n";
    
    ofDrawBitmapString( ss.str(), 20, 20 );
}

void ofApp::keyPressed( int key ){
	switch(key) {
        case 'a':
        {
            int i = floor( ofRandom(1, 9.9) );
            string name = ad_util::data_path + "img/crater" + ofToString(i)+ ".jpg";
            ofImage img;
            if( img.loadImage( name ) ){
                int w = img.getWidth();
                int h = img.getHeight();
                int res = 2;
                int size = 1;
                float realw = w * size;
                float realh = h * size;
                while( realw<mesh_w || realh<mesh_h ){
                    size++;
                    realw = w * size;
                    realh = h * size;
                }
                
                float diffx = realw - mesh_w;
                float diffy = realh - mesh_h;
                
                ofVec2f start( ofRandom(-diffx, 0), ofRandom(-diffy, 0) );
                load_mesh( img, start, res, 45, size );
            }
            
            set_indices();
            break;
        }
            
        case ' ':
            bStart = !bStart;
            break;
            
        case 'f':
            ofSetWindowShape( 1920*2, 1080*2 );
            break;
            
        case 'I':
            bDraw_info = !bDraw_info;
            break;
            
        case 'S':
            bStart = true;
            //saver.start( ofGetTimestampString(), "", 3000, frame );
            break;
            
        default:
            break;
	}
}
