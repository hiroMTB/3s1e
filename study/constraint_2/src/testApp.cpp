#include "testApp.h"
#include "ad_util.h"

void testApp::setup() {
	ofBackground( 0 );
		
	world.setup();
    world.setGravity( ofVec3f(0,0,0) );
    
    svg.load( ad_util::data_path + "svg/v2/ABC.svg" );
    for (int i=0; i<svg.getNumPath(); i++) {
		ofPath & path = svg.getPathAt(i);
		vector<ofPolyline> &polys = path.getOutline();
		for(int j=0; j<polys.size(); j++){
			ofPoint s = polys[j].getVertices()[0];
			ofPoint e = polys[j].getVertices()[polys[j].getVertices().size()-1];
			constraint_line * cl = new constraint_line(&world, s, e, 500, 100, 1);
			clines.push_back( cl );
            
            ad_attract_line * al = new ad_attract_line( &cl->shapes, s, e, 20, 500, 60000, 0.3 );
            attrs.push_back( al );
		}
    }
	
	int w = svg.getWidth() + 1;
	int h = svg.getHeight() + 1;

	cout << "canvas size: " << w << ", " << h << endl;
	exporter.setFrameRange(3000);
	exporter.setup(w, h, 25, GL_RGB, 4);
    exporter.setAutoExit( true );
}

void testApp::update() {
    
    for (int i=0; i<clines.size(); i++) {
        attrs[i]->update();
    }
    
	world.update();

	for (int i=0; i<clines.size(); i++) {
		clines[i]->update();
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
		
      	ofSetColor( 255 );
		for (int i=0; i<clines.size(); i++) {
			clines[i]->draw();
		}
		
		if( bDrawDebug ){
			//world.drawDebug();
            for (int i=0; i<attrs.size(); i++) {
                attrs[i]->draw();
            }
		}
        
	} exporter.end();
	
	ofBackground(0);
    ofSetColor( 255 );
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
