#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    ofSetLogLevel( OF_LOG_VERBOSE );
    
    mW = 2000;
	mH = 1000;
	 
    gn1.create( mW/4, mH/4 );
    gn1.setFreq( 0.02 );
    gn1.setOctaves( 12 );
    gn1.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn1.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn1.setFrame(0.1);
    gn1.update();
    
    gn2.create( mW/4, mH/4);
    gn2.setFreq( 2 );
    gn2.setOctaves( 4 );
    gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn2.setFrame(0.01);
    gn2.update();
    
    ofRectangle bounds = ofRectangle( 0,0, mW, mH);
    bounds = ofRectangle( 0, 0, 50, mH);

    int n = 10;
    for( int i=0; i<n; i++ ){
        float x = ofNoise(i*0.01) * bounds.width;
        float y = ofNoise(x, i*0.0001) * bounds.height - i*0.5;
        
        //x = bounds.width/2 + ofRandomf();
        ofVec3f pos(x, y, 0);
        seeds.push_back( pos );
        
        nd1.push_back( pos );
        nd2.push_back( ofVec3f() );
        speeds.push_back(ofVec3f( -320, 0) );
    }
    
    // voronoi
    voro.setBounds( bounds );
    voro.setPoints( seeds);
    voro.generate();
    
    exporter.setup(mW, mH, 25, GL_RGBA, 8);
    exporter.setOutputDir(ofGetTimestampString());
    exporter.setAutoExit(true);
    
    ofSetWindowPosition(0, 0);
    ofSetWindowShape(mW, mH);
}

void ofApp::setVertices(){
	
    for( int i=0; i<seeds.size(); i++ ){

        // low speed, Continum
        float amp1 = 1;
        //nd1[i].x -= 12;
        //nd1[i].y -= ( gn1.getNoisef(i,1) * amp1 ) * 0;
        
        // high speed, temporary
        float amp2 = 0.001;
        nd2[i].x = gn2.getNoisef(i,0) * amp2;
        nd2[i].y = gn2.getNoisef(i,1) * amp2;
        
        speeds[i].x += ( gn1.getNoiseuf(i,1) * amp1 ) * 2.0;
        speeds[i].y += ( gn1.getNoisef(i,2) * amp1 - 0.02);
        
        seeds[i].x = speeds[i].x + nd2[i].x;
        seeds[i].y = speeds[i].y + nd2[i].y;
    }
    
//    float speed = ofNoise( ofGetFrameNum()*0.1)*30 + 1;
    float speed = 1;
    int w = voro.getBounds().width;
    ofRectangle bounds = ofRectangle( 0, 0, w+speed, mH );
  
    // new seed on start
    for (int i=0; i<10; i++) {
        float x =  bounds.width - 10;
        float y = gn1.getNoiseuf(i*10, 0) * 700;
        
        ofVec3f pos(x, y, 0);
        seeds.push_back( pos );
        
        nd1.push_back( pos );
        nd2.push_back( ofVec3f() );
        speeds.push_back(ofVec3f(-300, 0, 0) );
    }
    
    //voro.clear();
    voro.setBounds( bounds );
    voro.setPoints( seeds );
    voro.generate();
}

void ofApp::setColors(){
    //vector<ofVec3f> & vs = mesh.getVertices();
    //vector<ofFloatColor> & cs = mesh.getColors();
//    
//    for (int i=0; i<cs.size(); i++) {
//        int index = i;
//        
//        if( gn1.getNoiseuf(index,1)>0.65 ){
//            cs[index].a      -= 0.005;
//            cs[index+1].a    -= 0.005;
//            cs[index+mW].a   -= 0.005;
//            
//            vs[index].z      += ofRandom(1, 15);
//            vs[index+1].z    += ofRandom(1, 15);
//            vs[index-1].z    += ofRandom(1, 15);
//            vs[index+mW].z   += ofRandom(1, 15);
//            vs[index-mW].z   += ofRandom(1, 15);
//            
//            hole[index] = 1;
//        }
//    }
}

void ofApp::update(){

    gn1.addFrame( 0.01 );
    gn1.update();

    gn2.addFrame( 0.021 );
    gn2.update();

    setVertices();
    //setIndices();
    //setColors();
}

void ofApp::draw(){

	glPointSize(1);
	glLineWidth(1);
    
    exporter.begin();
    ofBackground( 255 );
    ofTranslate( mW*0.6, mH );
    voro.draw();
    exporter.end();
    
    ofBackground(255);
    ofSetColor(225);
    exporter.draw(0, 0);
	draw_info();
    float nscale = 0.5;
    gn1.draw( ofGetWidth()-10-mW*nscale, 10, nscale );
    gn2.draw( ofGetWidth()-10-mW*nscale, 10+10+mH*nscale, nscale );
}

void ofApp::draw_info(){
	
	stringstream ss;
	ss << "fps     : " << ofGetFrameRate() << "\n";
	
	ofSetColor(0);
	ofDrawBitmapString(ss.str(), 40, 40 );
	
}

void ofApp::keyPressed(int key){

    if( key == 'S'){
        exporter.startExport();
    }
}


