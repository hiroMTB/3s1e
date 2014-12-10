#include "testApp.h"
#include "ad_util.h"

void testApp::setup() {
	ofBackground( 0 );
	
//	camera.setPosition(ofVec3f(0, 0, -600.f));
//	camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));
	
	world.setup();
    world.setGravity( ofVec3f(0,10,0) );
    world.setCamera(&camera);
    
    svg.load( ad_util::data_path + "svg/v1/ABC.svg" );
    for (int i=0; i<svg.getNumPath(); i++) {
		ofPath & path = svg.getPathAt(i);
		vector<ofPolyline> &polys = path.getOutline();
		for(int j=0; j<polys.size(); j++){
			ofPoint s = polys[j].getVertices()[0];
			ofPoint e = polys[j].getVertices()[polys[j].getVertices().size()-1];
			
			constraint_line cl(&world, s, e, 20, 30, 1);
			clines.push_back( cl );
		}
    }
	
	int w = svg.getWidth() + 1;
	int h = svg.getHeight() + 1;

	cout << "canvas size: " << w << ", " << h << endl;
	exporter.setFrameRange(3000);
	exporter.setup(w, h, 30, GL_RGBA, 4);
	
    // Attractor
    int n = 30;
    for( int i=0; i<n; i++){
        ofVec3f pos(i - n*0.5, 0, 0);
        pos.y -= ofNoise(i*0.1)*150 + ofNoise(i*0.013)*300 + 200;
        pos.x *= 1.2;
        attrs.push_back(ad_attractor(pos, 40000, ofNoise(i*0.001)*2.0 + 1) );
    }
}

void testApp::update() {
	world.update(1/30.0, 10);

	for (int i=0; i<clines.size(); i++) {
		clines[i].update();
	}
	
    // move
    for(int a=0; a<attrs.size(); a++ ){
        //attrs[a].pos.rotate(attrs[a].speed, ofVec3f(1,0,0) );
    
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

	exporter.begin();{
		ofEnableAlphaBlending();
		ofEnableAntiAliasing();
		ofEnableSmoothing();

		ofBackground(0);
		glPointSize(1);
		glLineWidth(1);
		ofFill();
		
		for (int i=0; i<clines.size(); i++) {
			clines[i].draw();
		}
		
		if( bDrawDebug ){
			world.drawDebug();
			ad_attractor::draw_all();
		}
	} exporter.end();
	
	ofBackground(0);
	exporter.draw(0, 0);
	
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
