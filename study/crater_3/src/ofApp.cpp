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
    
    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    mainMesh.setMode( OF_PRIMITIVE_TRIANGLES );
    
    mesh_w = 512*2;
    mesh_h = 512;
    for( int y=0; y<mesh_h; y++ ){
        for( int x=0; x<mesh_w; x++ ){
            mainMesh.addVertex( ofVec3f(x,y,0) );
            mainMesh.addColor( ofFloatColor(1) );
        }
    }
    
    set_indices();
}

void ofApp::set_indices(){
    mainMesh.clearIndices();
    
    // index
    int index = 0;
    for( int y=0; y<mesh_h-1; y++ ){
        for( int x=0; x<mesh_w-1; x++ ){
            mainMesh.addIndex( index );
            mainMesh.addIndex( index + 1 );
            mainMesh.addIndex( index + mesh_w );
            
            mainMesh.addIndex( index + 1 );
            mainMesh.addIndex( index + mesh_w + 1);
            mainMesh.addIndex( index + mesh_w );
            index++;
        }
    }
    
}

void ofApp::make_random_mesh(){

    ofDirectory dir;
    int dir_num = dir.listDir( ad_util::data_path + "img/crater_mosaic" );
    if( dir_num>0 ){
        int level = floor( ofRandom(0, dir_num) );
        string dir_path = dir.getPath( level );
        ofDirectory level_dir;
        int file_num = level_dir.listDir( dir_path );
        
        ofImage imgL, imgR;
        bool loadOK = false;
        while( loadOK==false ) {
            int n1 = floor( ofRandom(0, file_num) );
            string file_name_L =level_dir.getName( n1 );
            
            vector<string> words = ofSplitString( file_name_L, "_");
            string xids = words[ words.size()-2 ];
            string yids_ext = words[ words.size()-1 ];
            int xid = ofToInt( xids );

            string file_name_R = "";

            for( int i=0; i<words.size()-2; i++ ){
                file_name_R += words[i];
                file_name_R += "_";
            }
            
            file_name_R += ofToString( xid+1 ) + "_" + yids_ext;
            string file_path_R = dir_path + "/" + file_name_R;
            string file_path_L =level_dir.getPath( n1 );
            
            cout << "loading imgL: " << file_path_L << ", imgR: " << file_path_R << endl;
            
            loadOK = imgL.loadImage(file_path_L) && imgR.loadImage(file_path_R);
        }
    
        int res = 1;
        load_mesh( imgL, ofVec2f(0,0), res, extrusionAmount, 1 );
        load_mesh( imgR, ofVec2f(mesh_w/2, 0), res, extrusionAmount, 1 );
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
            mainMesh.setVertex( index, ofVec3f( mx, my, z) );
            mainMesh.setColor( index, ofFloatColor(b*1.5 - 0.1, 0.9) );
        }
    }
    
}

void ofApp::update(){

    float frame = ofGetFrameNum();
    
    for( int i=0; i<mainMesh.getNumVertices(); i++ ){
        ofVec3f p = mainMesh.getVertex( i );

        if( p.z < 0 )
            continue;
        
        int w = mesh_w;
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
        ofTranslate( -mesh_w/2, -mesh_h/2 );
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
            make_random_mesh();
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
