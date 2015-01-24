#include "ofApp.h"
#include "ad_util.h"


//#define OPEN_DIALOG
#define RENDER
#define SPLINE
#define CP_POINT
#define CIRCLE

ofColor ofApp::orange = ofColor( 255, 80, 0, 200);

void ofApp::setup(){
	bStart = true;
	frame=0;
	rot = 0;
	ofSetVerticalSync( true );
	ofSetFrameRate( 25 );
	ofSetCircleResolution( 100 );
	
    //load_svg( ad_util::data_path + "/svg/solo/v4/C.svg" );
    load_svg();
    setup_window( win.x, win.y );
    
	bool ok = img.loadImage( ad_util::data_path  + "img/rk/Mx80_1_org.jpg");
	if( !ok ) cout << "image load failer" << endl;
	
    reset();
    
#ifdef RENDER
    ofSetWindowShape(200, 400);
    ofSetWindowPosition(0, 0);
    exporter.startExport();
#else
    ofSetWindowShape( win.x, win.y);
    ofSetWindowPosition( 0, 0 );
#endif

}

void ofApp::reset(){

    frame = 0;
    
    // clear
    splines.clear();
    vines.clear();
    
    rf_points.clear();
    rf_points.setUsage( GL_DYNAMIC_DRAW );
    rf_points.setMode( OF_PRIMITIVE_POINTS );
    
    r_lines.clear();
    r_lines.setUsage( GL_DYNAMIC_DRAW );
    r_lines.setMode( OF_PRIMITIVE_LINES );

    // Initial Particle
    for (int i=0; i<10; i++) {
        add_particle();
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
	int cv_num = round( ofRandom(5, 8));
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
    v.angle = index % (int)(round)(spreadAngle+1);
	v.num_cv = cv_num;
	
	for( int i=0; i<v.num_cv; i++ ){
		v.speeds.push_back( ofVec3f(0,0,0) );
        v.speeds[i].x = 2.0*(float)i/v.num_cv + ofNoise( 1, i*0.5 + index*0.1 )*50.0;
		v.speeds[i].y = ofRandomf() * 45.0;
		
		if( ofNoise(2, i+index) > 0.8 )
			v.speeds[i].x *= ofRandom(1.2, 2.3);
		if( ofRandomuf() > 0.8 )
			v.speeds[i].y *= ofRandom(1.5, 2);
		
        v.speeds[i].x *= spread_vec.x;
        v.speeds[i].y *= spread_vec.y;

        if( i== v.num_cv-1){
            v.speeds[i] *= 1.2;
        }

        v.accels.push_back( ofVec3f(0,0,0) );

        v.phase.push_back( ofRandom(-3.14, 3.14) );
        v.freq.push_back( ofRandom(-1.0, 1.0) );
    }
    
	vines.push_back(v);
}

void ofApp::update(){
	if( !bStart ) return;
	
	frame++;
	
    r_lines.clearVertices();
    r_lines.clearColors();
	
    for (int i=0; i<spreadAngle*0.5; i++) {
        add_particle();
    }
	
    update_particle();
}

void ofApp::update_particle(){
    
    vector<ofVec3f> & vs = rf_points.getVertices();
    const vector<ofFloatColor> & cols = rf_points.getColors();
    
    // Move Spline Top
    for (int i=0; i<vs.size(); i++ ) {
        
        // TOP
        {
            int topId = vines[i].num_cv-1;
            if(topId < 0) break;
            vines[i].past_top = splines[i].cv[topId];
            
            float phase_adder = 1.0/25.0 * vines[i].freq[topId];
            vines[i].phase[topId] += phase_adder;
            
            ofVec3f add;
            add.x = 0.05 + ofNoise( ofGetFrameNum()+i, cols[i].r*0.03, i*1.2 );
            add.y = sin( vines[i].phase[topId]  ) * 0.6;
            add.x *= spread_vec.x;
            add.y *= spread_vec.y;
            
            vines[i].accels[topId] = add;
            vines[i].speeds[topId] += vines[i].accels[topId];
            vs[i] += vines[i].speeds[topId];
            splines[i].cv[topId] = vs[i];
            
            vines[i].speeds[topId] *= 0.85;
        }
        
        // 1 ~ TOP-1
        for( int j=1; j<vines[i].num_cv-1; j++ ){
            
            float phase_adder = 1.0/25.0 * vines[i].freq[j];
            vines[i].phase[j] += phase_adder;
            
            
            float rate = (float)j/vines[i].num_cv;
            ofVec3f add;
            add.x = 0.02*rate + ofNoise(2, j, ofGetFrameRate(), i*1.5 );
            add.y = sin( vines[i].phase[j]  );

            float ampy = 0.1 + 0.3*rate;
            float ampx = 0.4;

            add.x *= ampx;
            add.y *= ampy;
            add.x *= spread_vec.x;
            add.y *= spread_vec.y;

            vines[i].speeds[j] += add;
            splines[i].cv[j] += vines[i].speeds[j];
            vines[i].speeds[j] *= 0.85;
        }
        
        // update Color
        vector<ofFloatColor> & cols = splines[i].lineVbo.getColors();
        const vector<ofFloatColor> & cols_p = rf_points.getColors();
        for( int j=0; j<cols.size(); j++ ){
            cols[j] = cols_p[i];
            cols[j].a = 0.01 + (float)j/cols.size() * 0.5;
        }
        
        
        // update spline
        splines[i].update();
        
        vines[i].age++;
    }
}

void ofApp::draw(){
	
	const vector<ofVec3f> & vs = rf_points.getVertices();
	const vector<ofFloatColor> & cols = rf_points.getColors();

    exporter.begin();{
		
		ofEnableAlphaBlending();
		ofBackground( 255 );
        //bg_shape.draw(0, 0);

        ofPushMatrix();{
            ofTranslate( start );
            ofRotate( -gAngle );
            
#ifdef SPLINE
			ofSetColor(255);
            for( int i=0; i<splines.size(); i++ ){
                ofPushMatrix();
                ofRotateZ( vines[i].angle -spreadAngle*0.5 );
                splines[i].draw();
                ofPopMatrix();
            }
#endif
			
#ifdef CP_POINT
            ofSetColor(225, 20);
            for( int i=0; i<splines.size(); i++ ){
                ofPushMatrix();
                ofRotateZ( vines[i].angle-spreadAngle*0.5 );

                glPointSize(1);
                glBegin( GL_POINTS );
                for (int j=0; j< splines[i].cv.size(); j++) {
                    const ofVec3f &v = splines[i].cv[j];
                    glVertex3f( v.x, v.y, v.z );
                }
                glEnd();
                ofPopMatrix();
            }
#endif

#ifdef CIRCLE
			glLineWidth(1);
			r_lines.draw();

			for( int i=0; i<vs.size(); i++ ){
                ofPushMatrix();
                ofRotateZ( vines[i].angle-spreadAngle*0.5);

                if( !exporter.isExporting() ){
                    ofSetLineWidth(1);
                    ofSetColor(0,0,255);
                    ofLine(0,0, 100,0);
                }
                
                ofNoFill();
				float rate = vines[i].age/1350.0;
				int radius = vines[i].radius + ofRandom(1, 5);
				float angle =  ofRandomf() * 360.0;
				ofVec3f dir = ofVec3f(cos(angle), sin(angle), 0) * radius;

				ofFloatColor c = cols[i]*(1.0-rate) + orange*rate;
				ofSetColor( c*0.3, 200 );
				ofLine( vs[i], vs[i] + dir );

				ofSetColor( c, 200 );
                ofNoFill();
				ofCircle(vs[i], radius );
				
				if( ofRandomuf()>0.9 ){
					dir *= ofRandom(2.0, 3.0);
					ofSetColor( c*0.3, 200 );
					ofLine( vs[i], vs[i] + dir );
					
                    ofFill();
					ofSetColor( c, 150 );
					ofCircle(vs[i]+dir, 2 );
				}
                ofPopMatrix();
			}
#endif

            
            for( int i=0; i<vs.size(); i++ ){
                ofPushMatrix();
                ofRotateZ( vines[i].angle-spreadAngle*0.5);
                ofSetColor( cols[i] );
                glPointSize(1);
                glBegin( GL_POINTS );
                int last_index = splines[i].cv.size();
                const ofVec3f &v = splines[i].cv[last_index];
                glVertex3f( v.x, v.y, v.z );
                glEnd();
                ofPopMatrix();
            }
            
			// RF Particles
			//ofDisableAntiAliasing();
            //glPointSize( 1 );
			//rf_points.draw();
            
            if( !exporter.isExporting() ){
                ad_util::draw_axis();
                ofNoFill();
                ofSetColor(255, 0, 0);
                ofCircle(0, 0, 50);
            }

            ofFill();
		}ofPopMatrix();

        mask.draw();

        if( !exporter.isExporting() ){
            red_guide.draw(0,0);
            green_guide.draw(0,0);
            blue_guide.draw(0,0);
        }

	} exporter.end();

    ofBackground(40);
    if( !exporter.isExporting() ){
        ofSetColor(255);
        exporter.draw(0, 0);
    }
	draw_info();
}

