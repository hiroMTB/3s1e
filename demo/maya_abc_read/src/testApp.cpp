#include "testApp.h"
#include "ofxAlembic.h"

ofEasyCam cam;

ofxAlembic::Reader abc;

void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	
	string path = "maya_export_legacy.abc";
	
	abc.open(path);
	
	abc.dumpNames();
    cam.setDistance( 15 );
}

void testApp::exit(){
	abc.close();
}

void testApp::update()
{
	float t = fmodf(ofGetElapsedTimef(), abc.getMaxTime());
	
	// update alemblic reader with time in sec
	abc.setTime( t * 4 );
}


void testApp::draw(){
    static float angle = 0;
    angle += 0.1;
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    ofBackground( 255 );
	cam.begin();

	
	glPointSize( 5 );
	glLineWidth( 1 );
	// get meshes and draw
    
	{
		ofMesh mesh;
//		abc.get("realflowMeshShape1'", mesh);   // does not work with name
        abc.get(1, mesh);

        ofPushMatrix();
        ofPushMatrix();
        ofRotate(angle+45, 0, 1, 0);
        ofSetColor( 30 );
        mesh.draw( OF_MESH_WIREFRAME );

		ofSetColor( 0, 0, 255 );
		mesh.draw( OF_MESH_POINTS );
		ofPopMatrix();

	
    /*{
		vector<ofVec3f> points;
		abc.get("/Emitter/EmitterCloud", points);
		
		ofSetColor(0, 255, 0);
		glBegin(GL_POINTS);
		for (int i = 0; i < points.size(); i++)
			glVertex3fv(points[i].getPtr());
		glEnd();
	}

	{
		vector<ofPolyline> curves;
		abc.get("/Tracer/TracerSpline", curves);
		
		ofSetColor(0, 0, 255);
		for (int i = 0; i < curves.size(); i++)
			curves[i].draw();
	}
	*/
    
	cam.end();
	
	ofDrawBitmapString(ofToString(abc.getTime()) + "/" + ofToString(abc.getMaxTime()), 10, 20);
}
}