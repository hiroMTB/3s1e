#include "ofApp.h"

void ofApp::setup(){

    ofSetFrameRate( 60 );
	ofSetVerticalSync( true );
    cam.setDistance( 400 );

    bDraw_info = true;
    bGrav = true;
    
    setup_physics();
    setup_particles();
    setup_attrs();
}

void ofApp::setup_physics(){
    world.setup();
    world.setCamera( &cam );
    world.setGravity( ofVec3f(0, 0, 0) );
    world.enableCollisionEvents();
    ofAddListener(world.COLLISION_EVENT, this, &ofApp::onCollision);

    for( int i=1; i<=10; i++ ){
        sphereShapes.push_back( ofBtGetSphereCollisionShape( 1.0 + i*i*i*i*0.0005) );
    }
}

void ofApp::setup_particles(){
    
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

    float w = ofGetWidth();

	int num = w*0.4;
    int ydist = 0;
	for(int i=0; i<num; i++){
		float x = ((float)i/(num)-0.5) * 700.0;
		{
			ofVec3f pos( x, ydist, 0);
			ofxBulletSphere * s= new ofxBulletSphere();

            float size = ofNoise(x*0.01, i*0.0001) * 10.0;
            size = ofMap(size, 0, 10.0, 1, 9);
            float mass = 5.0 - ofNoise(x*0.01+10, i*0.001) * 3.0;
            float friction = ofNoise(x*0.00001+1, i*0.01+10.0) * 10.0;
            s->init( sphereShapes[(int)size] );
			s->create( world.world, pos, mass );
            s->setFriction( friction );
			s->setActivationState( DISABLE_DEACTIVATION );
			s->add();
			shapes.push_back( s );
			points.addVertex( pos );
            points.addColor( ofFloatColor(0) );
		}
	}
	
	for(int i=0; i<num; i++){
		float x = ((float)i/(num)-0.5) * 700.0;
		{
			ofVec3f pos( x, -ydist, 0);
            ofxBulletSphere * s= new ofxBulletSphere();

            float size = ofNoise(4+x*0.005, i*0.000001) * 10.0;
            float mass = 5.0 - ofNoise(x*0.01+10, 3+i*0.001) * 3.0;
            float friction = ofNoise(x*0.001+1, i*0.01+10.0) * 10.0;

            size = ofMap(size, 0, 10.0, 1, 9);
            s->init( sphereShapes[(int)size] );
            s->create( world.world, pos, mass );
            s->setFriction( friction );
			s->setActivationState( DISABLE_DEACTIVATION );
			s->add();
			shapes.push_back( s );
			points.addVertex( pos );
            points.addColor( ofFloatColor(0) );
        }
	}
}

void ofApp::setup_attrs(){
    attrs.setUsage( GL_DYNAMIC_DRAW );
    attrs.setMode( OF_PRIMITIVE_POINTS );

    float w = ofGetWidth();
    w *= 2.5;
    int step = 60;
    float ydist = 100;
    float adder = w/step;
    for( int i=0; i<step; i++ ){
        float x = i * adder - w/2;
        ofVec3f pos( x, ydist, 0);
        pos.rotate( ofNoise(i, x*0.001)*360.0, ofVec3f(1,0,0) );
        attrs.addVertex( pos );
        attrs.addColor( ofFloatColor(1,0,0) );
    }
}

void ofApp::update(){
    collision.clear();
    
	frame = ofGetFrameNum();
    
    if(frame < 30 ) return;
        
    if( bGrav ) apply_grav();
    
    update_lines();
    update_points();
    update_attrs();
    
	world.update();
}

void ofApp::update_attrs(){
    for( int i=0; i<attrs.getNumVertices(); i++ ){
        ofVec3f ap = attrs.getVertex(i);
        float dist = ap.distance(ofVec3f(ap.x, 0,0) );
        ap.rotate( 200.0/dist, ofVec3f(1,0,0) );
        attrs.setVertex(i, ap);
    }
}

void ofApp::update_points(){
    
    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f sp = shapes[i]->getPosition();
        ofVec3f pp(sp);
        sp *= 0.01;
        pp.y += ofSignedNoise( sp.x, sp.y, frame * 0.001);
        pp.y += sp.y;
        points.setVertex( i, pp );
        ofFloatColor c;
        c.setHsb( ofNoise(i*0.01)*0.3+0.8, ofRandom(0.1,0.8), ofNoise(i*0.001)*0.4+0.4);
        c.a = ofNoise(c.r*0.0001, c.g*0.001, frame*0.001)*0.6 + 0.1;
        points.setColor( i, c);
    }
}

void ofApp::update_lines(){
    lines.clear();
    for( int i=0; i<shapes.size(); i++ ){
        
        //        if(ofRandom(1.0)>0.1)
        //            continue;
        ofVec3f pos1 = shapes[i]->getPosition();

        float noise = ofNoise(pos1.x*0.1, pos1.y*0.01, i*0.0001);
        int num_line = 3 + noise*noise*8.0;
        
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
            if( d<1 || 12+noise*100.0<d  ) continue;
            lines.addVertex( pos1 );
            lines.addVertex( pos2 );
            ofFloatColor c;
            c.setHsb(   ofNoise(i*0.1,      frame*0.001)*0.6 + 0.55,
                        ofNoise(10+i*0.2,   frame*0.001)*0.8 + 0.3,
                        ofNoise(2+i*3,      frame*0.001)*0.5 + 0.3
                     );
            c.a = ofNoise(noise+0.3, c.r*0.001, frame*0.001)*0.8 + 0.1;
            lines.addColor( c );
            lines.addColor( c );
        }
    }
}

void ofApp::apply_grav(){
    
    for( int i=0; i<shapes.size(); i++ ){
        
        ofVec3f pos = shapes[i]->getPosition();
        ofVec3f force( 0,0,0 );

        for( int j=0; j<attrs.getNumVertices(); j++ ){
            
            if( i%2 == j%2) continue;

            ofVec3f diff = attrs.getVertex(j) - pos;
            float dist2 = diff.lengthSquared();
            if(dist2>30){
                float power = (1.0/dist2) * 2400000.0;
                force += diff.normalized() * power;
            }
        }

        // vertical force
        {
            ofVec3f d = (ofVec3f(pos.x, 0, 0) - pos);
            float dist2 = d.lengthSquared();
            force += d.normalized() * dist2*0.05;
        }
        
        force *= bNegativeGrav ? -1 : 1;
        shapes[i]->applyCentralForce( force );
    }
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofBackground( 0 );

    cam.begin();{
        ofNoFill();
        ofSetLineWidth( 1 );
        for( int i=0; i<collision.size(); i++ ){
            ofSetColor( 200, 80);
            ofVec3f &p = collision[i];
            ofCircle( p, 1 );
        }
    
        ofSetColor( 195, 10, 110 );
        glLineWidth( 1 );
        lines.draw();
		
        glPointSize( 1 );
        points.draw();
		
        //glPointSize( 1 );
		//attrs.draw();
    }cam.end();

    ofSetColor(0);
    ofSetLineWidth( 4 );
    ofLine(0, ofGetHeight()/2, ofGetWidth(), ofGetHeight()/2);
    
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
    collision.insert( collision.end(), cdata.worldContactPoints1.begin(), cdata.worldContactPoints1.end() );
}

void ofApp::keyPressed( int key ) {

	switch( key ) {
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'I':
            bDraw_info = !bDraw_info;
            break;
        
        case 'S':
            saver.start( ofGetTimestampString(), "", 3000 );
            break;

        case 'g':
            bGrav = !bGrav;
            break;

        case 'n':
            bNegativeGrav = !bNegativeGrav;
            break;
            
        default:
			break;
	}
}
