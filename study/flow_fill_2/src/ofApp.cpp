#include "ofApp.h"
#include "ad_util.h"


//#define OPEN_DIALOG
//#define RENDER
#define SPLINE
#define CP_POINT
#define CIRCLE

ofColor ofApp::orange = ofColor( 255, 80, 0, 200);

void ofApp::setup(){
	bStart = true;
	frame=0;
	rot = 0;
	ofSetVerticalSync( true );
	ofSetFrameRate(30);
	ofSetCircleResolution( 100 );
	
	rf_points.setUsage( GL_DYNAMIC_DRAW );
	rf_points.setMode( OF_PRIMITIVE_POINTS );
	
	r_lines.setUsage( GL_DYNAMIC_DRAW );
	r_lines.setMode( OF_PRIMITIVE_LINES );

	load_svg();
	
	bool ok = img.loadImage( ad_util::data_path  + "img/rk/Mx80_1_org.jpg");
	if( !ok ) cout << "image load failer" << endl;
	
	// Initial Particle
	for (int i=0; i<300; i++) {
		add_particle();
	}
	
	exporter.setup(win.x, win.y, 25, GL_RGB, 0);
	exporter.setFilePattern( ofGetTimestampString() + "/F_%05i.png");
	exporter.setFrameRange(0, 500);
	exporter.setAutoExit(true);
	
	ofSetWindowShape( win.x, win.y );
	ofSetWindowPosition( 0, 0 );
	
#ifdef RENDER
	exporter.startExport();
#else
	
#endif

}

void ofApp::load_svg(){
	{
		/*
		 *      SVG load
		 */
#ifdef OPEN_DIALOG
		string path = "";
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
#else
		string path = "svg/A.svg";
#endif
		
		svg.load( path );
		win.x = round( svg.getWidth()) ;
		win.y = round( svg.getHeight() );
		cout << "svg : " << win << endl;
		
		int n = svg.getNumPath();
		if( n==0 ) ofExit();
		
		for (int i=0; i<n; i++) {
			ofPath &p = svg.getPathAt(i);
			if( p.getStrokeColor().r == 255){
				vector<ofPolyline> polys = p.getOutline();
				for (int j=0; j<polys.size(); j++) {
					const vector<ofVec3f> &vs = polys[j].getVertices();
					if( vs.size() == 2 ){
						ofVec3f st = vs[0];
						ofVec3f end = vs[1];
						ofVec3f dir = end-st;
						gAngle = dir.angle(ofVec3f(1,0,0) );
						if(dir.y > 0)
							gAngle = -gAngle;
						start = st;
						
						max_dist = dir.length();
						cout << "gAngle : " << gAngle << endl;
						cout << "start : " << start << endl;
						cout << "max_dist : " << max_dist << endl;
					}
				}
			}
		}
	}
}

void ofApp::add_particle(){
	
	// Add Particle
	int index = rf_points.getNumVertices();
	rf_points.addVertex( ofVec3f(0,0,0) );

	int w = img.getWidth();
	int h = img.getHeight();
	int x = (index*300) % w;
	int y = index / w;
	x%=w; y%=h;
	ofFloatColor c = img.getPixelsRef().getColor(x, y);
	c.a = 0.2;
	rf_points.addColor( c );

	// Add Spline
	ofxSimpleSpline ss;
	ss.subdivisions = 20;
	int cv_num = round( ofRandom(3, 7));
	ss.cv.resize( cv_num );
	ss.cv[0] = ofVec3f(0,0,0);
	for(int i=1; i<cv_num; i++){
		ss.cv[i] = ofVec3f(0,0,0);
	}
	ss.update();
	
	int nv = ss.lineVbo.getNumVertices();
	vector<ofFloatColor> cols(nv);
	ss.lineVbo.addColors( cols );

	splines.push_back( ss );
	
	// Add Vine
	Vine v;
	v.radius = ofRandom(2, 20);
	v.angle = ofRandom(-360, 360); //ofSignedNoise(1, index*0.4) * 360.0;
	v.num_cv = cv_num;
	
	for( int i=0; i<v.num_cv; i++ ){
		v.speeds.push_back( ofVec3f(0,0,0) );
		v.speeds[i].x = ofNoise( i, index*0.1 ) * 40.0;
		v.speeds[i].y = ofSignedNoise( i, index*0.1 ) * 5.0;
		
		if( ofNoise(i) > 0.8 )
			v.speeds[i].x *= ofRandom(1.2, 2);
		if( ofRandomuf() > 0.8 )
			v.speeds[i].y *= ofRandom(1.2, 2);
		
		v.accels.push_back( ofVec3f(0,0,0) );
	}
	
	vines.push_back(v);
}

void ofApp::update_particle(){
	
	vector<ofVec3f> & vs = rf_points.getVertices();
	const vector<ofFloatColor> & cols = rf_points.getColors();
	
	// Move Spline Top
	for (int i=0; i<vs.size(); i++ ) {

		// TOP
		int topId = vines[i].num_cv-1;
		vines[i].past_top = splines[i].cv[topId];

		vines[i].accels[topId].x = ofNoise( 1, ofGetFrameNum()*0.01, i*0.5 );
		vines[i].accels[topId].y = ofSignedNoise( 2, ofGetFrameNum()*0.01, i*0.5 ) * 0.5;
		vines[i].speeds[topId] += vines[i].accels[topId];
		vs[i] += vines[i].speeds[topId];
		vines[i].speeds[topId] *= 0.75;
		splines[i].cv[topId] = vs[i];
		
		
		ofVec3f dpos = vs[i] - vines[i].past_top;
		dpos *= 0.1;
		
		// 1 ~ TOP-1
		for( int j=1; j<vines[i].num_cv-1; j++ ){
			splines[i].cv[j] += dpos;
			splines[i].cv[j].x += ofNoise(2, j, ofGetFrameRate()*0.1, i ) * 2.0;
			splines[i].cv[j].y += ofSignedNoise(j, ofGetFrameRate()*0.1, i ) * 0.75;
		}
	
		// update Color
		vector<ofFloatColor> & cols = splines[i].lineVbo.getColors();
		const vector<ofFloatColor> & cols_p = rf_points.getColors();
		for( int j=0; j<cols.size(); j++ ){
			cols[j] = cols_p[i];
			cols[j].a = 0.01 + (float)j/cols.size() * 0.7;
		}

		
		// update spline
		splines[i].update();
		
		vines[i].age++;
	}
}

void ofApp::update(){
	if( !bStart ) return;
	
	frame++;
	
    r_lines.clearVertices();
    r_lines.clearColors();
	
	add_particle();
	update_particle();
}

void ofApp::draw(){
	
	const vector<ofVec3f> & vs = rf_points.getVertices();
	const vector<ofFloatColor> & cols = rf_points.getColors();

    exporter.begin();{
		
		ofEnableAlphaBlending();
		ofBackground( 255 );
		ofPushMatrix();{
			
			
			ofTranslate( 10, exporter.getFbo().getHeight()/2 );

			if( !exporter.isExporting() )
				ad_util::draw_axis();
            
#ifdef SPLINE
			ofSetColor(255);
            for( int i=0; i<splines.size(); i++ ){
                splines[i].draw();
            }
#endif
			
#ifdef CP_POINT
            ofSetColor(5, 100);
            for( int i=0; i<splines.size(); i++ ){
                glPointSize(1);
                glBegin( GL_POINTS );
                for (int j=0; j< splines[i].cv.size(); j++) {
                    const ofVec3f &v = splines[i].cv[j];
                    glVertex3f( v.x, v.y, v.z );
                }
                glEnd();
            }
#endif

#ifdef CIRCLE
			glLineWidth(1);
			r_lines.draw();

			for( int i=0; i<vs.size(); i++ ){
				ofNoFill();
				float rate = vines[i].age/550.0;
				int radius = vines[i].radius + ofRandom(1, 5);
				float angle = vines[i].angle + ofRandomf() * 360.0;
				ofVec3f dir = ofVec3f(cos(angle), sin(angle), 0) * radius;

				ofFloatColor c = cols[i]*(1.0-rate) + orange*rate;
				ofSetColor( c*0.3, 200 );
				ofLine( vs[i], vs[i] + dir );

				ofSetColor( c, 200 );
				ofCircle(vs[i], radius );
				
				if( ofRandomuf()>0.95 ){
					dir *= ofRandom(2.0, 3.0);
					ofSetColor( c*0.3, 200 );
					ofLine( vs[i], vs[i] + dir );
					
					ofSetColor( c, 200 );
					ofCircle(vs[i]+dir, radius*ofRandom(0.1, 0.4) );
				}
			}
#endif

			// RF Particles
			ofDisableAntiAliasing();
            glPointSize( 1 );
			rf_points.draw();
            
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
	ss << "particles : " << rf_points.getNumVertices() << "\n";
	ss << "\n";
    ofDrawBitmapString(ss.str(), 20, 20 );
	ofPopMatrix();
}

void ofApp::keyPressed( int key ){
	
	switch( key ) {
		case ' ':
			bStart = !bStart;
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
