#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){

    ofSetLogLevel( OF_LOG_VERBOSE );
    
	mesh.setUsage( GL_DYNAMIC_DRAW );
	mesh.setMode( OF_PRIMITIVE_TRIANGLES );
	
	mW = 512;
	mH = 256;

	cam.lookAt( ofVec3f(0,0,0), ofVec3f(0,1,0) );
    // cam.enableOrtho();
    // cam.setFov(10);
    cam.setDistance( mH );
    
    gn1.create( mW, mH );
    gn1.setFreq( 0.007 );
    gn1.setOctaves( 16 );
    gn1.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn1.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
    gn2.create( mW, mH );
    gn2.setFreq( 2 );
    gn2.setOctaves( 4 );
    gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
    gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    
    string fixed_point = "img/fixed_point/lg/losglaciares12.jpg";
    string gns2 = "img/gns2.jpg";
    bool ok = img.loadImage( ad_util::data_path + gns2 );
    if( ok ) cout << "laod image ok" << endl;
    else cout << "can not load image" << endl;

    int imgW = img.getWidth();
    int imgH = img.getHeight();
    
    for (int y=0; y<mH; y++) {
        for (int x=0; x<mW; x++) {
            ofVec3f pos( x-mW/2, y-mH/2, 0 );
            mesh.addVertex( pos );
            
            //float skipx = (float)imgW/mW;
            //float skipy = (float)imgH/mH;
            //ofFloatColor c = img.getColor( x*skipx, y*skipy );
            ofFloatColor c = img.getColor( ofRandom(0, imgW), ofRandom(0, imgH) );
            
            c.a = 0.9;
            if( x==0 || y==0 || x==mW || y==mH) mesh.addColor( ofFloatColor(0,0) );
            else mesh.addColor( c );
            
            nd1.push_back( pos );
            nd2.push_back( ofVec3f() );
            speeds.push_back(ofVec3f() );
            hole.push_back(0);
        }
    }
}

void ofApp::setVertices(){
	
    vector<ofVec3f> & vs = mesh.getVertices();
    unsigned char * pix = img.getPixelsRef().getPixels();
    
    float imgW = img.getWidth();
    float imgH = img.getHeight();
    float skipx = imgW/mW;
    float skipy = imgH/mH;
    
    for( int i=0; i<vs.size(); i++ ){
        
        // color dynamics
        int imgx = (i%mW) * skipx;
        int imgy = (i/mW) * skipy;
        int cid = imgx + imgy*imgW;
        float camp = 5.0;
        float r = pix[cid + 0] / 255.0 * camp;
        float g = pix[cid + 1] / 255.0 * camp;
        float b = pix[cid + 2] / 255.0 * camp;
        
        // low speed, Continum
        float amp1 = 0.01;
        speeds[i].x = gn1.getNoisef(i,0) * amp1 * r;
        speeds[i].y = gn1.getNoisef(i,1) * amp1 * g;
        speeds[i].z = gn1.getNoisef(i,2) * amp1 * b;
        nd1[i] += speeds[i];
        
        // high speed, temporary
        float amp2 = 0.1;
        nd2[i].x = gn2.getNoisef(i,0) * amp2;
        nd2[i].y = gn2.getNoisef(i,1) * amp2;
        nd2[i].z = gn2.getNoisef(i,2) * amp2 * 0.3;
        
        vs[i].x = nd1[i].x + nd2[i].x;
        vs[i].y = nd1[i].y + nd2[i].y;
        vs[i].z = nd1[i].z + nd2[i].z;
    }
}

void ofApp::setIndices(){
	
    /*
     
        Mesh indexing
        
        w - w+1 - w+2 - w+3 - w+n - (2w-1)
        0 -   1 -   2 -   3 -   n - ( w-1)
     
     */
    
    mesh.clearIndices();
    vector<ofVec3f> & vs = mesh.getVertices();
    vector<ofFloatColor> & cs = mesh.getColors();
    
	for( int i=0; i<mH-1; i++ ){
		for( int j=0; j<mW-1; j++){
            int index = j + i*mW;
            
            if ( hole[index] == 1) {
                cs[index].a      -= ofRandom(0.01, 0.2);

                vs[index].z      += gn2.getNoiseuf(index, 0) * 20;
                vs[index+1].z    += gn2.getNoiseuf(index, 1) * 20;
                vs[index-1].z    += gn2.getNoiseuf(index, 2) * 20;
                vs[index+mW].z   += gn2.getNoiseuf(index, 0) * 20;
                vs[index-mW].z   += gn2.getNoiseuf(index, 1) * 20;

                continue;
            }
            
			if( gn1.getNoiseuf(index,0)<0.6 ){
				mesh.addIndex( index );
				mesh.addIndex( index+1 );
				mesh.addIndex( index+mW );

//                mesh.addIndex( index+1 );
//                mesh.addIndex( index+mW+1 );
//                mesh.addIndex( index+mW );
            }else{
                cs[index].a      = 1;
                
                vs[index].z      += 10;
                vs[index+1].z    += 10;
                vs[index-1].z    += 10;
                vs[index+mW].z   += 10;
                vs[index-mW].z   += 10;

                hole[index] = 1;
                hole[index+1] = 1;
                hole[index-1] = 1;
                hole[index+mW] = 1;
                hole[index-mW] = 1;            }
        }
	}
}

void ofApp::setColors(){
    vector<ofVec3f> & vs = mesh.getVertices();
    vector<ofFloatColor> & cs = mesh.getColors();
    
    for (int i=0; i<cs.size(); i++) {
        int index = i;
        
        if( gn1.getNoiseuf(index,1)>0.65 ){
            cs[index].a      -= 0.005;
            cs[index+1].a    -= 0.005;
            cs[index+mW].a   -= 0.005;
            
            vs[index].z      += ofRandom(1, 15);
            vs[index+1].z    += ofRandom(1, 15);
            vs[index-1].z    += ofRandom(1, 15);
            vs[index+mW].z   += ofRandom(1, 15);
            vs[index-mW].z   += ofRandom(1, 15);
            
            hole[index] = 1;
        }
    }
}

void ofApp::update(){

    gn1.setFrame( ofGetFrameNum() * 0.008 );
    gn1.update();

    gn2.setFrame( ofGetFrameNum() * 1 );
    gn2.update();

    setVertices();
    setIndices();
    //setColors();
}

void ofApp::draw(){

	glPointSize(1);
	glLineWidth(1);
    
    ofBackground(0);
    
	cam.begin();
    if( cam.getOrtho() ){
        ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
        float s = mH*0.015;
        ofScale(s,s);
    }
	mesh.drawWireframe();
	mesh.drawVertices();
	cam.end();

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


