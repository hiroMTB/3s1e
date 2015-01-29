//
//  ofApp_misc.cpp
//  flow_fill_2
//
//  Created by mtb on 20/01/15.
//
//

#include "ofApp.h"
#include "ad_util.h"
#include "ofxSvg.h"

void ofApp::load_svg( string path ){
    ofxSVG svg;
    
    if( path == ""){
        ofFileDialogResult openFileResult= ofSystemLoadDialog("Select a svg file");
        if (openFileResult.bSuccess){
            ofLogVerbose("getPath(): "  + openFileResult.getPath());
            ofFile file (openFileResult.getPath());
            
            if (file.exists()){
                string fileExtension = ofToUpper(file.getExtension());
                if (fileExtension == "svg" || fileExtension == "SVG" )
                    path = openFileResult.getPath();
            }else{
                ofLogVerbose("file not exist");
                ofExit();
            }
        }else {
            ofLogVerbose("User hit cancel");
            ofExit();
        }
    }
    
    svg.load( path );
    
    win.x = ceil( svg.getWidth()) ;
    win.y = ceil( svg.getHeight() );
    if( (int)win.x%4 != 0){
        win.x += 4 - (int)win.x%4;
    }
    
    cout << "\nLoading svg : " << win << " " << path <<  endl;
    
    int n = svg.getNumPath();
    if( n==0 ) ofExit();
    
    for (int i=0; i<n; i++) {
        ofPath &p = svg.getPathAt(i);
        
        // Black Line
        if( p.isFilled() ){
            //ofColor c = p.getFillColor();
            //if( c.r + c.b + c.g == 0 ){
                mask = p;
                cout << "LOAD Mask" << endl;
            //}
        }else{
            ofColor c = p.getStrokeColor();
            if( c == ofColor::black ){
                vector<ofPolyline> polys = p.getOutline();
                for (int j=0; j<polys.size(); j++) {
                    const vector<ofVec3f> &vs = polys[j].getVertices();
                    if( vs.size() == 2 ){
                        ofPoint st = ( polys[j].getVertices()[0] );
                        ofPoint end = ( polys[j].getVertices()[1]);
                        grav_wall.create_line(st, end, ofApp::density );
                        cout << "crate particle line" << endl;
                    }
                }
            }else if( c == ofColor::green ){
                vector<ofPolyline> polys = p.getOutline();
                for (int j=0; j<polys.size(); j++) {
                    const vector<ofVec3f> &vs = polys[j].getVertices();
                    if( vs.size() == 2 ){
                        ofPoint st = ( polys[j].getVertices()[0] );
                        ofPoint end = ( polys[j].getVertices()[1]);
                        grav_wall.create_waterline(st, end );
                        cout << "crate waterline" << endl;
                    }
                }
            }else if( c == ofColor::blue ){

                wall_path.push_back( p );
                
                vector<ofPolyline> polys = p.getOutline();
                for (int j=0; j<polys.size(); j++) {
                    const vector<ofVec3f> &vs = polys[j].getVertices();
                    if( vs.size() == 2 ){
                        ofPoint st = ( polys[j].getVertices()[0] );
                        ofPoint end = ( polys[j].getVertices()[1]);
                        grav_wall.create_wall(st, end, 1 );
                        cout << "crate Wall" << endl;
                    }
                }
            }
        }
    }
    cout << "svg canvas size: " << win.x << ", " << win.y << endl;
    
}

void ofApp::draw_info(){
    if ( !bDraw_info)
        return;
    
    stringstream ss;
    ss << "Exporter FilePatern : " << exps[0].getFilePattern() << "\n";
    ss << "fps: " << ofToString( ofGetFrameRate(),0 ) << "\n";
    ss << "frame     : " << ofGetFrameNum() << "\n";
    ss << "cur frame : " << exps[0].getFrameNum() << "\n";
    ss << "resolution: " << exps[0].getFbo().getWidth() << ", " << exps[0].getFbo().getHeight() << "\n" << "\n";
    ss << "I key:   toggle info " << "\n";
    ss << "Collision Shape : " << grav_wall.world.world->getNumCollisionObjects() << endl;
    ss << "\n" << gpu_noise.getNoiseParamString() << "\n";
    
    ofSetColor( 0 );
    ofDrawBitmapString( ss.str(), 10, 20 );
};

void ofApp::keyPressed( int key ){
    switch( key ) {

		case '0':
			sel_layer = 0;
			break;

		case '1':
			sel_layer = 1;
			break;
			
		case 'f':
            ofToggleFullscreen();
            break;
            
        case 'S':
            for (int i=0; i<2; i++) {
                exps[i].startExport();
            }
            break;
            
        case 'n':
            if( !exps[0].isExporting() ){
                ad_grav_wall::bDrawNearLine = !ad_grav_wall::bDrawNearLine;
            }
            break;
        case 'a':
            if( !exps[0].isExporting() ){
                ad_grav_wall::bDrawAttrLine = !ad_grav_wall::bDrawAttrLine;
            }
            break;

        case OF_KEY_RIGHT:
        {
            int i = gpu_noise.getShaderType();
            gpu_noise.setShaderType((ofxGpuNoise::ShaderType)++i);
            break;
        }
        case OF_KEY_LEFT:
        {
            int i = gpu_noise.getShaderType();
            gpu_noise.setShaderType((ofxGpuNoise::ShaderType)--i);
            break;
        }

        case OF_KEY_UP:
            gpu_noise.setFreq( gpu_noise.getFreq()*2.0 );
            break;
            
        case OF_KEY_DOWN:
            gpu_noise.setFreq( gpu_noise.getFreq()*0.5 );
            break;
    };
}

void ofApp::setup_window( int w, int h ){

    string timeStamp = ofGetTimestampString();
    
    exps.assign(2, ofxExportImageSequence() );

    for( int i=0; i<2; i++){
        exps[i].setup(w, h, 25, GL_RGB, 0);
        string pat = timeStamp + "/L" + ofToString(i) + "/F_%05i.png";
        exps[i].setFilePattern( pat );
        exps[i].setFrameRange(1, 3502);
        exps[i].setAutoExit(true);
    }

    if(bRender){
        ofSetWindowShape(w/4, h/4);
        ofSetWindowPosition(0, 0);
        exps[0].startExport();
        exps[1].startExport();
    }else{
        ofSetWindowShape(w, h);
        ofSetWindowPosition(0, 0);
    }
}

void ofApp::load_noise(){
    gpu_noise.setup();
    gpu_noise.setOctaves( 6 );
    gpu_noise.setFreq( 0.015 );
    gpu_noise.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gpu_noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
    int nsize = 4096;
    gpu_noise.create( nsize, 8);
    noise_size = nsize * 4;
    gpu_noise.setFrame( 0.01 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
}
