//
//  ad_grav_wall.cpp
//  grav_wall_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#include "ad_grav_wall.h"
#include "ofApp.h"
#include "ad_geom_util.h"
#include "ofxTransitions.h"

int ad_grav_wall::particle_col_group = 1;
int ad_grav_wall::wall_col_group = 2;
int ad_grav_wall::particle_col_setting = wall_col_group;
int ad_grav_wall::wall_col_setting = particle_col_group;

int ad_grav_wall::pid = 0;

ad_grav_wall::ad_grav_wall()
:bInitPhysics(false),
bReleased(false),
impulse(0.4)
{}

void ad_grav_wall::setup( ofImage * img,  ofCamera *cam ){
    if( !bInitPhysics ){
        world.setup();
        world.setCamera( cam );
        world.setGravity( ofVec3f(0, 0, 0) );
//        world.enableCollisionEvents();
//        ofAddListener(world.COLLISION_EVENT, this, &ad_grav_wall::onCollision);
        
        for( int i=1; i<=30; i++ ){
            float size = 13 + i;
            sphereShapes.push_back( ofBtGetSphereCollisionShape( size ) );
            cout << ofToString(size,2) << " ";
        }
        cout << endl;
        
        bInitPhysics = true;
        
        pid = -1;
    }
    
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );

    prep_lines.setUsage( GL_DYNAMIC_DRAW );
    prep_lines.setMode( OF_PRIMITIVE_LINES );
    
    colref = img;

    wall_thickness = 80;
}

void ad_grav_wall::create_line( ofVec2f p1, ofVec2f p2, float density ){
    
    gvWall * gvw = new gvWall(p1, p2);

    /*
     *     add Attractors
     */
    ofVec2f dir = p2 - p1;
    gvw->norm = dir.rotated(90, ofVec3f(0,0,1)).normalize();
    float length = dir.length();
    gvws.push_back( gvw );

    
    /*
     *  add Particles
     */
    int num = length * density;
    int nshapes = shapes.size();
    int side_num = num/2;
    ofVec2f adder = dir/side_num;

    /*
     *      j=0 -> 0<dist
     *      j=1 -> dist<0
     */
    for (int j=0; j<2; j++) {

        int updown = j==0 ? 1:-1;
        int logr = ofRandom(1,12);
        float start_rate = ofRandom(0.15, 0.2);
        int start_id = side_num * start_rate;

        float expand_rate = ofRandom(1.1, 1.75);
        for(int i=0; i<side_num; i++){
        
            pid++;

            float logf = log10(1+i)/log10(1+side_num-1);
			logf *= 0.3;
			logf += (float)i/side_num * 0.7;
			
            int add = side_num * logf * (0.8-start_rate);
            add += start_id;
            if( add >= side_num )
                add = side_num - (add%side_num * expand_rate);
            
            ofVec3f pivot = p1 + adder*add;
            
            float dist;
            if( ofNoise(i, j*0.007) > 0.55 ){
                // stream
                dist = ofNoise( j*13+i*0.01 )*100 + ofNoise(j*12 + i*0.3)*30.0;
                dist *= 4.0;
            }else{
                dist = ofNoise( j, i*0.05 )*100 + ofNoise(j, i*0.5)*30.0;
				dist *= 3.0;
			}
			
			
            if( ofRandomuf()>0.9)
                dist *= 1.75;
            if( ofRandomuf()>0.91)
                dist *= 1.6;
            if( ofRandomuf()>0.92)
                dist *= 1.6;
            if( ofRandomuf()>0.93)
                dist *= 1.5;
            if( ofRandomuf()>0.94)
                dist *= 1.5;

    
            if( ofRandomuf()>0.6){
                pivot += adder*ofRandom(40, 170);
                dist *= 0.75;
            }

            dist = MAX( wall_thickness*1.5, dist);
            dist *= updown;

            ofVec3f pos = pivot + gvw->norm*dist;
            
            pivots.push_back( pivot );
            
            ofxBulletSphere * s= new ofxBulletSphere();
            
            float mass = 3.0 - ofNoise(pid*0.1, pos.y);
            float size = ofNoise(i+300,mass)*30.0 + i*0.001;
            size = ofMap(size, 0, 30.0, 0, 29, true);
            
            float friction = 0.5 + ofApp::app->getNoise(i+400,2)*0.1;
            float restriction = 0.5 + ofNoise(pid*0.08);
            if( ofRandomf() > 0.9 ) restriction *= 1.5;
            
            s->init( sphereShapes[(int)size] );
            s->create( world.world, pos, mass );
            s->setFriction( friction );
            s->setRestitution( restriction );
            s->setActivationState( DISABLE_DEACTIVATION );
            
            int mask = 0;
            if( ofNoise(pid*0.01) > 0.15 ){
                mask = wall_col_group;
            }else{
                mask = particle_col_group;
            }
            
            if( ofNoise(updown, pid*0.008) > 0.95){
                mask = 0;
            }

            s->add( particle_col_group, mask );
            
            s->getRigidBody()->setUserPointer( gvw );
            shapes.push_back( s );
            points.addVertex( pos );
            
            if( colref ){
                int x = pid % (int)colref->getWidth();
                int y = pid / colref->getWidth();
                ofFloatColor c = colref->getColor( x, y );
                c *= 0.98;
                c.a = 0.99;
                
                if( ofRandomuf()>0.88 ){
                    c.r += 0.5;
                }else{
                    float angle = c.getHueAngle();
                    c.setHueAngle( angle + ofSignedNoise(i*0.03)*60);
                }
                
                points.addColor( c );
            }else{
                points.addColor( ofFloatColor(0) );
            }
        }
    }
}

void ad_grav_wall::create_wall(ofVec2f p1, ofVec2f p2){
    // wall shape, x direction default
    ofxBulletBox * w = new ofxBulletBox();
    ofVec3f center = (p2+p1) / 2.0;
    
    ofVec3f xa( 1, 0, 0);
    ofVec3f dir = p1 - p2;
    if( dir.y < 0){
        dir = -dir;
    }
    float angle = dir.angle( xa );
    ofQuaternion quat( angle, ofVec3f(0,0,1) );
    float mass = 0;
    float length = dir.length();
    ofVec3f size( length+3, wall_thickness, 1000 );
    w->create( world.world, center, quat, mass, size.x, size.y, size.z );
    
    float restriction = 1;
    float friction = 0.5;
    w->setProperties( restriction, friction );
    w->add( wall_col_group, wall_col_setting );
    w->getRigidBody()->setUserPointer( NULL );
    
    walls.push_back( w );
}

void ad_grav_wall::update(){
    frame = ofGetFrameNum();
    update_attrs();
    update_points();
    update_lines();
    world.update();
}

void ad_grav_wall::update_attrs(){
    
    // gravity
    if( 1 ){
        btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
        for( int i=0; i<objs.size(); i++ ){
            
            btTransform &trans = objs[i]->getWorldTransform();
            btVector3 &bpos = trans.getOrigin();
            ofVec3f pos(bpos.x(), bpos.y(), bpos.z());
            gvWall * gl = static_cast<gvWall*>( objs[i]->getUserPointer() );
            if( gl == NULL )
                continue;

            ofVec3f grav_dir = ad_geom_util::vec_pl(pos, gl->p1, gl->p2 );
            grav_dir.normalize();
			
            btVector3 grav(grav_dir.x, grav_dir.y, grav_dir.z );
            
            btRigidBody* body = btRigidBody::upcast(objs[i]);
            body->applyCentralForce( grav * impulse * 400 );
        }
    }
}

void ad_grav_wall::update_lines(){

    lines.clear();
    prep_lines.clear();
    polys.clear();
    
    const vector<ofFloatColor> & pcol = points.getColors();
    
	btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
	
#pragma mark GLOBAL_PIVOT_LINE
    if(0){
        //float limit = 730;      // ABC
        // float limit = 1050;    // DEF
         float limit = 6050;  //FGH
        
		int n = 1 + ofNoise(ofGetFrameNum()*1.2) * 800;
		for( int i=0; i<n; i++ ){
		
			int id1 = ofNoise(i, ofGetFrameNum()*2.0) * (objs.size()-1);
			if( ofRandomuf()<0.02 ) id1 = ofRandom(0, 20);
			
			ofPoint &gpv = ofApp::app->global_pivot;
			
			btTransform &trans1 = objs[id1]->getWorldTransform();
			btVector3 &bpos1 = trans1.getOrigin();
			ofVec3f pos1( bpos1.x(), bpos1.y(), bpos1.z() );
			
			float d = pos1.distance( gpv );
			if( d<30 || limit<d  ) continue;
			
			ofFloatColor c = ofFloatColor(0.8, 0.8) - pcol[id1];
			c.a = 0.3;
			
			lines.addVertex( pos1 );
			lines.addVertex( gpv );
			lines.addColor( c );
			lines.addColor( c );
			
			for( int k=0; k<3; k++ ){
				float rate = 1+k*0.5;
				
				ofVec3f d1 = pos1;
				ofVec3f d2 = gpv;
				d1.x += ofRandomf() * rate;
				d1.y += ofRandomf() * rate;
				
				lines.addVertex( d1 );
				lines.addVertex( d2 );
				lines.addColor( c );
				lines.addColor( c );
			}
		}
	}
#pragma mark RANDOM_LINE
	for( int i=0; i<1000; i++ ){

		int id1 = ofRandom(0, objs.size()-1);
		int id2 = ofRandom(0, objs.size()-1);
		
        if( objs[id1]->isKinematicObject() || objs[id2]->isKinematicObject() )
            continue;
        
		btTransform &trans1 = objs[id1]->getWorldTransform();
		btVector3 &bpos1 = trans1.getOrigin();
		ofVec3f pos1( bpos1.x(), bpos1.y(), bpos1.z() );
		
		btTransform &trans2 = objs[id2]->getWorldTransform();
		btVector3 &bpos2 = trans2.getOrigin();
		ofVec3f pos2( bpos2.x(), bpos2.y(), bpos2.z() );
		
		float d = pos1.distance(pos2);
		float limit = 200;
		if( d<30 || limit<d  ) continue;
		
		ofFloatColor c = ofFloatColor(0.8, 0.8) - pcol[id1];
		c.b = 0.8;
		c.a = 0.1;
		
		lines.addVertex( pos1 );
		lines.addVertex( pos2 );
		lines.addColor( c );
		lines.addColor( c );
		
		for( int k=0; k<3; k++ ){
			float rate = 1.0 + k;
			
			ofVec3f d1 = pos1;
			ofVec3f d2 = pos2;
			d1.x += ofRandomf() * rate;
			d1.y += ofRandomf() * rate;
			d2.x += ofRandomf() * rate;
			d2.y += ofRandomf() * rate;
			
			lines.addVertex( d1 );
			lines.addVertex( d2 );
			lines.addColor( c );
			lines.addColor( c );
		}
	}

#pragma mark NEAR_LINE
	for( int i=0; i<objs.size(); i++ ){
        
        float nR = ofApp::app->getNoise(i,0);
        float nG = ofApp::app->getNoise(i,1);
        float nB = ofApp::app->getNoise(i,2);
        
		btTransform &trans = objs[i]->getWorldTransform();
		btVector3 &bpos = trans.getOrigin();
        ofVec3f pos1( bpos.x(), bpos.y(), bpos.z() );
        pos1.z = 0;
		
		{
            int num_line = ofNoise(i*0.1, ofGetFrameNum()*0.5)*5 + ofGetFrameNum()*0.003;
            num_line = MIN(num_line, 10);
			bool bStatic1 = objs[i]->isStaticObject();
			if( bStatic1 ) continue;
			
			if( num_line <= 0 ) continue;
			
			multimap<float, ofVec3f> near_p;
			pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
			for( int line=0; line<num_line; line++ ){
				near_p.insert( pair1 );
			}
			
			for( int j=0; j<objs.size(); j++ ){
				if( i==j ) continue;

				bool bStatic2 = objs[j]->isStaticObject();
				if( bStatic2 ) continue;
				
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
			}
			multimap<float, ofVec3f>::iterator itr = near_p.begin();
			
			for(; itr!=near_p.end(); itr++ ){
				ofVec3f &pos2 = itr->second;
				if(pos2.x == -12345) continue;
				
				float d = pos1.distance(pos2);
				float limit = 600;
				
				if( d<10 || limit<d  ) continue;

				ofFloatColor c = ofFloatColor(0.8, 0.8) - pcol[i];
				c.a = 0.2;

				lines.addVertex( pos1 );
				lines.addVertex( pos2 );
				lines.addColor( c );
				lines.addColor( c );

				c.r += nR*0.1 + ofRandomf()*0.01;
				c.g += nG*0.1 + ofRandomf()*0.01;
				c.b += nB*0.1  + ofRandomf()*0.01;

				c.a = 0.05;

				int n = ofNoise( i, ofGetFrameNum() ) * 10.0;
				for( int k=0; k<n; k++ ){
					float rate = 1.0 + k;

					ofVec3f d1 = pos1;
					ofVec3f d2 = pos2;
					d1.x += ofRandomf() * rate;
					d1.y += ofRandomf() * rate;
					d2.x += ofRandomf() * rate;
					d2.y += ofRandomf() * rate;

					lines.addVertex( d1 );
					lines.addVertex( d2 );
					lines.addColor( c );
					lines.addColor( c );
				}
			}
		}
    }

#pragma mark PREP_LINE
    {
        int limit = 110;
        for( int i=0; i<objs.size(); i++ ){
            btTransform &trans = objs[i]->getWorldTransform();
            btVector3 &bpos = trans.getOrigin();
            ofVec3f pos1( bpos.x(), bpos.y(), bpos.z() );
            pos1.z = 0;
            
            gvWall * gl = static_cast<gvWall*>(objs[i]->getUserPointer());
            if( gl != NULL){
                ofVec3f toline = ad_geom_util::vec_pl(pos1, gl->p1, gl->p2);
                ofVec3f onLine = pos1 + toline;
                float len = toline.length();
                bool isOnline = ad_geom_util::isOnline(onLine, gl->p1, gl->p2);
                
                if( ofApp::app->getNoise(i,2) + ofNoise(i*0.002) > 1.45 ){
                    
                    if( len < limit && isOnline ){
                        
                        prep_lines.addVertex(pos1);
                        prep_lines.addVertex(onLine);
                        
                        ofFloatColor c = pcol[i];
                        c.b *= 1;
                        c.a = 0.1;
                        prep_lines.addColor( c );
                        prep_lines.addColor( c );
                        
                        int n = 1 + ofNoise( i, ofGetFrameNum() ) * 20.0;
                        for( int k=0; k<n; k++ ){
                            float rate = 1.0 + k*0.1;
                            
                            ofVec3f d1 = pos1;
                            ofVec3f d2 = onLine;
                            d1.x += ofRandomf() * rate;
                            d1.y += ofRandomf() * rate;w
                            d2.x += ofRandomf() * rate;
                            d2.y += ofRandomf() * rate;
                            
                            lines.addVertex( d1 );
                            lines.addVertex( d2 );
                            lines.addColor( c );
                            lines.addColor( c );
                        }
                    }
                }
            }
        }
    }

#pragma mark POLY_LINE
    if(0){
    for( int i=0; i<20; i++ ){
        
        for( int j=0; j<3; j++ ){
            
            int id = ofApp::app->getNoise(i + j*511) * (objs.size()-1);
            if(ofRandomuf()>0.65)
                id = ofRandom(0, objs.size()-1);
            
            btTransform &trans = objs[id]->getWorldTransform();
            btVector3 &bpos = trans.getOrigin();
            ofVec3f pos1( bpos.x(), bpos.y(), 0);
            gvWall * gl = static_cast<gvWall*>(objs[id]->getUserPointer());
            if( gl == NULL) continue;
            ofVec3f toline = ad_geom_util::vec_pl(pos1, gl->p1, gl->p2);
            ofVec3f onLine = pos1 + toline;
            float len = toline.length();
            
            int minl = ofNoise(id*0.01, ofGetFrameNum()*0.12) * 200 + 100;
            int maxl = ofNoise(id*0.012, ofGetFrameNum()*0.113)*200 + minl;
            
            if ( minl<len && len<maxl ) {
                ofVec3f dir = gl->p2-gl->p1;
                ofVec3f dirn = dir.getNormalized();
                ofVec3f toLinen = toline.getNormalized();
                
                float lenToEnd = (gl->p2 - onLine).length();
                
                dirn *= ofNoise( i*0.03 )<0.0 ? -1 : 1;
                
                int n = ofNoise( ofGetFrameNum()*0.1 ) *3 +1;
                for (int k=0; k<n; k++) {
                    float width = lenToEnd; //300 + ofRandomf()*10;
                    int step = width/2;
                    
                    ofVec3f current = pos1 + ofVec3f(ofRandomf()*5, 5, 0);
                    ofVec3f toline = ad_geom_util::vec_pl(current, gl->p1, gl->p2);
                    float height = toline.length();
                    
                    ofPolyline poly;
                    poly.addVertex( pos1 );
                    
                    current -= dirn*10;
                    poly.addVertex( current );
                    
                    ofxTransitions tr;
                    for( int l=0; l<step; l++ ){
                        float plotx = log10(l)/log10(step);
                        float x = width * plotx;
                        float y = tr.easeInOutCubic(l, 0, height, step);
                        ofVec3f v =  dirn*x + toLinen*y;
                        poly.addVertex( current + v);
                    }
                    polys.push_back( poly );
                }
            }
        }
    }
    }
}

void ad_grav_wall::update_points(){

    vector<ofVec3f> & verts = points.getVertices();
    vector<ofFloatColor> &cols = points.getColors();
	
    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f p = shapes[i]->getPosition();
        p.z = 0;
        verts[i] = p;
    }
}

void ad_grav_wall::draw(){

    int exFrame = ofApp::app->exporter.getFrameNum();
    
    ofNoFill();

	if( exFrame <= 1 ){
		ofSetLineWidth( 1 );
		for( int i=0; i<gvws.size(); i++ ){
			ofSetColor( 0 );
			ofLine( gvws[i]->p1, gvws[i]->p2 );
		}
	}

    glLineWidth( 1 );
	
    if( 1 ){
        prep_lines.draw();
        
        for( int i=0; i<polys.size(); i++ ){
            ofSetColor(30,0,ofRandom(20,40), 100);
            polys[i].draw();
        }

        lines.draw();
    }
    glPointSize( 2 );
    points.draw();

}

void ad_grav_wall::onCollision( ofxBulletCollisionData &cdata ){
    //if(frame > 30)
    //    collision.insert( collision.end(), cdata.worldContactPoints1.begin(), cdata.worldContactPoints1.end() );
}

void ad_grav_wall::setGrav( float g){	
    impulse = g;
}

void ad_grav_wall::toggleCollision(){

    btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
    for( int i=0; i<objs.size(); i++ ){
        
        if( ofNoise(i) > 0.2 ){
            short mask = particle_col_group | wall_col_group;
            objs[i]->activate();
            objs[i]->getBroadphaseHandle()->m_collisionFilterMask = mask;
        }
    }
}
