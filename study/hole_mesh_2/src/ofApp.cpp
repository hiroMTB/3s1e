#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    ofSetLogLevel( OF_LOG_VERBOSE );
    	
	mW = 1024;
	mH = 512;

	cam.lookAt( ofVec3f(0,0,0), ofVec3f(0,1,0) );
    // cam.enableOrtho();
    // cam.setFov(10);
    cam.setDistance( mH );
    
    gn1.create( mW, mH );
    gn1.setFreq( 0.007 );
    gn1.setOctaves( 16 );
    gn1.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn1.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn1.setFrame(0.1);
    gn1.update();
    
    gn2.create( mW, mH );
    gn2.setFreq( 2 );
    gn2.setOctaves( 4 );
    gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn2.setFrame(0.01);
    gn2.update();
    
    string fixed_point = "img/fixed_point/lg/losglaciares12.jpg";
    string gns2 = "img/gns2.jpg";
    bool ok = img.loadImage( ad_util::data_path + gns2 );
    if( ok ) cout << "laod image ok" << endl;
    else cout << "can not load image" << endl;

    int imgW = img.getWidth();
    int imgH = img.getHeight();

    ofRectangle bounds = ofRectangle( 0,0, mW*1.75, mH*1.75 );
    bounds = ofRectangle( 0, 0, 10, mH*1.75 );

    int n = 1000;
    for( int i=0; i<n; i++ ){
        float x = ofNoise(i*0.01) * bounds.width;
        float y = ofNoise(x, i*0.0001) * bounds.height - i*0.5;
        
        //x = bounds.width/2 + ofRandomf();
        ofVec3f pos(x, y, 0);
        seeds.push_back( pos );
        
        nd1.push_back( pos );
        nd2.push_back( ofVec3f() );
        speeds.push_back(ofVec3f() );
        hole.push_back(0);
    }
    
    // voronoi
    voro.setBounds( bounds );
    voro.setPoints( seeds );
    voro.generate();
}

void ofApp::setVertices(){
	
    unsigned char * pix = img.getPixelsRef().getPixels();
    
    float imgW = img.getWidth();
    float imgH = img.getHeight();
    float skipx = imgW/mW;
    float skipy = imgH/mH;
    
    for( int i=0; i<seeds.size(); i++ ){
        
        // color dynamics
        int imgx = (i%mW) * skipx;
        int imgy = (i/mW) * skipy;
        int cid = imgx + imgy*imgW;
        float camp = 5.0;
        float r = pix[cid + 0] / 255.0 * camp;
        float g = pix[cid + 1] / 255.0 * camp;
        float b = pix[cid + 2] / 255.0 * camp;
        
        // low speed, Continum
        float amp1 = 0.1;
        nd1[i].x = gn1.getNoisef(i,0) * amp1 * r*3 + 0.3;
        nd1[i].y = gn1.getNoisef(i,1) * amp1 * g;
        
        // high speed, temporary
        float amp2 = 0.1;
        nd2[i].x = gn2.getNoisef(i,0) * amp2;
        nd2[i].y = gn2.getNoisef(i,1) * amp2;
        
        seeds[i].x += nd1[i].x + nd2[i].x;
        seeds[i].y += nd1[i].y + nd2[i].y;
    }
    
    ofRectangle bounds = ofRectangle( 0, 0, 10 + ofGetFrameNum(), mH*1.75 );
  
    // new seed on start
    for (int i=0; i<6; i++) {
        float x = ofRandomuf();
        float y = gn1.getNoisef(i*10, 0) * bounds.height - i;
        
        ofVec3f pos(x, y, 0);
        seeds.push_back( pos );
        
        nd1.push_back( pos );
        nd2.push_back( ofVec3f() );
        speeds.push_back(ofVec3f() );
        hole.push_back(0);
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

    gn1.setFrame( ofGetFrameNum() * 0.008 );
    gn1.update();

    gn2.setFrame( ofGetFrameNum() * 1 );
    gn2.update();

    setVertices();
    //setIndices();
    //setColors();
}

void ofApp::draw(){

	glPointSize(1);
	glLineWidth(1);
    
    ofBackground(0);
    
//	cam.begin();
//    if( cam.getOrtho() ){
//        ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
//        float s = mH*0.015;
//        ofScale(s,s);
//    }
//	mesh.drawWireframe();
//	mesh.drawVertices();
//    
//	cam.end();

    voro.draw();
    
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

}


