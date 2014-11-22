//
//  ad_grav_line.cpp
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#include "ad_grav_line.h"
#include "ofApp2.h"

ad_grav_line::ad_grav_line()
:bInitPhysics(false)
{}

void ad_grav_line::setup( ofCamera *cam ){
    if( !bInitPhysics ){
        world.setup();
        world.setCamera( cam );
        world.setGravity( ofVec3f(0, 0, 0) );
        world.enableCollisionEvents();
        ofAddListener(world.COLLISION_EVENT, this, &ad_grav_line::onCollision);
        
        for( int i=1; i<=10; i++ ){
            sphereShapes.push_back( ofBtGetSphereCollisionShape( 1.0 + i*i*i*i*0.0008) );
        }
        
        bInitPhysics = true;
    }
    
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
    
    add_random_particle( 100 );
}

void ad_grav_line::add_random_particle( int num ){
    for(int i=0; i<num; i++){
        ofVec3f pos( ofRandomf(), ofRandomuf(), 0 );
        pos.x *= 800;
        pos.y *= 700;
        pos.y += 500;
        ofxBulletSphere * s= new ofxBulletSphere();
        
		float size = ofApp2::app->getNoise(i) * 10.0;
        size = ofMap(size, 0, 10.0, 1, 9);
		float mass = 5.0 - ofApp2::app->getNoise(i) * 3.0;
        if( ofRandomuf() < 0.1) mass = 0;
		float friction = ofApp2::app->getNoise(i+100) * 10.0;
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

void ad_grav_line::create_line( ofVec2f p1, ofVec2f p2, float density ){
    
    gvline gvl(p1, p2);

    /*
     *     add Attractors
     */
    ofVec2f dir = p2 - p1;
    gvl.norm = dir.rotated(90, ofVec3f(0,0,1)).normalize();
    int step = 10;
    float dist = 300;
    {
        ofVec2f adder( dir.x/step, dir.y/step);
        for( int i=0; i<step+30; i++ ){
            ofVec3f attr = gvl.norm*dist;
            attr.rotate( ofRandom(-360, 360), dir);
            attr += adder*i;
            attr += p1;
            gvl.attrs.addVertex( attr );
            gvl.attrs.addColor( ofFloatColor(1,0,0) );
        }
        gvls.push_back( gvl );
    }
    
    /*
     *  add Particles
     */
    float offset = 10;
    float length = dir.length() - offset;
    float rate = (length-offset) / length;
    int num = length * density;
    ofVec2f adder = dir / num * rate;
    for(int i=0; i<num; i++){
        for(int d=0; d<2; d++){
            ofVec3f pos = p1 + adder*i;
            pos += adder * offset;
            pos += adder*ofApp2::app->getSignedNoise(i*10);
            ofxBulletSphere * s= new ofxBulletSphere();
            
			float mass = 5.0 - ofApp2::app->getNoise(i,1) * 3.0;
            float size = ofApp2::app->getNoise(i+300,2) * 10.0 + i*0.001;
            size = ofMap(size, 0, 10.0, 1, 9, true);

            if( ofRandomuf() < 0.011)
                mass = 0;

            float friction = ofApp2::app->getNoise(i+400,2) * 10.0;
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

void ad_grav_line::update(){
    frame = ofGetFrameNum();
    update_attrs();
    update_lines();
    update_points();
    world.update();
}

void ad_grav_line::update_attrs(){
    // move attr
    for( int j=0; j<gvls.size(); j++ ){
        for( int i=0; i<gvls[j].attrs.getNumVertices(); i++ ){
            ofVec3f ap = gvls[j].attrs.getVertex(i);
            ofVec3f ax = gvls[j].p2 - gvls[j].p1;
            ap -= gvls[j].p1;
            ap.rotate( 1.0, ax );
            ap += gvls[j].p1;
            gvls[j].attrs.setVertex(i, ap);
        }
    }
    
    // gravity
	btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
	for( int i=0; i<objs.size(); i++ ){
		
		btTransform &trans = objs[i]->getWorldTransform();
		btVector3 &bpos = trans.getOrigin();
		ofVec3f pos(bpos.x(), bpos.y(), bpos.z());
        ofVec3f force( 0,0,0 );
        
        for( int g=0; g<gvls.size(); g++ ){
            
//            if( ofApp2::app->getNoise(i,2) >0.7) continue;
            if( g != i%gvls.size()) continue;
                
            ofVboMesh & attrs = gvls[g].attrs;
            for( int j=0; j<attrs.getNumVertices(); j++ ){
                
                ofVec3f diff = attrs.getVertex(j) - pos;
                float dist2 = diff.lengthSquared();
                if(dist2>10){
                    float power = (1.0/dist2) * 1000000.0;
                    force += diff.normalized() * (power * ofApp2::app->getNoise(i*2) );
                }
            }
            
            // vertical force
//            {
//                ofVec3f d = (ofVec3f(pos.x, 0, 0) - pos);
//                float dist2 = d.lengthSquared();
//                force += d.normalized() * dist2*0.05;
//            }
            
            //force *= gvls[g].bNegativeGrav ? -1 : 1;
        }
        shapes[i]->applyCentralForce( force );

    }
}

void ad_grav_line::update_lines(){

    lines.clear();
	btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
	for( int i=0; i<objs.size(); i++ ){
		btTransform &trans = objs[i]->getWorldTransform();
		btVector3 &bpos = trans.getOrigin();
		ofVec3f pos1( bpos.x(), bpos.y(), bpos.z() );
		
        float noise = ofApp2::app->getNoise(i);
        int num_line = 3; // + noise*noise*8.0;
        bool bStatic1 = objs[i]->isStaticObject();
        if( bStatic1 ) num_line = 1;
        
        multimap<float, ofVec3f> near_p;
        pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
        for( int line=0; line<num_line; line++ ){
            near_p.insert( pair1 );
        }
        

        for( int j=i+1; j<objs.size(); j++ ){

			bool bStatic2 = objs[j]->isStaticObject();
            if( bStatic1 && bStatic2 ) continue;
            
			btTransform &trans = objs[j]->getWorldTransform();
			btVector3 &bpos = trans.getOrigin();
			ofVec3f pos2( bpos.x(), bpos.y(), bpos.z() );

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
            float limit = 200;
            
            if( bStatic1 ) limit = 500;
            
            if( d<1 || limit<d  ) continue;
            lines.addVertex( pos1 );
            lines.addVertex( pos2 );

			ofFloatColor c;
			 c.setHsb(  ofApp2::app->getNoise(i,0)*0.3	+ 0.6,
						ofApp2::app->getNoise(i,1)*0.3	+ 0.1,
						ofApp2::app->getNoise(i,2)*0.3	+ 0.3
                     );
			
			c.a = ofApp2::app->getNoise(i+1000)*0.8 + 0.2;
            lines.addColor( c );
            lines.addColor( c );
        }
    }
}

void ad_grav_line::update_points(){
    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f sp = shapes[i]->getPosition();
        ofVec3f pp(sp);
        sp *= 0.01;
		pp.y += ofApp2::app->getSignedNoise(i+100, 2);
        pp.y += sp.y;
        points.setVertex( i, pp );
        ofFloatColor c;
        c.setHsb( ofApp2::app->getNoise(i+3000,2)*0.4+0.55, ofRandom(0.1,0.8), ofApp2::app->getNoise(i*4,2)*0.4+0.4);
        c.a = ofApp2::app->getNoise(i+3300)*0.8 + 0.2;
        points.setColor( i, c);
    }
}

void ad_grav_line::draw(){
    ofNoFill();
    ofSetLineWidth( 1 );
	
	for( int i=0; i<gvls.size(); i++ ){
		ofSetLineWidth( 1 );
		ofSetColor( 0 );
		ofLine( gvls[i].p1, gvls[i].p2 );
	}
	
	for( int i=0; i<collision.size(); i++ ){
		if( i%3 == 0){
			ofSetColor( 30, 150);
			ofVec3f &p = collision[i];
			ofCircle( p, 1 );
		}
    }
    collision.clear();

    glPointSize( 1 );
    points.draw();

    glLineWidth( 1 );
    lines.draw();
}

void ad_grav_line::onCollision( ofxBulletCollisionData &cdata ){
    if(frame > 30)
        collision.insert( collision.end(), cdata.worldContactPoints1.begin(), cdata.worldContactPoints1.end() );
}
