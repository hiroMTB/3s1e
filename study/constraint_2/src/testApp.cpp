#include "testApp.h"
#include "ad_util.h"

void testApp::setup() {
	ofBackground( 0 );
	
	camera.setPosition(ofVec3f(0, 0, -600.f));
	camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));
	
	world.setup();
    world.setGravity( ofVec3f(0,10,0) );
    world.setCamera(&camera);
    
    svg.load( ad_util::data_path + "svg/ABC.svg" );
    for (int i=0; i<; i++) {
        
    }
    
    ofVec3f start1(-500, 0, 0);
    ofVec3f end1(-300, 120, 0);
    cline1 = new constraint_line(&world, start1, end1, 200, 30, 1);
    
    ofVec3f start2 = end1;
    ofVec3f end2(300, 140, 0);
    cline2 = new constraint_line(&world, start2, end2, 200, 30, 1);

    ofVec3f start3 = end2;
    ofVec3f end3(500, -40, 0);
    cline3 = new constraint_line(&world, start3, end3, 200, 30, 1);

    
    // Attractor
    int n = 30;
    for( int i=0; i<n; i++){
        ofVec3f pos(i - n*0.5, 0, 0);
        pos.y -= ofNoise(i*0.1)*150 + ofNoise(i*0.013)*300 + 200;
        pos.x *= 1.2;
        attrs.push_back(Attr(pos, 40000, ofNoise(i*0.001)*2.0 + 1) );
    }
}

void testApp::update() {
	world.update(1/30.0, 10);

    cline1->update();
    cline2->update();
    cline3->update();
    
    // move
    for(int a=0; a<attrs.size(); a++ ){
        attrs[a].pos.rotate(attrs[a].speed, ofVec3f(1,0,0) );
    
        // atract
//        for(int i=0; i<shapes.size(); i++ ){
//            ofVec3f p = shapes[i]->getPosition();
//
//            // pull
//            ofVec3f dir = (attrs[a].pos - p);
//            float dist2 = dir.lengthSquared();
//            
//            if( dist2 < 30 || 1000000 < dist2) continue;
//            ofVec3f impl = dir.normalized() * (attrs[a].power / (dist2+0.0000000001) );
//            shapes[i]->getRigidBody()->applyCentralImpulse( btVector3(impl.x, impl.y, impl.z) );
//            // p.z = 0;
//            vecp[i] = p;
//            linep[i*2] = p;
//        }
    }
    
    // Move Pivot
    //ofVec3f &pivot = linep[i*2 +1];
        //pivot.y -= 1;
        //joints[i]->updatePivotPos(pivot, 1);
    //}
}

void testApp::draw() {

    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    
    glPointSize(1);
    glLineWidth(1);
    ofFill();
    
    camera.begin();
	
    cline1->draw();
    cline2->draw();
    cline3->draw();
    
    if( bDrawDebug ){
        world.drawDebug();
        for( int i=0; i<attrs.size(); i++){
            attrs[i].draw();
        }
    }
	
	camera.end();
	
	ofSetColor(255, 255, 255);
	stringstream ss;
	ss << "framerate      : " << ofGetFrameRate() << endl;
    ss << "frame          : " << ofGetFrameNum() << endl;
	ss << "draw debug (d) : " << ofToString(bDrawDebug, 0) << endl;
	ofDrawBitmapString(ss.str().c_str(), 10, 10);
}

void testApp::keyPressed(int key) {
	switch (key) {
		case 'd':
			bDrawDebug = !bDrawDebug;
			break;
		default:
			break;
	}
}
