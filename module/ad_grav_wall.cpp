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

int ad_grav_wall::particle_col_group = 1;
int ad_grav_wall::wall_col_group = 2;
int ad_grav_wall::particle_col_setting = wall_col_group;
int ad_grav_wall::wall_col_setting = particle_col_group;

int ad_grav_wall::pid = 0;

ad_grav_wall::ad_grav_wall()
:bInitPhysics(false),
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
            float size = 0.5 + i*i*0.002;
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
        float start_rate = ofRandom(0.1, 0.4);
        int start_id = side_num * start_rate;

        float expand_rate = ofRandom(1.1, 1.75);
        for(int i=0; i<side_num; i++){
        
            pid++;

            float logf = log10(3+i)/log10(3+side_num-1);
            int add = side_num * logf * (0.8-start_rate);
            add += start_id;
            if( add >= side_num )
                add = side_num - (add%side_num * expand_rate);
            
            ofVec3f pivot = p1 + adder*add;
            
            float dist;
            if( ofNoise(pid*0.0072) > 0.4){
                // stream
                dist = ofNoise( j*13+i*0.04 )*120 + ofNoise(j*12 + i*0.7)*30.0;
                dist *=1.3;
            }else{
                dist = ofNoise( j, i*0.05 )*120 + ofNoise(j, i*0.5)*30.0;
                dist *= (1 + ofNoise(pid*0.007)*2.0);
            }
            
            dist *= 2.0;
            dist += add*add*0.00001;
            
            if( add > side_num*0.9 )
                dist *= 0.5;
            else if(add < side_num*0.5)
                dist *= 0.5;
            
            if( ofRandomuf()>0.96){
                dist *= 3;
            }
            
            dist = MAX(110, dist);
            dist *= updown;
            
            ofVec3f pos = pivot + gvw->norm*dist;
//            pos.y += ofSignedNoise(pid*0.0021) * 50;
//            pos.x += ofSignedNoise(nshapes+pid*0.00001) * 40;
            
            pivots.push_back( pivot );
            
            ofxBulletSphere * s= new ofxBulletSphere();
            
            float mass = 5.0 - ofNoise(i, pos.y);
            float size = ofNoise(i+300,mass)*30.0 + i*0.001;
            size = ofMap(size, 0, 30.0, 0, 29, true);
            
            float friction = ofApp::app->getNoise(i+400,2) * 10.0;
            float restriction = 1 + ofNoise(pid*0.01)*0.25;
            
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
                
                if( ofRandomuf()>0.8 ){
                    c.r += 0.7;
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
    
    {
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
        wall_thickness = 40;
        ofVec3f size( length+3, wall_thickness, 1000 );
        w->create( world.world, center, quat, mass, size.x, size.y, size.z );

        float restriction = 1.52;
        float friction = 0.95;
        w->setProperties( restriction, friction );
        w->add( wall_col_group, wall_col_setting );
        w->getRigidBody()->setUserPointer( NULL );
        
        walls.push_back( w );
    }
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
            body->applyCentralForce( grav * impulse * 150 );
        }
    }
}

void ad_grav_wall::update_lines(){

    lines.clear();
    prep_lines.clear();
    polys.clear();
    
    const vector<ofFloatColor> & pcol = points.getColors();
    
	btCollisionObjectArray& objs = world.world->getCollisionObjectArray();
	for( int i=0; i<objs.size(); i++ ){
        
        float nR = ofApp::app->getNoise(i,0);
        float nG = ofApp::app->getNoise(i,1);
        float nB = ofApp::app->getNoise(i,2);
        
		btTransform &trans = objs[i]->getWorldTransform();
		btVector3 &bpos = trans.getOrigin();
		ofVec3f pos1( bpos.x(), bpos.y(), bpos.z() );
        pos1.z = 0;
        int num_line = 1 + (nR+nG);
        bool bStatic1 = objs[i]->isStaticObject();
        if( bStatic1 ) continue;
        
        multimap<float, ofVec3f> near_p;
        pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
        for( int line=0; line<num_line; line++ ){
            near_p.insert( pair1 );
        }
        

        for( int j=i+1; j<objs.size(); j++ ){

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
        
#pragma mark NEAR_LINE
        for(; itr!=near_p.end(); itr++ ){
            ofVec3f &pos2 = itr->second;
            if(pos2.x == -12345) continue;
            
            float d = pos1.distance(pos2);
            float limit = 200;
            
            if( d<2 || limit<d  ) continue;

            ofFloatColor c = ofFloatColor(0.8, 0.8) - pcol[i];
            c.r += nR*0.1 + ofRandomf()*0.01;
            c.g += nG*0.1 + ofRandomf()*0.01;
            c.b += nB*0.1  + ofRandomf()*0.01;
            lines.addVertex( pos1 );
            lines.addVertex( pos2 );
            lines.addColor( c );
            lines.addColor( c );
        }
        
        
        // pivot line
        
        gvWall * gl = static_cast<gvWall*>(objs[i]->getUserPointer());
        if( gl != NULL){
            ofVec3f toline = ad_geom_util::vec_pl(pos1, gl->p1, gl->p2);
            ofVec3f onLine = pos1 + toline;
            float len = toline.length();
            bool isOnline = ad_geom_util::isOnline(onLine, gl->p1, gl->p2);
            if( len < 300 && isOnline ){
                
                if( ofApp::app->getNoise(i,2) + ofNoise(i*0.002) > 1.45 ){
                    
                    prep_lines.addVertex(pos1);
                    prep_lines.addVertex(onLine);
                    
                    ofFloatColor c = pcol[i];
                    c.b *= 0.2;
                    c *= 0.8;
                    c.a = 0.4;
                    prep_lines.addColor( c );
                    prep_lines.addColor( c );
                }

                
                if ( 10<len && len<1500 ) {
                    if( ofApp::app->getNoise(i)>0.3) continue;
                    
                    int n = ofApp::app->getNoise( i ) * 5;
                    for (int k=0; k<n; k++) {
                        ofVec3f dir = gl->p2-gl->p1;
                        ofVec3f dirn = dir.getNormalized();
                        int step = ofRandom(14, 20);
                        float curb_rate = ofRandom(0.6, 3.5);
                        
                        if(len < 120) curb_rate *= 2.3;

                        ofVec3f adder = toline/step;
                        
                        ofPolyline poly;
                        poly.addVertex( pos1 );
                        int pn = 4;

                        dirn *= ofNoise( i*0.03 )<0.25 ? -1 : 1;
                        for (int l=0; l<step; l++) {
                    
                            ofVec3f p = pos1 + adder * (pow((float)l,pn)/pow((float)step-1,pn)) * step;
                            p += dirn * l* log10(i)/log10(step-1) * curb_rate;
                            poly.addVertex( p );
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

        // keep point coloor from image
        cols[i].r += ofApp::app->gpu_noise.getNoisef(i, 1) * 0.0002;
        cols[i].g += ofApp::app->gpu_noise.getNoisef(i, 2) * 0.0002;
        cols[i].b += ofApp::app->gpu_noise.getNoisef(i, 3) * 0.0002;
    }
}

void ad_grav_wall::draw(){
    ofNoFill();
    ofSetLineWidth( 1 );
	for( int i=0; i<gvws.size(); i++ ){
		ofSetColor( 0 );
		ofLine( gvws[i]->p1, gvws[i]->p2 );
	}

    glPointSize( 2 );
    points.draw();

    glLineWidth( 1 );
    lines.draw();

    for( int i=0; i<polys.size(); i++ ){
        ofSetColor(10,0,60, 170);
        polys[i].draw();
    }

    
    prep_lines.draw();
}

void ad_grav_wall::onCollision( ofxBulletCollisionData &cdata ){
    //if(frame > 30)
    //    collision.insert( collision.end(), cdata.worldContactPoints1.begin(), cdata.worldContactPoints1.end() );
}

