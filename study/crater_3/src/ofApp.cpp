#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    extrusionAmount = 30.0;
    res = 1;

    cam.setDistance( 700 );
    cam.setFov( 40 );
    
    extrusionAmount = 50;
}

void ofApp::make_random_quad(){

    ofDirectory dir;
    int dir_num = dir.listDir( ad_util::data_path + "img/crater_mosaic" );
    if( dir_num>0 ){
        int level = floor( ofRandom(0, dir_num) );
        string dir_path = dir.getPath( level );
        ofDirectory level_dir;
        int file_num = level_dir.listDir( dir_path );
        int n = floor( ofRandom(0, file_num) );
        string file_path = level_dir.getPath( n );
        make_quad( file_path );

        cout << "load: LV: " << dir.getName(level)<< ", name: " << file_path << endl;

    }
}

void ofApp::make_line( string file_name ){
    img.loadImage( file_name );

    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    mainMesh.setMode( OF_PRIMITIVE_LINES );

    int width = img.getWidth() / res;
    int height = img.getHeight() / res;
    
    for( int y=0; y<height-1; y++ ){
        for( int x=0; x<width; x++ ){
            ofVec2f index1( x*res, y*res );
            ofVec2f index2( (x+1)*res, y*res );
            ofFloatColor c1 = img.getPixelsRef().getColor( index1.x, index1.y );
            ofFloatColor c2 = img.getPixelsRef().getColor( index2.x, index2.y );
    
            float b1 = c1.getBrightness();
            float b2 = c2.getBrightness();
            float z1 = b1 * extrusionAmount;
            float z2 = b2 * extrusionAmount;
            mainMesh.addVertex( ofVec3f(index1.x, index1.y, z1) );
            mainMesh.addVertex( ofVec3f(index2.x, index2.y, z2) );

            float a1 = b1>0.3 ? 1.0 : 0.7;
            float a2 = b2>0.3 ? 1.0 : 0.7;
            mainMesh.addColor( ofFloatColor(0.7-b1, a1) );
            mainMesh.addColor( ofFloatColor(0.7-b2, a2) );
        }
    }
}

void ofApp::make_quad( string file_name ){

    img.loadImage( file_name );
    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    mainMesh.setMode( OF_PRIMITIVE_TRIANGLES );

    int width = img.getWidth() / res;
    int height = img.getHeight() / res;
    
    for( int y=0; y<height; y++ ){
        for( int x=0; x<width; x++ ){

            ofFloatColor c = img.getPixelsRef().getColor( x*res, y*res );
            float b = c.getBrightness();
            float z = b * extrusionAmount;
            mainMesh.addVertex( ofVec3f(x*res, y*res, z) );
//            mainMesh.addColor( c );
            mainMesh.addColor( ofFloatColor(b*1.5 - 0.1, 0.9) );
        }
    }
    
    // index
    int index = 0;
    for( int y=0; y<height-1; y++ ){
        for( int x=0; x<width-1; x++ ){
//            mainMesh.addIndex(x+y*width);				// 0
//            mainMesh.addIndex((x+1)+y*width);			// 1
//            mainMesh.addIndex(x+(y+1)*width);			// 10
//            
//            mainMesh.addIndex((x+1)+y*width);			// 1
//            mainMesh.addIndex((x+1)+(y+1)*width);		// 11
//            mainMesh.addIndex(x+(y+1)*width);			// 10

            mainMesh.addIndex( index );
            mainMesh.addIndex( index + 1 );
            mainMesh.addIndex( index + width );
            
            mainMesh.addIndex( index + 1 );
            mainMesh.addIndex( index + width + 1);
            mainMesh.addIndex( index + width );
            
            index++;
        }
    }
}

void ofApp::update(){

    float frame = ofGetFrameNum();
    
    for( int i=0; i<mainMesh.getNumVertices(); i++ ){
        ofVec3f p = mainMesh.getVertex( i );

        if( p.z < 0 )
            continue;
        
        int w = img.getWidth() / res;
        if( 0 ){
            ofVec2f sp( i%w, i/w );
            ofVec3f noise;
            int octave = 4;
            float amp = 0.5;
            sp *= 0.01;
            
            for( int i=0; i<octave; i++ ){
                //noise.x += ofSignedNoise( sp.x, frame*0.001 ) * amp;
                //noise.y += ofSignedNoise( sp.y, frame*0.001 ) * amp;
                noise.z += ofNoise( sp.x, sp.y, frame*0.001 ) * amp;
                sp *= 2.0;
                amp *= 0.5;
            }

            noise *= 0.05;
            p -= noise;
        }
        
        p.z -= 0.1;
        
        mainMesh.setVertex( i, p );
        
        ofFloatColor c = mainMesh.getColor( i );
        float a = p.z/extrusionAmount;
        a = p.z>13 ? 0.95 : 0.0;
        
        mainMesh.setColor( i , ofFloatColor(c, a) );
    }
}

void ofApp::draw(){
	ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground( 255 );
    
    cam.begin(); {
        ofTranslate( -img.getWidth()/2, - img.getHeight()/2 );
        glPointSize( 1 );
        glLineWidth( 1 );
        mainMesh.drawWireframe();
    } cam.end();
	
	ofSetColor( 255 );
	string msg = "fps: " + ofToString(ofGetFrameRate(), 2 );
	ofDrawBitmapString( msg, 10, 20 );
}

void ofApp::keyPressed( int key ){
	switch(key) {
        case 'a':
        {
            make_random_quad();
            break;
        }
		case 'f':
			ofToggleFullscreen();
			break;
            
        case OF_KEY_UP:
            extrusionAmount += 10;
            break;

        case OF_KEY_DOWN:
            extrusionAmount -= 10;
            break;
        
        case 's':
            mainMesh.save( "test.ply");
            break;
	}
}
