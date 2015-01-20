#include "ofApp.h"
#include "ad_util.h"

void ofApp::setup(){
	
	bStart = false;
    frame=0;
    rot = 0;
	
    ofSetVerticalSync( true );
	ofSetFrameRate(30);
	
	ofBackground( 255 );
	ofEnableAlphaBlending();
		
    {
        points.setUsage( GL_DYNAMIC_DRAW );
        points.setMode( OF_PRIMITIVE_POINTS );
        
        points_a.setUsage( GL_DYNAMIC_DRAW );
        points_a.setMode( OF_PRIMITIVE_POINTS );
        
        tracker.setUsage( GL_DYNAMIC_DRAW );
        tracker.setMode( OF_PRIMITIVE_POINTS );
    }
    {
        lines.setUsage( GL_DYNAMIC_DRAW );
        lines.setMode( OF_PRIMITIVE_LINES );
    }
    
	bool ok = img.loadImage( ad_util::data_path  + "img/rk/Mx80_1_org.jpg");	// px
	if( !ok ) cout << "image load failer" << endl;
		
	int w = 2000;
	int h = 1200;
	
	exporter.setup(w, h, 25, GL_RGB, 8);
    exporter.setFilePattern( ofGetTimestampString() + "/F_%05i.png");
    exporter.setFrameRange(0, 500);
	exporter.setAutoExit(true);

    ofSetWindowShape( w, h );
    ofSetWindowPosition( 0, 0 );
}

void ofApp::update(){
	if( !bStart )
		return;
	
	frame++;
	frame = (int)frame % 750;
	
	string path = "sim/A/Linear02_" + ofToString( frame,0,5,'0' )+ ".abc";
	
	ofxAlembic::Reader abc;
	abc.open(path);
//	abc.dumpNames();
	
    vector<ofVec3f> pos;
    abc.get( 0, pos );
	
    points.clear();
    points_a.clear();
    
    lines.clearVertices();
    lines.clearColors();
    
	unsigned char * pix = img.getPixels();
	int n = pos.size();
	
	for(int i=0; i<n; i++ ){

		// inverse walksthrough
		int index  = (n-1-i) % (int)(img.getWidth() * img.getHeight() );

        const ofVec3f &p = pos[index];
        float scale = 100.0;
        points.addVertex( p * scale );
        points_a.addVertex( p * scale );
        
        int w = img.getWidth();
        int h = img.getHeight();
        int x = (i*300) % w;
        int y = i / w;

        x %= w;
        
        ofFloatColor c = img.getPixelsRef().getColor(x, y);

        if( ofNoise(1, x, y) > 0.9 ){
            c.r = 0.5;
        }
            
        c.a = 0.3;
        points.addColor( c );
        c.a = 0.05;
        points_a.addColor( c );

        if( ofGetFrameNum()%1 == 0){
            if( i >= splines.size() ){
                splines.push_back( ofxSimpleSpline() );
                splines[i].subdivisions = 10;
                
                spRmxs.push_back(ofVboMesh());
                spRmxs[spRmxs.size()-1].setUsage(GL_DYNAMIC_DRAW);
                spRmxs[spRmxs.size()-1].setMode(OF_PRIMITIVE_LINE_STRIP);
            }
            splines[i].cv.push_back( p * scale );
        }
    }

    
    // MOVE
    for (int i=0; i<splines.size(); i++) {
        for (int j=0; j<splines[i].cv.size()-1; j++) {
            splines[i].cv[j].z += ofSignedNoise( 1, i*0.1, j*0.1 ) * 1.1;
            splines[i].cv[j].y += ofNoise( 2, i*0.1, j*0.1 ) * 2.75;
        }
        
        splines[i].update();
        
        
        // RMX
        spRmxs[i].clearVertices();

        int nv = splines[i].lineVbo.getNumVertices();

        const vector<ofFloatColor> & cs = points.getColors();
        for (int j=0; j<nv-5; j++) {
            
            if( ofNoise( ofGetFrameNum()*0.1, i*0.1, j*0.1) > 0.5 ){
                int index = j;
                const ofVec3f v1 = splines[i].lineVbo.getVertex( index );
                spRmxs[i].addVertex( v1 );
                ofFloatColor c = ofFloatColor(1)-cs[i];
                c.a = 0.3;
                spRmxs[i].addColor( c );

                if ( ofRandomuf()>0.92) {
                    int id1 = index + ofRandom(-15, 15);
                    if( id1<0 || nv<=id1 ) continue;
                    int id2 = index + ofRandom(-15, 15);
                    if( id2<0 || nv<=id2 ) continue;
                    
                    const ofVec3f v1 = splines[i].lineVbo.getVertex( id1 );
                    const ofVec3f v2 = splines[i].lineVbo.getVertex( id2 );
                    lines.addVertex( v1 );
                    lines.addVertex( v2 );
                    
                    //ofFloatColor c(1,0,0);
                    lines.addColor( c );
                    lines.addColor( c );
                }
            }
        }
    }
    
    
    tracker.clear();
    int track = ofGetFrameNum();
    for (int i=0; i<splines.size(); i++) {
        int nv = splines[i].lineVbo.getNumVertices();
        
        if( nv <= 1)
            continue;
        
        nv -= 1;
        int index = track % nv;
        const ofVec3f &v = splines[i].lineVbo.getVertex( index );
        tracker.addVertex( v );
        tracker.addColor( ofFloatColor(1,0,0,0.6) );
    }
    

	abc.close();

}

void ofApp::draw(){
    
    exporter.begin();{
		
		ofEnableAlphaBlending();
        
		ofBackground( 255 );

		ofPushMatrix();{
            ofTranslate(exporter.getFbo().getWidth()/2, 10); //exporter.getFbo().getHeight()/2);
            ofRotate(90, 0, 1, 0);
            ofRotate(rot, 0, 1, 0);
			
			ad_util::draw_axis();
            

//            ofEnableAntiAliasing();
//            const vector<ofFloatColor> & cs = points.getColors();
//            for( int i=0; i<splines.size(); i++ ){
//                ofSetColor(ofFloatColor(1)-cs[i], 80);
//                splines[i].draw();
//            }

            
            ofEnableAntiAliasing();
            for (int i=0; i<spRmxs.size(); i++) {
                spRmxs[i].draw();
            }
            glLineWidth(1);
            lines.draw();

            
            ofDisableAntiAliasing();
            ofSetColor(10, 70);
            for( int i=0; i<splines.size(); i++ ){
                glPointSize(1);
                glBegin( GL_POINTS );
                for (int j=0; j< splines[i].cv.size(); j++) {
                    const ofVec3f &v = splines[i].cv[j];
                    glVertex3f( v.x, v.y, v.z );
                }
                glEnd();
                
            }
            glPointSize(1);
            tracker.draw();
            
            ofEnableAntiAliasing();
            glPointSize( 8 );
            points_a.draw();

            ofDisableAntiAliasing();
            glPointSize( 1 );
			points.draw();

            
		}ofPopMatrix();
	
	} exporter.end();
	
    ofBackground(40);
	ofSetColor(255);
	exporter.draw(0, 0);
	draw_info();
}

void ofApp::draw_info(){
	ofPushMatrix();
	ofSetColor( 0 );
	stringstream ss;
	ss << "fps       : " << (int)ofGetFrameRate() << "\n";
	ss << "frame     : " << (int)frame << "\n";
	ss << "exporter frame : " << exporter.getFrameNum() << "\n";
	ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
	ss << "particles : " << points.getNumVertices() << "\n";
	ss << "\n";
    ofDrawBitmapString(ss.str(), 20, 20 );
	ofPopMatrix();
    
}

void ofApp::keyPressed( int key ){
	
	switch( key ) {
		case ' ':
			bStart = !bStart;
			break;
			
		case 'f':
			ofSetWindowShape( 1920*2, 1080*2 );
			break;
						
		case 'S':
			bStart = true;
			exporter.startExport();
			ofSetWindowShape(exporter.getFbo().getWidth()/4, exporter.getFbo().getHeight()/4 );
			ofSetWindowPosition(0, 0);
			break;
			
		case 'r':
			rot += 90;
			break;
	}
}
