#include "ofApp.h"

void ofApp::setup(){

    ofSetFrameRate( 60 );
	ofSetVerticalSync( true );
    cam.setDistance( 150 );

    bDraw_info = true;
    rot_angle = 0;
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

    world.setup();
    
    // world.enableCollisionEvents();
    // ofAddListener( world.COLLISION_EVENT, this, &ofApp::onCollision );
    world.setCamera( &cam );
    world.setGravity( ofVec3f(0, 0, 0) );
    
    sphereShape = ofBtGetSphereCollisionShape( 1.0 );
    for( int i=0; i<100; i++){
        add_dust( 100 );
    }
}

void ofApp::add_dust( int num ){
    
    ofVec3f center( ofRandom(-200,200), ofRandom(-200, 200), ofRandom(-200, 0) );

    for( int i = 0; i<num; i++ ) {
        ofxBulletSphere * s= new ofxBulletSphere();
        s->init( sphereShape );

        //ofVec3f pos( ofRandom(-1.0,1.0), ofRandom(-1.0,1.0), ofRandom(-1.0,1.0) );

        float angle = ofRandom( 0, 360 );
        float length = ofRandom(0.01, 100.0);

        float noise = 0;
        float amp = 0.5;
        float index = i;
        for( int j=0; j<4; j++){
        
            noise += ofSignedNoise(index, 1) * amp;
            amp *= 0.5;
            index *= 2.0;
        }
        noise *= 10.0;
        ofVec3f pos( length*noise, 0, 0);
        pos.rotate( angle, ofVec3f(ofRandom(1.0), ofRandom(1.0),ofRandom(1.0)) );
        pos += center;
        
        float mass = ofRandom( 0.01, 3.0 );
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
    
    ofVec3f attractor( 10, 10, 50 );
    attractor.rotate( ofGetFrameNum()/4, ofVec3f(0.5, 0.3, 0.1) );
    ofVec3f diff;
    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f pos = shapes[i]->getPosition();
        diff = attractor - pos;
        float dist2 = diff.lengthSquared();
        float power = (1.0/dist2) * 50000.0;

        ofVec3f force( 0,0,0 );
        
        if( 1 ){
            float noise = 0;
            float amp = 0.5;
            for( int j=0; j<4; j++ ){
                noise += ofNoise( pos.x, pos.y, pos.z, frame*0.001) * amp;
                amp *= 0.5;
            }
            force += diff.normalized() * power;
        }
       
#pragma mark ADD_PERLIN_NOISE
        if( 0 ){
            ofVec3f noise; noise.zero();
            float amp = 0.5;
            for( int j=0; j<4; j++ ){
                noise.x += ofSignedNoise( pos.x, frame*0.001 ) * amp;
                noise.y += ofSignedNoise( pos.y, frame*0.001 ) * amp;
                noise.z += ofSignedNoise( pos.z, frame*0.001 ) * amp;
                amp *= 0.5;
            }
            
            force += noise*5.0;
        }

        shapes[i]->applyCentralForce( force );
        points.setVertex( i, pos );

    }
    
    lines.clear();
    int num_line = 3;   // per point
    for( int i=0; i<shapes.size(); i++ ){
//        if(ofRandom(1.0)>0.1)
//            continue;
        ofVec3f pos1 = shapes[i]->getPosition();
        multimap<float, ofVec3f> near_p;
        pair<float, ofVec3f> pair1( 999999999999, ofVec3f(0,0,0) );
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
            lines.addVertex( pos1 );
            lines.addVertex( pos2 );
            lines.addColor( ofFloatColor( ofRandom(0.2,0.4), ofRandom(0.2, 0.4)) );
            lines.addColor( ofFloatColor( ofRandom(0.2,0.4), ofRandom(0.2, 0.4)) );
        }
    }
	world.update();
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofBackground( 0 );

    cam.begin();{
        ofRotate( rot_angle+=0.03, 0, 1, 0 );
        ofSetColor( 255 );
        glLineWidth( 1 );
        lines.draw( OF_MESH_WIREFRAME );
        
        glPointSize( 2 );
        points.ofMesh::draw( OF_MESH_POINTS );
    }cam.end();

    saver.save();
    
    draw_info();
}

void ofApp::draw_info(){
    if ( !bDraw_info)
        return;

    ofSetColor( 255 );
	stringstream ss;
	ss << "fps: " << ofToString( ofGetFrameRate(),0 ) << endl;
    ss << "frame     : " << ofGetFrameNum() << "\n";
    ss << "cur frame : " << saver.frame_cur << "\n";
    ss << "end frame : " << saver.frame_end << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
    ss << "num shapes: " << shapes.size() << endl;
    ss << "I key:   toggle info " << endl;
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
