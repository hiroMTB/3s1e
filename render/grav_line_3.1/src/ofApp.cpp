//
//  ofApp.cpp
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#include "ofApp.h"
#include "ad_util.h"

ofApp * ofApp::app = NULL;

void ofApp::setup(){
    bDraw_info = true;
    bStart = false;
    density = 0.5;

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetCircleResolution( 6 );

    ofImage img;
    if( img.loadImage( "img/losglaciares10.jpg" ) ){
        gravline.setup( &img );
        cout << "load image OK" << endl;
    }else{
        gravline.setup();
        cout << "Can not load image " << endl;
    }
    setup_noise();
    setup_svg();
    
    exporter.setup(canvas.x, canvas.y, 25, GL_RGB, 8);
    exporter.setOutputDir( ofGetTimestampString() );
    exporter.setFrameRange(1, 3000);
    exporter.setAutoExit(true);
    
    ofSetWindowShape(canvas.x*0.75, canvas.y*0.75);
    ofSetWindowPosition(0, 0);
    cout << "canvas size : " << canvas << endl;
};

void ofApp::setup_svg(){
    ofFileDialogResult openFileResult= ofSystemLoadDialog("Select SVG file");
    if (openFileResult.bSuccess){
        
        ofLogVerbose("User selected a file");
        ofLogVerbose("getName(): "  + openFileResult.getName());
        ofLogVerbose("getPath(): "  + openFileResult.getPath());
        
        ofFile file (openFileResult.getPath());
    	if (file.exists()){
            
            svg.load( openFileResult.getPath() );
            int n = svg.getNumPath();
            if( n>0 ){
                cout << "load SVG success, found " << n << " paths" << endl;
            }else{
                cout << "load SVG fail, can not find path" << endl;
                ofExit();
            }
            
            float w = svg.getWidth()  + 1;
            float h = svg.getHeight() + 1;
            canvas.set(w, h);

            for( int i=0; i<n; i++ ){
                ofPath &p = svg.getPathAt(i);
                ofFloatColor c = p.getStrokeColor();
                
                vector<ofPolyline>& lines = p.getOutline();
                for(int j=0;j<(int)lines.size();j++){
                    ofPoint st = ( lines[j].getVertices()[0] );
                    ofPoint end = ( lines[j].getVertices()[1]);
                    
                    gravline.create_line(st, end, density );
                    
                    cout << "grav line from " << st << "  ->  " << end << endl;
                }
            }
        }
        
    }else {
        ofLogVerbose("User hit cancel");
    }
}


void ofApp::setup_noise(){
    gpu_noise.setup();
    gpu_noise.setOctaves( 4 );
    gpu_noise.setFreq( 0.0021 );
    gpu_noise.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gpu_noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
    int nsize = 512;
    gpu_noise.create( nsize, nsize);
    noise_size = nsize * nsize;
    gpu_noise.setFrame( 0.01 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
}

void ofApp::update(){
    if( !bStart ) return;
    frame++;
    gpu_noise.setFrame( frame *0.001 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    gravline.update();
}

void ofApp::draw(){
    
    exporter.begin();{
        ofEnableAlphaBlending();
        ofEnableAntiAliasing();
        ofEnableSmoothing();

        ofSetupScreenOrtho();
        ofBackground( 255, 255, 255, 255 );
        gravline.draw();
        
        if(!exporter.isExporting()) gravline.draw_attr();
        
    }exporter.end();

    ofPushMatrix();{
        ofBackground(0,0,0,255);
        ofSetColor(255);
        exporter.draw(0, 0);

        draw_info();
        if( !exporter.isExporting() )
            gpu_noise.draw( ofGetWidth()-110, 40, 0.2);
    }ofPopMatrix();
}

void ofApp::draw_info(){
    if ( !bDraw_info)
        return;
    
    stringstream ss;
    ss << "fps: " << ofToString( ofGetFrameRate(),0 ) << "\n";
    ss << "frame     : " << frame << "\n";
    ss << "export frame : " << exporter.getFrameNum() << "\n";
    ss << "resolution: " << exporter.getFbo().getWidth() << ", " << exporter.getFbo().getHeight() << "\n" << "\n";
    ss << "num shapes: " << gravline.shapes.size() << "\n";
    ss << "Collision Object : " << gravline.world.world->getNumCollisionObjects() << "\n";
    ss << "\n";
    ss << "joints    : " << gravline.joints.size() << "\n";
    ss << "I key     : toggle info " << "\n";
    ss << "\n" << gpu_noise.getNoiseParamString() << "\n";
    
    ofSetColor( 0 );
    ofDrawBitmapString( ss.str(), 20, 40 );
};

void ofApp::keyPressed( int key ){
    switch( key ) {
        case ' ':
            bStart = !bStart;
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;

        case 'S':
            bStart = true;
            exporter.startExport();
            ofSetWindowShape(canvas.x*0.25, canvas.y*0.25);
            ofSetWindowPosition(0, 0);
            break;

        case 'I':
            bDraw_info = !bDraw_info;
            break;
            
//        case OF_KEY_RIGHT:
//        {
//            int i = gpu_noise.getShaderType();
//            gpu_noise.setShaderType((ofxGpuNoise::ShaderType)++i);
//            break;
//        }
//        case OF_KEY_LEFT:
//        {
//            int i = gpu_noise.getShaderType();
//            gpu_noise.setShaderType((ofxGpuNoise::ShaderType)--i);
//            break;
//        }
//            
//        case OF_KEY_UP:
//            gpu_noise.setFreq( gpu_noise.getFreq()*2.0 );
//            break;
//            
//        case OF_KEY_DOWN:
//            gpu_noise.setFreq( gpu_noise.getFreq()*0.5 );
//            break;
    };
}
