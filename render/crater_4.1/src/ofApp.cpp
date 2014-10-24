#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    bStart = false;
    bDraw_info = true;
    
    res = 1;
    extrusionAmount = 45;
    mesh_w = 512*2;
    mesh_h = 512;
    
	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofBackground( 0 );
    
    cam.setDistance( 700 );
    cam.setFov( 40 );
    
    mainMesh.clear();
    mainMesh.setUsage( GL_DYNAMIC_DRAW );
    mainMesh.setMode( OF_PRIMITIVE_TRIANGLES );
    
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
            
            cout << "load img, L: " << file_name_L << ", R: " << file_name_R << endl;
            
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

    if( !bStart )
        return;
    
    float frame = ofGetFrameNum();
    if( (int)frame%25==0 )
        make_random_mesh();
        
    for( int i=0; i<mainMesh.getNumVertices(); i++ ){
        ofVec3f p = mainMesh.getVertex( i );

        if( p.z < 0 )
            continue;
        
        int w = mesh_w;
        if( 1 ){
            ofVec2f sp( i%w, i/w );
            ofVec3f noise;
            int octave = 2;
            float amp = 0.5;
            sp *= 0.01;
            
            for( int i=0; i<octave; i++ ){
                noise.x += ofSignedNoise( sp.x, frame*0.001 ) * amp;
                noise.y += ofSignedNoise( sp.y, frame*0.001 ) * amp;
                noise.z += ofNoise( sp.x, sp.y, frame*0.001 ) * amp;
                sp *= 2.0;
                amp *= 0.5;
            }

            noise *= 0.5;
            p -= noise;
        }
        
        p.z -= 0.6;
        
        mainMesh.setVertex( i, p );
        
        ofFloatColor c = mainMesh.getColor( i );
        float a = p.z/extrusionAmount;
        a = p.z>10 ? 0.95 : 0.0;
        
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
