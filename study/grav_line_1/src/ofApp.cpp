#include "ofApp.h"

void ofApp::setup(){

    ofSetFrameRate( 60 );
	ofSetVerticalSync( true );
    cam.setDistance( 400 );

    bDraw_info = true;
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );

	p2p.setUsage( GL_DYNAMIC_DRAW );
	p2p.setMode( OF_PRIMITIVE_LINES );

	
	attrs.setUsage( GL_DYNAMIC_DRAW );
	attrs.setMode( OF_PRIMITIVE_POINTS );
	
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

    world.setup();
    
    world.setCamera( &cam );
    world.setGravity( ofVec3f(0, 0, 0) );
    
    sphereShape = ofBtGetSphereCollisionShape( 1.0 );

	float w = ofGetWidth();

//	for( int i=0; i<10; i++){
//        add_dust( 10 );
//    }

	int num = w/2;
	for(int i=0; i<num; i++){
		float x = ((float)i/(num)-0.5) * 700.0;
		{
			ofVec3f pos( x, 10, 0);
			float mass = 3.0 - abs(x)/600.0*2;
			ofxBulletSphere * s= new ofxBulletSphere();
			s->init( sphereShape );
			s->create( world.world, pos, mass );
			s->setActivationState( DISABLE_DEACTIVATION );
			s->add();
			shapes.push_back( s );
			points.addVertex( pos );
		}
	}
	
	for(int i=0; i<num; i++){
		float x = ((float)i/(num)-0.5) * 700.0;
		{
			ofVec3f pos( x, -10, 0);
			float mass = 10.0 - abs(x)/500.0*9.8;
			ofxBulletSphere * s= new ofxBulletSphere();
			s->init( sphereShape );
			s->create( world.world, pos, mass );
			s->setActivationState( DISABLE_DEACTIVATION );
			s->add();
			shapes.push_back( s );
			points.addVertex( pos );
		}
	}
	
	w *= 1.1;
	int step = 50;
	float adder = w/step;
	for( int i=0; i<step; i++ ){
		float x = i * adder - w/2;
		attrs.addVertex( ofVec3f(x, ofNoise(i, x)*50, 0) );
		attrs.addColor( ofFloatColor(1,0,0) );
	}
}

void ofApp::add_dust( int num ){
    
	ofVec3f center( ofRandomf(), ofRandomf(), ofRandomf() );
	center *= 50.0;

    for( int i = 0; i<num; i++ ) {
        ofxBulletSphere * s= new ofxBulletSphere();
        s->init( sphereShape );

        ofVec3f pos( ofRandom(-1.0,1.0), ofRandom(-1.0,1.0), ofRandom(-1.0,1.0) );
		pos *= 100;
//        float angle = ofRandom( 0, 360 );
//        float length = ofRandom(0.01, 100.0);
//
//        float noise = 0;
//        float amp = 0.5;
//        float index = i;
//        for( int j=0; j<4; j++){
//        
//            noise += ofSignedNoise(index, 1) * amp;
//            amp *= 0.5;
//            index *= 2.0;
//        }
//        noise *= 5.0;
//        ofVec3f pos( length*noise, 0, 0);
//        pos.rotate( angle, ofVec3f(ofRandom(1.0), ofRandom(1.0),ofRandom(1.0)) );
        pos += center;
        
        float mass = ofNoise( i ) + 1.0;
        s->create( world.world, pos, mass );
        s->setActivationState( DISABLE_DEACTIVATION );
        s->add();
        shapes.push_back( s );
        
        points.addVertex( pos );
        
        ofFloatColor c;
        c.set( ofRandom(1.0), ofRandom(1.0) );
        c = c*0.7 + 0.1;
        points.addColor( c );
    }
}

void ofApp::update(){

	float frame = ofGetFrameNum();
	
	for( int i=0; i<attrs.getNumVertices(); i++ ){
		ofVec3f ap = attrs.getVertex(i);
		ofVec3f n;
		n.x = ofSignedNoise(i, ap.x, frame*0.0001)*6.0;
		n.y = ofSignedNoise(i, ap.y, frame*0.0001)*6.0;
		n.z = ofSignedNoise(i, ap.z, frame*0.0001)*6.0;
		float dist = ap.distance(ofVec3f(ap.x, 0,0) );
		ap.rotate( 100.0/dist, ofVec3f(1,0,0) );
		attrs.setVertex(i, ap+n);
	}
	
    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f pos = shapes[i]->getPosition();
		ofVec3f force( 0,0,0 );
		for( int j=0; j<attrs.getNumVertices(); j++ ){
			ofVec3f diff = attrs.getVertex(j) - pos;
			float dist2 = diff.lengthSquared();
			if(dist2>100){
				float power = (1.0/dist2) * 100000.0;
				force += diff.normalized() * power;
			}
		}
		
        shapes[i]->applyCentralForce( force );

		pos.x = ((float)i/shapes.size()-0.5) * ofGetWidth()/2;;
		pos.z *= 0.1;
		points.setVertex( i, pos );
    }
	
	lines.clear();
	for( int i=0; i<shapes.size(); i++ ){
		float noise = ofNoise(i*0.01);
		int num_line =  1 + noise*10.0;   // per point

		//        if(ofRandom(1.0)>0.1)
		//            continue;
		ofVec3f pos1 = shapes[i]->getPosition();
		multimap<float, ofVec3f> near_p;
		pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
		for( int line=0; line<num_line; line++ ){
			near_p.insert( pair1 );
		}
		
		for( int j=i+1; j<shapes.size(); j++ ){
			
			ofVec3f pos2 = shapes[j]->getPosition();
			float dist = pos1.distance( pos2 );
			
			multimap<float, ofVec3f>::iterator itr = near_p.end();
			
			itr--;
			if( dist < itr->first ){
				std::pair<float, ofVec3f> pair2( dist, pos2 );
				near_p.insert( pair2 );
				
				multimap<float, ofVec3f>::iterator end = near_p.end();
				near_p.erase( --end );
			}
		}
		multimap<float, ofVec3f>::iterator itr = near_p.begin();
		
		for(; itr!=near_p.end(); itr++ ){
			ofVec3f &pos2 = itr->second;
			if(pos2.x == -12345) continue;
			
			float d = pos1.distance(pos2);
			if( d<3 || 12+noise*60.0<d  ) continue;
			lines.addVertex( pos1 );
			lines.addVertex( pos2 );
			ofFloatColor c;
			c.setHsb( ofNoise(i*0.01)*0.6+0.7, ofRandom(0.1,0.8), ofNoise(i*0.01)*0.8+0.1);
			c.a = ofNoise(c.r*0.001)*0.7+ 0.1;
			lines.addColor( c );
			lines.addColor( c );
		}
	}
	
	p2p.clear();
	for( int i=0; i<points.getNumVertices(); i++ ){
		
		ofVec3f p1 = points.getVertex(i);
		if( ofRandomf()<0.3) continue;
		p1.z = 0;
		p1.y = 1;
		p2p.addVertex(p1);
		p1.y = 0;
		p2p.addVertex(p1);
		ofFloatColor c( 0.1, 0.2, 1, 0.6);
		p2p.addColor( c );
		p2p.addColor( c );
	}
	
	world.update();
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    //ofEnableAntiAliasing();
    //ofEnableSmoothing();
    ofBackground( 0 );

    cam.begin();{
        ofSetColor( 195, 10, 110 );
        glLineWidth( 1 );
        lines.draw();

		glLineWidth( 1 );
		p2p.draw();
		
        glPointSize( 1 );
        points.draw();
		
		glPointSize( 1 );
		attrs.draw();
    }cam.end();

    saver.save();
    
    draw_info();
}

void ofApp::draw_info(){
    if ( !bDraw_info)
        return;

    ofSetColor( 255 );
	stringstream ss;
	ss << "fps: " << ofToString( ofGetFrameRate(),0 ) << "\n";
    ss << "frame     : " << ofGetFrameNum() << "\n";
    ss << "cur frame : " << saver.frame_cur << "\n";
    ss << "end frame : " << saver.frame_end << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
    ss << "num shapes: " << shapes.size() << "\n";
    ss << "I key:   toggle info " << "\n";
	ss << "\nCam\n";
	ss << "pos       : " << cam.getPosition().x << ", " << cam.getPosition().y << "," << cam.getPosition().z << "\n";
	ss << "target pos: " << cam.getTarget().getPosition().x << ", " << cam.getTarget().getPosition().y << "," << cam.getTarget().getPosition().z << "\n";
	ss << "dist      : " << cam.getDistance() << "\n";
	ss << "fov       : " << cam.getFov() << "\n";
	ss << "\n";

	ofDrawBitmapString( ss.str().c_str(), 10, 10 );
}

void ofApp::onCollision( ofxBulletCollisionData& cdata ){

	for( int i = 0; i < shapes.size(); i++) {
		if( *shapes[i] == cdata ){
		}
	}
}

void ofApp::keyPressed( int key ) {

	switch( key ) {
        case 'a':
            add_dust( 1000 );
            break;

        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'I':
            bDraw_info = !bDraw_info;
            break;
        
        case 'S':
            saver.start( ofGetTimestampString(), "physic_3_", 3000 );
            break;
            
        default:
			break;
	}
}
