#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    bStart = false;
    bDraw_info = true;
    
    res = 3;
    extrusionAmount = 45;
    threthold = 8;

    mesh_w = 1920;
    mesh_h = 1080;
    
	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    cam.setDistance( 1470 );
    cam.setFov( 40 );
    
    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    mainMesh.setMode( OF_PRIMITIVE_TRIANGLES );
    
    for( int y=0; y<mesh_h/res; y++ ){
        for( int x=0; x<mesh_w/res; x++ ){
            mainMesh.addVertex( ofVec3f(x*res,y*res,0) );
            mainMesh.addColor( ofFloatColor(1) );
        }
    }
    
    set_indices();
}

void ofApp::set_indices(){
    mainMesh.clearIndices();
    
    // index
    int index = 0;
    for( int y=0; y<(mesh_h/res)-1; y++ ){
        for( int x=0; x<(mesh_w/res); x++ ){
            mainMesh.addIndex( index );
            mainMesh.addIndex( index + 1 );
            mainMesh.addIndex( index + (mesh_w/res) );
            

            if( ofRandom(1.0) > 0.9999 ){
                mainMesh.addIndex( ofRandom(0, mainMesh.getNumVertices()-1) );
                mainMesh.addIndex( ofRandom(0, mainMesh.getNumVertices()-1) );
                mainMesh.addIndex( ofRandom(0, mainMesh.getNumVertices()-1) );
            }else{
                mainMesh.addIndex( index + 1 );
                mainMesh.addIndex( index + (mesh_w/res) + 1);
                mainMesh.addIndex( index + (mesh_w/res) );
            }
            index++;
        }
    }
}

void ofApp::make_random_mesh(){

    ofDirectory dir;
    string dir_path = ad_util::data_path + "img/crater_marculy";

    int img_num = dir.listDir( dir_path );
    if( img_num>0 ){
        
        ofImage img;
        int n = floor( ofRandom(0, img_num) );
        string file_name = dir.getName( n );
        string file_path = dir.getPath( n );
        
        img.loadImage( file_path );
        
        load_mesh( img, ofVec2f(0,0), extrusionAmount, 1 );
    }
    
    set_indices();
}

void ofApp::load_mesh( ofImage &img, ofVec2f start, float extrusion, int size ){
    int w = img.getWidth() / res;
    int h = img.getHeight() / res;
    
    int index = 0;
    for( int y=0; y<h; y++ ){
        for( int x=0; x<w; x++ ){
            
            ofVec2f sampling_point( x*size*res, y*size*res );
            
            if( sampling_point.x>=mesh_w || sampling_point.y>=mesh_h )
                continue;
            
            if( sampling_point.x<0 || sampling_point.y<0 )
                continue;
            
            ofFloatColor c = img.getPixelsRef().getColor( sampling_point.x, sampling_point.y );
            float b = c.getBrightness();
            float z = b * extrusion;
            mainMesh.setVertex( index, ofVec3f( sampling_point.x, sampling_point.y, z) );
            mainMesh.setColor( index, calc_color(z) );
            index++;
        }
    }
    
}


ofFloatColor ofApp::calc_color( float z ){
    ofFloatColor c;
    float gray = (z-threthold) / (extrusionAmount-threthold);
    float a = z>threthold ? 0.7 : 0.0;
    c.set( gray );
    c.a = a;
    return c;
}

void ofApp::update(){

    if( !bStart )
        return;
    
//    set_indices();
    
    float frame = ofGetFrameNum();
    if( (int)frame%25==0 )
        make_random_mesh();
        
    for( int i=0; i<mainMesh.getNumVertices(); i++ ){
        ofVec3f p = mainMesh.getVertex( i );

        if( p.z < 0 )
            continue;
        
        int w = mesh_w/res;
        if( 1 ){
            ofVec2f sp( i%w, i/w );
            ofVec3f noise;
            int octave = 4;
            float amp = 0.5;
            sp *= 0.003;
            
            for( int i=0; i<octave; i++ ){
                noise.x += ofSignedNoise( sp.x, frame*0.01 ) * amp;
                noise.y += ofSignedNoise( sp.y, frame*0.01 ) * amp;
                noise.z += ofNoise( sp.x, sp.y, frame*0.01 ) * amp;
                sp *= 2.0;
                amp *= 0.5;
            }
            noise.x *= 4.0;
            noise.y *= 4.0;
            noise.z *= 2.0;
            p -= noise;
        }
        
        p.z -= 0.6;
        
        mainMesh.setVertex( i, p );
        
        ofFloatColor c = mainMesh.getColor( i );
        mainMesh.setColor( i , calc_color(p.z) );
    }
}

void ofApp::draw(){
	ofEnableDepthTest();
    ofEnableAlphaBlending();
    
    ofBackground( 255 );
    
    cam.begin(); {
        ofTranslate( -mesh_w/2, -mesh_h/2 );
        glPointSize( 1 );
        glLineWidth( 1 );
        mainMesh.drawWireframe();
    } cam.end();
	
    saver.save();
    draw_info();
}

void ofApp::draw_info(){
    if( !bDraw_info ) return;
    ofSetColor( 200 );
    ofRect( 5, 5, 600, 300 );
    int y = 20;
    ofSetColor( 0 );
    stringstream ss;
    ss << "fps       : " << (int)ofGetFrameRate() << "\n";
    ss << "frame     : " << ofGetFrameNum() << "\n";
    ss << "cur frame : " << saver.frame_cur << "\n";
    ss << "end frame : " << saver.frame_end << "\n";
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
            make_random_mesh();
            break;
        
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
            saver.start( ofGetTimestampString(), "", 3000, 0 );
            break;
            
        default:
            break;
	}
}
