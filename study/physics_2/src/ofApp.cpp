#include "ofApp.h"

void ofApp::setup(){

    ofSetFrameRate( 60 );
	ofSetVerticalSync( true );
    cam.setDistance( 200 );

    bAttract = true;
    bDebug_draw = false;
    bViz1_draw = false;
    bDraw_info = true;
    
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

	viz1.setUsage( GL_DYNAMIC_DRAW );
	viz1.setMode( OF_PRIMITIVE_POINTS );

    attrs.setUsage( GL_DYNAMIC_DRAW );
    attrs.setMode( OF_PRIMITIVE_POINTS );

    trail.setUsage( GL_DYNAMIC_DRAW );
    trail.setMode( OF_PRIMITIVE_POINTS );
    
    world.setup();
    
    // world.enableCollisionEvents();
    // ofAddListener( world.COLLISION_EVENT, this, &ofApp::onCollision );
    world.setCamera( &cam );
    world.setGravity( ofVec3f(0, 0, 0) );
    
    sphereShape = ofBtGetSphereCollisionShape( 1.5 );

    add_dust( 3000, ofVec3f( 0, -30, 0), -20.0 );
    add_dust( 3000, ofVec3f( 0, 30, 0), -20.0 );
    
    // planet
    attrs.addVertex( ofVec3f(0,0,0) );
    attrs.addColor( ofFloatColor(1,0,0) );
    
    // attr 1
    attrs.addVertex( ofVec3f(0,0,0) );
    attrs.addColor( ofFloatColor(0,1, 0) );

    // attr 2
    attrs.addVertex( ofVec3f(0,0,0) );
    attrs.addColor( ofFloatColor(0,0,1) );
    
    make_wall();
    
    trail_head = 0;
}

void ofApp::make_wall(){
    ofVec3f startLoc;
    ofPoint dimens;
    float boundsWidth_x = 200.0;
    float boundsWidth_y = 200.0;
    float boundsWidth_z = 200.0;
    
    for(int i = 0; i < 6; i++) {
        
        float pos_x = ofRandom(-1.0,1.0) * 50.0;
        float pos_y = ofRandom(-1.0,1.0) * 50.0;
        float pos_z = ofRandom(-1.0,1.0) * 50.0;

        float thickness = ofRandom(5, 20.0);
        
        bounds.push_back( new ofxBulletBox() );
        if(i == 0) { // ground //
            startLoc.set( 0., pos_y, 0. );
            dimens.set( boundsWidth_x, thickness, boundsWidth_z );
        } else if (i == 1) { // back wall //
            startLoc.set(0, 0, pos_y);
            dimens.set(boundsWidth_x, boundsWidth_y, thickness );
        } else if (i == 2) { // right wall //
            startLoc.set( pos_x, 0, 0.);
            dimens.set(thickness, boundsWidth_y, boundsWidth_z );
        } else if (i == 3) { // left wall //
            startLoc.set( -pos_x, 0, 0.);
            dimens.set( thickness, boundsWidth_y, boundsWidth_z );
        } else if (i == 4) { // ceiling //
            startLoc.set( 0, -pos_y, 0.);
            dimens.set( boundsWidth_x, thickness, boundsWidth_z );
        } else if (i == 5) { // front wall //
            startLoc.set( 0, 0, -pos_y );
            dimens.set( boundsWidth_x, boundsWidth_y, thickness );
        }
        
        /*	void create(
                btDiscreteDynamicsWorld* a_world,
                ofVec3f a_loc,
                ofQuaternion a_rot,
                float a_mass=1.f,
                float a_sizeX=5.f,
                float a_sizeY=5.f,
                float a_sizeZ=5.f
            );
         */
        
        ofQuaternion q;
        q.makeRotate( ofRandom(-360,360), ofVec3f(ofRandom(1.0),ofRandom(1.0),ofRandom(1.0)) );
        bounds[i]->create( world.world, startLoc, q, 10000., dimens.x, dimens.y, dimens.z );
        bounds[i]->setProperties(.25, .95);
        bounds[i]->add();
    }
    
}

void ofApp::add_dust( int num, ofVec3f center, float random ){
    
    for( int i = 0; i<num; i++ ) {
        ofxBulletSphere * s= new ofxBulletSphere();
        s->init( sphereShape );

        float angle = ofRandom( 0, 360 );
        float length = ofRandom( 1, random );

        float noise = 0;
        float amp = 0.5;
        float index = i;
        for( int j=0; j<4; j++){
            noise += ofSignedNoise(index, 1) * amp;
            amp *= 0.5;
            index *= 2.0;
        }

        noise *= 20.0;
        
        ofVec3f pos( length*noise, 0, 0);
        pos.rotate( angle, ofVec3f(ofRandom(1.0), ofRandom(1.0),ofRandom(1.0)) );
        pos.y = 0;
        pos += center;
        
        float mass = ofRandom( 0.1, 3.0 );
        s->create( world.world, pos, mass );
        s->setActivationState( DISABLE_DEACTIVATION );
        s->add();
        shapes.push_back( s );
        
        points.addVertex( pos );
		initial_pos.push_back( pos );
		viz1.addVertex( ofVec3f(0,0,0) );
		
        ofFloatColor c( ofRandom(0,0.1), ofRandom(0.6,0.8) );
        points.addColor( c );
		viz1.addColor( c );
    }
}

void ofApp::update(){
	
    float frame = ofGetFrameNum();
    
    ofVec3f attr = ofVec3f( 30, 0, 0 );
    attr.rotate( -0.3*frame, ofVec3f(0.1, 0.5, 0.3) );
    attrs.setVertex( 0, attr );
    
    ofVec3f stlt1( 15, 21, -7) ;
    stlt1.rotate( -0.7*frame, ofVec3f(-0.1, 0.8, -0.1) );
    attrs.setVertex( 1, attr+stlt1 );

    ofVec3f stlt2( -31, -31, 2);
    stlt2.rotate( 1.0*ofGetFrameNum(), ofVec3f(-0.1, 0.8, -0.1) );
    attrs.setVertex( 2, attr+stlt2 );

    ofVec3f diff;
    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f pos = shapes[i]->getPosition();
        ofVec3f force( 0,0,0 );

        {
            // attr1
            diff = ( attr + stlt1 ) - pos;
            float dist2 = diff.lengthSquared();
            float mass = (float)(i % 100) * 0.1;
            float power = (mass/dist2) * 5000.0;
            
            if( bAttract ){
                force += diff.normalized() * power;
            }
        }
        
        {
            // attr2
            diff = ( attr + stlt2 ) - pos;
            float dist2 = diff.lengthSquared();
            float mass = (float)(i % 100) * 0.1;
            float power = (mass/dist2) * 5000.0;
            
            if( bAttract ){
                force += diff.normalized() * power;
            }
        }

        
        shapes[i]->applyCentralForce( force );

        if( bViz1_draw ){
			ofVec3f repr = ( pos - initial_pos[i] );
			repr.x = 150.0 * log10( repr.x ); //0;
			repr.y = 150.0 * log10( repr.y );
//			repr.z = 150.0 * log10( repr.z );
          repr.x = fmodf( repr.x, ofGetWidth()/2 );
          repr.y = fmodf( repr.y, ofGetHeight()/2 );
            
            //repr *= 3.0;
            repr.x += ofGetWidth()/2;
            repr.z = 0;
            viz1.setVertex( i, repr );
        }else{
            
            //
            //      flat lan
            //
//            pos.z = 0;

            points.setVertex( i, pos );
            
            if( ofRandom(1.0) > 0.5 ){
                
                if( trail.getNumVertices() > trail_max ){
                    trail.setVertex( trail_head, pos );
                    trail.setColor( trail_head, ofFloatColor(0,1) );
                    trail_head++;
                    if( trail_head> trail_max )
                        trail_head = 0;
                }else{
                    trail.addVertex( pos );
                    trail.addColor( ofFloatColor(0,1) );
                }
            }
        }
    }

	if( 0 ){
		lines.clear();
		for( int i=0; i<50; i++ ){
            
            int index1 = i*30;
            ofVec3f pos1 = shapes[index1]->getPosition();
            
            for( int j=0; j<500; j++) {
                int index2 = ofRandom( 0, trail.getNumVertices()-1 );
			
                ofVec3f pos2 = trail.getVertex( index2 );
                float dist = pos1.distance( pos2 );

                float min = 5.0;
                float max = 30.0;
        
                if( min<dist && dist<max ){
                    lines.addVertex( pos1 );
                    lines.addVertex( pos2 );
                    
                    lines.addColor( ofFloatColor(0, 0.3) );
                    lines.addColor( ofFloatColor(0, 0.3) );
                }
            }
		}
	}
	
    
    // trail animation
    if( 1 ){
  
        for( int i=0; i<trail.getNumVertices(); i++ ){
            
            ofVec3f p = trail.getVertex( i );
            ofFloatColor c = trail.getColor( i );
            if (c.a <0 ) {
                c.a = 0;
            }else{
                c.a -= 0.00;
                trail.setColor( i, c );
            }
        }
    }

	world.update();
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofEnableDepthTest();
    ofBackground( 255 );
    
    if( !bViz1_draw ){
        cam.begin();
//        ofRotate( ofGetFrameNum()*0.05, 1, 0, 0 );
        ofSetColor( 255 );
        glLineWidth( 1 );
        lines.draw();

        glPointSize( 1 );
        points.draw();

        glPointSize( 1 );
        trail.draw();

        if( bDebug_draw ){
            world.drawDebug();
            glPointSize( 5 );
            attrs.draw();
        }

        cam.end();
    }else{
        ofPushMatrix();
        ofTranslate( 10, ofGetHeight()/2 );
        viz1.draw();
        ofPopMatrix();
    }
    
    draw_info();
}

void ofApp::draw_info(){
    if ( !bDraw_info )
        return;

    ofSetColor( 255 );
	stringstream ss;
	ss << "fps: " << ofToString( ofGetFrameRate(),0 ) << endl;
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
            bAttract = !bAttract;
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'I':
            bDraw_info = !bDraw_info;
            break;
        default:
			break;
			
		case 'd':
			bDebug_draw = !bDebug_draw;
			break;

        case 'v':
            bViz1_draw = !bViz1_draw;
            break;

    }
}
