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

bool ad_grav_wall::bDrawNearLine = true;
bool ad_grav_wall::bDrawAttrLine = false;
int ad_grav_wall::gravType = 0;

int ad_grav_wall::particle_col_group = 1;
int ad_grav_wall::wall_col_group = 2;
int ad_grav_wall::particle_col_setting = wall_col_group;
int ad_grav_wall::wall_col_setting = particle_col_group;
int ad_grav_wall::pid = 0;

ad_grav_wall::ad_grav_wall()
:bInitPhysics(false),
bReleased(false),
impulse(-0.0),
near_line_num(12)
{}

void ad_grav_wall::setup( ofImage * img,  ofCamera *cam ){
    if( !bInitPhysics ){
        world.setup();
        world.setCamera( cam );
        world.setGravity( ofVec3f(0, 0, 0) );
        
        for( int i=1; i<=30; i++ ){
            float size = 0.3 + i*0.02;
            btSphereShape * s = ofBtGetSphereCollisionShape( 1 );
            s->setLocalScaling( btVector3(size, size, size) );
            sphereShapes.push_back( s );
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
    
    colref = img;
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
    ofVec2f adder = dir/(float)num;

    /*
     *      j=0 -> 0<dist
     *      j=1 -> dist<0
     */
    float start_rate = 0;
    int start_id = num * start_rate;
    
    for(int i=0; i<num; i++){
        
        pid++;
        
        //float logf = log10(1+i)/log10(1+num);
        float logf = (float)i/num;
        
        int add = num * logf;
        add += start_id;
        if( add >= num )
            add = num - (add%num);
        
        ofVec3f pivot = p1 + adder*add;
        float dist = 2;
        
        ofVec3f pos = pivot + gvw->norm * dist;
        pivots.push_back( pivot );
        
        ofxBulletSphere * s = new ofxBulletSphere();
        
        float massNoise = ofNoise(pid*0.1, pos.y) + ofNoise(pid*0.0011, pos.y*0.0013 ) + ofNoise(pid*0.00027, pos.y*0.00021 ) + ofRandomuf() + ofRandomuf();
        float mass = 5.0 - massNoise;
        mass = MAX(0.2, mass);
        
        if( ofRandomuf() > 0.8 ){
            mass += ofRandom(0.5, 1);

            if( ofRandomuf() > 0.9 ){
                mass += ofRandom(1, 3);
                
                if( ofRandomuf() > 0.9 ){
                    mass += ofRandom(2, 4);
                }
            }
        }
        
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
        
        int mask = wall_col_group;
        s->add( particle_col_group, mask );
        s->getRigidBody()->setUserPointer( gvw );
        shapes.push_back( s );
        points.addVertex( pos );
        
        if( colref ){
            int x = pid % (int)colref->getWidth();
            int y = pid / colref->getWidth();
            ofFloatColor c = colref->getColor( x, y );
            float b = c.getBrightness();
            if( b > 0.5 ){
                c.setBrightness( b*ofRandom(0.2, 0.3) );
            }
            c.a = 0.5;

            points.addColor( c );
        }else{
            points.addColor( ofFloatColor(0) );
        }
    }
    
}

void ad_grav_wall::create_wall(ofVec2f p1, ofVec2f p2, float thickness ){
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
    ofVec3f size( length+3, thickness, 1000 );
    w->create( world.world, center, quat, mass, size.x, size.y, size.z );
    
    float restriction = 1;
    float friction = 0.5;
    w->setProperties( restriction, friction );
    w->add( wall_col_group, wall_col_setting );
    w->getRigidBody()->setUserPointer( NULL );
    
    walls.push_back( w );
}

void ad_grav_wall::create_waterline(ofVec2f p1, ofVec2f p2 ){
    gvWall * gvw = new gvWall(p1, p2);
    ofVec2f dir = p2 - p1;
    gvw->norm = dir.rotated(90, ofVec3f(0,0,1)).normalize();
    waterlines.push_back( gvw );
}

void ad_grav_wall::update(){
    frame = ofGetFrameNum();
    update_attrs();
    update_points();
    update_lines();
    world.update();
}

void ad_grav_wall::update_attrs(){
    
    btCollisionObjectArray& objs = world.world->getCollisionObjectArray();

    if( bDrawAttrLine ){
        vector<ofVec3f> vs;
        vs.assign( objs.size()*2, ofVec3f(0,0,0) );
        attrLines.clear();
        attrLines.addVertices(vs);
        vector<ofFloatColor> cs;
        cs.assign( objs.size()*2, ofFloatColor(0,0,0,0) );
        attrLines.addColors( cs );
    }
    
    // gravity
    if( 1 ){
        for( int i=0; i<objs.size(); i++ ){
            
            btTransform &trans = objs[i]->getWorldTransform();
            btVector3 &bpos = trans.getOrigin();
            ofVec3f pos(bpos.x(), bpos.y(), bpos.z());
            gvWall * gl = static_cast<gvWall*>( objs[i]->getUserPointer() );
            if( gl == NULL )
                continue;

            ofVec3f grav_dir = ad_geom_util::vec_pl(pos, gl->p1, gl->p2 );
            ofVec3f grav_dirn = grav_dir.normalized();
			
            btVector3 grav(grav_dirn.x, grav_dirn.y, grav_dirn.z );
            float len = grav_dir.length();
            
            btRigidBody* body = btRigidBody::upcast(objs[i]);
            float invMass = body->getInvMass();
            btVector3 force;
            
            switch( gravType ){
                    
                case 0:
                    force  = grav * invMass / (len*len) * 100.0;
                    break;
                case 1:
                    force = grav * invMass * (len*len*0.00001 + 0.001);
                    break;
            }
            
            body->applyCentralForce( force * impulse * 400.0 );
            if( bDrawAttrLine ){
                attrLines.setVertex( i*2, pos );
                attrLines.setVertex( i*2+1, pos+grav_dir );
                attrLines.setColor( i*2, ofFloatColor(0) );
                attrLines.setColor( i*2+1, ofFloatColor(0) );
            }
        }
    }
}

void ad_grav_wall::update_lines(){

    lines.clear();
    
    const vector<ofFloatColor> & pcol = points.getColors();
    
	btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
	
#pragma mark RANDOM_LINE
    if( 0 ){
        
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
    }

#pragma mark NEAR_LINE
    {
        if( bDrawNearLine ){
            
            int num_dupl = 4;
            int vertex_per_point = near_line_num * num_dupl * 2;
            
            const vector<ofVec3f> &vs = points.getVertices();
            vector<ofFloatColor> &vc = points.getColors();
            
            const ofVec3f * input = &vs[0];
            vector<ofVec3f> outv;
            outv.assign( vs.size()*vertex_per_point, ofVec3f(-99999, -99999, -99999) );
            vector<ofFloatColor> outc;
            outc.assign( vs.size()*vertex_per_point, ofFloatColor(0,0,0,0) );
            
            calcNearestPoints(input, &outv[0], &vc[0], &outc[0], vs.size(), near_line_num, num_dupl, ofApp::app->noise );
            lines.addVertices( outv );
            lines.addColors( outc );
        }
    }
}

void ad_grav_wall::update_points(){

    vector<ofVec3f> & verts = points.getVertices();
    vector<ofFloatColor> &cols = points.getColors();
	
    for( int i=0; i<shapes.size(); i++ ){
        ofVec3f p = shapes[i]->getPosition();
        //p.z = 0;
        verts[i] = p;
    }
}

void ad_grav_wall::draw_guide(){
    
    ofNoFill();

    if( ofApp::bRender == false ){
		ofSetLineWidth( 1 );
		for( int i=0; i<gvws.size(); i++ ){
			ofSetColor( 0,0,200,200 );
			ofLine( gvws[i]->p1, gvws[i]->p2 );
		}

        for( int i=0; i<waterlines.size(); i++ ){
            ofSetColor( 0, 255, 0 );
            ofLine( waterlines[i]->p1, waterlines[i]->p2 );
        }
    }
    
    if( bDrawAttrLine ){
        glLineWidth( 1 );
        attrLines.setMode( OF_PRIMITIVE_LINES );
        attrLines.draw();
    }
}

void ad_grav_wall::draw_lines(){
    glLineWidth( 1 );
    lines.draw();
}

void ad_grav_wall::draw_points(){
    glPointSize( 4 );
    points.draw();
}

void ad_grav_wall::setGrav( float g){
    impulse = g;
}

void ad_grav_wall::toggleCollision( float rate ){

    btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
    for( int i=0; i<objs.size(); i++ ){
        
        if( ofNoise(1, i) < rate ){
            short mask = particle_col_group | wall_col_group;
            objs[i]->activate();
            objs[i]->getBroadphaseHandle()->m_collisionFilterMask = mask;

        }else{
            short mask = wall_col_group;
            //objs[i]->activate();
            objs[i]->getBroadphaseHandle()->m_collisionFilterMask = mask;
        }
    }
}

void ad_grav_wall::change_attr(){

    for( int i=0; i<shapes.size(); i++ ){
        
        //if( shapes[i]->getRigidBody()->getUserPointer() != NULL)
          //  continue;
        
        ofVec3f pos = shapes[i]->getPosition();

        bool find = false;
        float nearest = 99999999;
        for (int j=0; j<waterlines.size(); j++) {

            gvWall * wl = waterlines[j];
            ofVec2f &p1 = wl->p1;
            ofVec2f &p2 = wl->p2;
            ofVec2f dir = ad_geom_util::vec_pl( pos, p1, p2);
            ofPoint intp;
            bool isIntersect = ofLineSegmentIntersection(p1, p2, pos, pos+dir*2.0, intp);
            
            if( isIntersect ){
                float dist = dir.length();
                if( dist < nearest ){
                    // re register gbWall
                    shapes[i]->getRigidBody()->setUserPointer( wl );
                    nearest = dist;
                    find = true;
                }
            }
        }
        
        // Dead spot
        nearest = 99999999;
        if( find == false ){
            
            btVector3 random_force( ofSignedNoise(1, i), ofSignedNoise(2, i), 0);
            random_force *= 200.0;
            shapes[i]->getRigidBody()->applyCentralForce( random_force );
            
            for (int j=0; j<waterlines.size(); j++) {
                
                gvWall * wl = waterlines[j];
                ofVec2f &p1 = wl->p1;
                ofVec2f &p2 = wl->p2;
             
                float dist1 = p1.distance(pos);
                float dist2 = p2.distance(pos);
                
                if( dist1 < nearest ){
                    nearest = dist1;
                    shapes[i]->getRigidBody()->setUserPointer( wl );
                }
                if( dist2 < nearest ){
                    nearest = dist2;
                    shapes[i]->getRigidBody()->setUserPointer( wl );
                }
            }
        }
    }
}

void ad_grav_wall::reset_attr(){
   for( int i=0; i<shapes.size(); i++ ){
        shapes[i]->getRigidBody()->setUserPointer( NULL );
    }
}

void ad_grav_wall::change_size( float size ){

    int n = sphereShapes.size();
    for( int i=0; i<n; i++ ){
        btSphereShape * s = sphereShapes[i];
        s->setLocalScaling( btVector3( size, size, size ) );
    }
}
