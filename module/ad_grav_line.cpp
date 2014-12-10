//
//  ad_grav_line.cpp
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#include "ad_grav_line.h"
#include "ofApp.h"
#include "ad_geom_util.h"

ad_grav_line::ad_grav_line()
:bInitPhysics(false)
{}

void ad_grav_line::setup( ofCamera *cam ){
    if( !bInitPhysics ){
        world.setup();
        world.setCamera( cam );
        world.setGravity( ofVec3f(0, -10, 0) );
        world.enableCollisionEvents();
        //ofAddListener(world.COLLISION_EVENT, this, &ad_grav_line::onCollision);
        
        for( int i=1; i<=30; i++ ){
            float size = 0.1 + i*i*i*0.00015;
            sphereShapes.push_back( ofBtGetSphereCollisionShape( size ) );
            cout << "sphereShape[" << i << "], size = " << size << endl;
        }
        
        bInitPhysics = true;
    }
    
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

    prep_lines.setUsage( GL_DYNAMIC_DRAW );
    prep_lines.setMode( OF_PRIMITIVE_LINES );
}

void ad_grav_line::add_random_particle( int num ){
    for(int i=0; i<num; i++){
        ofVec3f pos( ofRandomf(), ofRandomuf(), 0 );
        pos.x *= 3000;
        pos.y *= 1300;
        pos.y += 0;
        ofxBulletSphere * s= new ofxBulletSphere();
        
		float size = ofApp::app->getNoise(i) * 32.0;
        size = ofMap(size, 0, 32.0, 0, 29);
		float mass = 5.0 - ofApp::app->getNoise(i) * 2.0;
		float friction = ofApp::app->getNoise(i+100) * 2.0;
        s->init( sphereShapes[(int)size] );
        s->create( world.world, pos, mass );
        s->setFriction( friction );
        s->setActivationState( DISABLE_DEACTIVATION );
        s->getRigidBody()->setUserPointer( NULL );
        s->add();
        shapes.push_back( s );
        points.addVertex( pos );
        points.addColor( ofFloatColor(0) );
    }
}

void ad_grav_line::create_line( ofVec2f p1, ofVec2f p2, float density ){
    
    gvline * gvl = new gvline(p1, p2);

    /*
     *     add Attractors
     */
    ofVec2f dir = p2 - p1;
    gvl->norm = dir.rotated(90, ofVec3f(0,0,1)).normalize();
    int step = 70;
    float dist = 300 + ofRandomuf()*100;
    {
        ofVec2f adder( dir.x/step, dir.y/step);
        for( int i=0; i<step; i++ ){
            ofVec3f attr = gvl->norm * dist;
            attr.rotate( ofNoise(dist*0.1, i*0.1) * 360.0, dir);
            attr += adder*i;
            attr += p1;
            gvl->attrs.addVertex( attr );
            gvl->attrs.addColor( ofFloatColor(1,0,0) );
        }
        gvls.push_back( gvl );
    }
    
    /*
     *  add Particles
     */
    float offset = 0;
    float length = dir.length() - offset;
    float rate = (length-offset) / length;
    int num = length * density;
    
    int nshapes = shapes.size();
    
    ofVec2f adder = dir / num * rate;
    for(int i=0; i<num; i++){
        for(int d=0; d<2; d++){
            ofVec3f pivot = p1 + adder*i;
            pivot += adder * offset;
            pivot += adder * (ofSignedNoise( nshapes + i*0.02 ) + ofSignedNoise( nshapes + i*0.3 ));
            ofVec3f pos = pivot + gvl->norm * (ofSignedNoise( i*0.01 ) + ofSignedNoise( i*0.3 ) + ofRandomf() ) * 3.0;
            
            pivots.push_back( pivot );
            
            ofxBulletSphere * s= new ofxBulletSphere();
            
			float mass = 5.0 - ofNoise(i, pos.y) * 2.0;
            float size = ofNoise(i+300*d,mass)*30.0 + i*0.001;
            size = ofMap(size, 0, 30.0, 0, 29, true);
//            cout << (int)size << " ";

            if( ofRandomuf() < 0.04)
                mass = 0;

            float friction = ofApp::app->getNoise(i+400,2) * 10.0;
            s->init( sphereShapes[(int)size] );
            s->create( world.world, pos, mass );
            s->setFriction( friction );
            s->setActivationState( DISABLE_DEACTIVATION );
            s->add( 1, 0);
            s->getRigidBody()->setUserPointer( gvl );
            shapes.push_back( s );
            points.addVertex( pos );
            points.addColor( ofFloatColor(0) );

            joints.push_back( NULL );
            
            int sn = shapes.size();
            if( sn > 31){
                ofxBulletJoint * joint1 = new ofxBulletJoint();
                joint1->create(world.world, s, shapes[sn-5]);
                joint1->add();
                joint1->setSTOP_CFM(0.1);
                joint1->setSTOP_ERP(0.1);

                ofxBulletJoint * joint2 = new ofxBulletJoint();
                joint2->create(world.world, s, shapes[sn-10]);
                joint2->add();
                joint2->setSTOP_CFM(0.1);
                joint2->setSTOP_ERP(0.1);
            }else if(sn != 0){
                ofxBulletJoint * joint1 = new ofxBulletJoint();
                joint1->create(world.world, s, shapes[0]);
                joint1->add();
                joint1->setSTOP_CFM(0.1);
                joint1->setSTOP_ERP(0.1);
            }
        }
    }
    
    cout << endl;
}

void ad_grav_line::update(){
    frame = ofGetFrameNum();
    update_attrs();
    update_points();
    update_lines();
    world.update();
}

void ad_grav_line::update_attrs(){
    // move attr
    for( int j=0; j<gvls.size(); j++ ){
        for( int i=0; i<gvls[j]->attrs.getNumVertices(); i++ ){
            ofVec3f ap = gvls[j]->attrs.getVertex(i);
            ofVec3f ax = gvls[j]->p2 - gvls[j]->p1;

            ap -= gvls[j]->p1;
            ap.rotate( 2, ax );
            ap += gvls[j]->p1;
            gvls[j]->attrs.setVertex(i, ap);
        }
    }
    
    // gravity
    if( 1 ){
        btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
        for( int i=0; i<objs.size(); i++ ){
            
            btTransform &trans = objs[i]->getWorldTransform();
            btVector3 &bpos = trans.getOrigin();
            ofVec3f pos(bpos.x(), bpos.y(), bpos.z());
            ofVec3f force( 0,0,0 );
            //for( int g=0; g<gvls.size(); g++ ){
                
                //if( g != i%gvls.size()) continue;
                //ofVboMesh & attrs = gvls[g].attrs;
            gvline * gl = static_cast<gvline*>( objs[i]->getUserPointer() );
            if( gl == NULL )
                continue;

            for( int j=0; j<gl->attrs.getNumVertices(); j++ ){
                ofVec3f diff = gl->attrs.getVertex(j) - pos;
                float dist2 = diff.lengthSquared();
                if(dist2>300){
                    float power = (1.0/dist2) * 1000000.0;
                    force += diff.normalized() * power * 10;
                }
            }
            shapes[i]->applyCentralForce( force );
        }
    }
    
    // pivots
    btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
    for( int i=0; i<objs.size(); i++ ){
        if( joints[i] != NULL )
            continue;
        
        btTransform &trans = objs[i]->getWorldTransform();
        btVector3 &bpos = trans.getOrigin();

        ofVec3f pos(bpos.x(), bpos.y(), bpos.z());
        ofVec3f & piv = pivots[i];
        
        float dist =pos.distance(piv);
        float limit = ofNoise(i*0.01) * 30.0 + 30;
        if( dist > limit ){
            joints[i] = new ofxBulletJoint();
            joints[i]->create(world.world, shapes[i], piv);
            joints[i]->add();
            joints[i]->setSTOP_CFM(0.01);
            joints[i]->setSTOP_ERP(0.01);
        }
    }
}

void ad_grav_line::update_lines(){

    lines.clear();
    prep_lines.clear();
    
	btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
	for( int i=0; i<objs.size(); i++ ){
        
        ofFloatColor c(1);
        c.setHsb(  ofApp::app->getNoise(i,0)*0.25	+ 0.6,
                 ofApp::app->getNoise(i,1)*0.3	+ 0.1,
                 ofApp::app->getNoise(i,2)*0.3	+ 0.1
                 );
        
        c.a = ofApp::app->getNoise(i+1000)*0.5 + 0.8;
        
		btTransform &trans = objs[i]->getWorldTransform();
		btVector3 &bpos = trans.getOrigin();
		ofVec3f pos1( bpos.x(), bpos.y(), bpos.z() );
        pos1.z = 0;
        float noise = ofApp::app->getNoise(i);
        int num_line = 5 + noise*noise*10.0;
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
            pos2.z = 0;

			float dist = pos1.distance( pos2 );
            
            multimap<float, ofVec3f>::iterator itr = near_p.end();
            
            itr--;
            if( dist < itr->first ){
                std::pair<float, ofVec3f> pair2( dist, pos2 );
                near_p.insert( pair2 );
                
                multimap<float, ofVec3f>::iterator end = near_p.end();
                near_p.erase( --end );
            }
            
            if( 1500< pos2.y && pos2.y<1700 ){
                if( ofRandomuf() > 0.9995){
                    ofVec3f dir = pos2-pos1;
                    float angle = dir.angle(ofVec3f(0,1,0));
                    if( angle < 30 ){
                        lines.addVertex( pos1 );
                        lines.addVertex( pos2 );
                        float b = c.b;
                        c.b *= 1.5;
                        lines.addColor( c );
                        lines.addColor( c );
                        c.b = b;
                    }
                }
            }
        }
        multimap<float, ofVec3f>::iterator itr = near_p.begin();
        
        for(; itr!=near_p.end(); itr++ ){
            ofVec3f &pos2 = itr->second;
            if(pos2.x == -12345) continue;
            
            float d = pos1.distance(pos2);
            float limit = 200;
            
            if( bStatic1 ) limit = 500;
            
            if( d<10 || limit<d  ) continue;

            lines.addVertex( pos1 );
            lines.addVertex( pos2 );
            lines.addColor( c );
            lines.addColor( c );
        }
        
        
        // pivot line
        if( ofRandomuf() > 0.01999 ){
            
            gvline * gl = static_cast<gvline*>(objs[i]->getUserPointer());
            if( gl != NULL){
                ofVec3f toline = ad_geom_util::vec_pl(pos1, gl->p1, gl->p2);
                ofVec3f onLine = pos1 + toline;
                
                ofRectangle area( gl->p1, gl->p2 );
                if( area.inside(onLine) ){
                    prep_lines.addVertex(pos1);
                    prep_lines.addVertex(onLine);

                    c.a *= 0.7;
                    prep_lines.addColor( c );
                    prep_lines.addColor( c );
                }
            }
        }
    }
    
}

void ad_grav_line::update_points(){

    vector<ofVec3f> & verts = points.getVertices();
    vector<ofFloatColor> & cols = points.getColors();

    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f p = shapes[i]->getPosition();
        p.z = 0;
        verts[i] = p;

        cols[i].r += ofApp::app->gpu_noise.getNoisef(i, 1) * 0.0002;
        cols[i].g += ofApp::app->gpu_noise.getNoisef(i, 2) * 0.0002;
        cols[i].b += ofApp::app->gpu_noise.getNoisef(i, 3) * 0.0002;
    }
}

void ad_grav_line::draw(){
    ofNoFill();
    ofSetLineWidth( 1 );
	for( int i=0; i<gvls.size(); i++ ){
		ofSetColor( 0 );
		ofLine( gvls[i]->p1, gvls[i]->p2 );
	}

    glPointSize( 3 );
    points.draw();

    glLineWidth( 1 );
    lines.draw();

    prep_lines.draw();
}

void ad_grav_line::draw_attr(){

    ofPushStyle();
    
    ofSetColor(5, 50);
    glPointSize(1);
    for( int g=0; g<gvls.size(); g++ ){
        gvls[g]->attrs.draw();
    }
    
    ofPopStyle();
}

void ad_grav_line::onCollision( ofxBulletCollisionData &cdata ){
    if(frame > 30)
        collision.insert( collision.end(), cdata.worldContactPoints1.begin(), cdata.worldContactPoints1.end() );
}
