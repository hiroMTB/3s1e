#include "testApp.h"

void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	
	string path = "water_mesh.abc";
	
	abc.open(path);
	
	abc.dumpNames();
    
    cam.setDistance( 15 );
//    cam.setNearClip( 4 );
//    cam.setFarClip( 6 );

    ofVboMesh m;
    m.setUsage( GL_DYNAMIC_DRAW );

    for( int i=0; i<4; i++ ){
        m.setMode( OF_PRIMITIVE_LINES );
        lines.push_back( m );
        
        m.setMode( OF_PRIMITIVE_POINTS );
        points.push_back( m );
    }
}

void testApp::exit(){
	abc.close();
}

void testApp::update(){
	float t = fmodf( ofGetElapsedTimef(), abc.getMaxTime() );
	
	// update alemblic reader with time in sec
	abc.setTime( t );
}

void testApp::draw(){
    static float angle = 0;
    angle += 0.3;
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    ofBackground( 255 );
	cam.begin();

	glPointSize(1);
	
    bool test1 = false;
	if( test1 ){
		ofMesh mesh;
//		abc.get("realflowMeshShape1'", mesh);   // does not work with name
        abc.get(5, mesh);

        ofPushMatrix();
        ofPushMatrix();
        ofTranslate(0, -2, 0);
        ofRotate(angle+45, 0, 1, 0);
        ofSetColor( 200, 200, 200, 30 );
        mesh.draw( OF_MESH_POINTS );
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(0, -1, 0);
        ofRotate(-angle*0.33, 0, 1, 0);
        ofSetColor( 170, 10, 80, 70 );
        mesh.draw( OF_MESH_POINTS );
        ofPopMatrix();
        
        ofTranslate(0, 0, 0);
        ofRotate(0, 0, 1, 0);
        ofSetColor( 0, 120, 160, 50 );
        mesh.draw( OF_MESH_POINTS );
        ofPopMatrix();
    }

    bool test2 = true;
    if( test2 ){
        
        for( int i=0; i<lines.size(); i++ ){
            lines[i].clear();
        }
        
        for( int i=0; i<points.size(); i++ ){
            points[i].clear();
        }
        
        ofMesh mesh;
        abc.get(5, mesh);
        
        int num_vertex = mesh.getNumVertices();
        int line_num = 1000;
        for( int i=0; i<line_num; i++ ){
            
            int index1 = floor( ofRandom( 0, num_vertex ));
            int index2 = floor( ofRandom( 0, num_vertex ));

            ofVec3f p1 = mesh.getVertex( index1 );
            ofVec3f p2 = mesh.getVertex( index2 );
            
            float dist = p1.distance( p2 );
            if( ofRandom(1.0)>0.9){
                dist *= 0.4;
            }
            
            if( 1.0<dist && dist<1.5 ){
                
                int type = ( 5.0-log10(ofRandom(1.0, 10000.0)) ) - 1.0;
                type = MAX( 0, type );

                lines[type].addVertex( p1 );
                lines[type].addVertex( p2 );
                
                ofFloatColor c1, c2;
                c1.white; c2.white;
                c1.setHsb( ofRandom(1.0), 0.3, 0.6, 0.6 - type*0.1 );
                c2.setHsb( ofRandom(1.0), 0.3, 0.6, 0.6 - type*0.1 );
                lines[type].addColor( c1 );
                lines[type].addColor( c2 );
            }
                int type = ( 5.0-log10(ofRandom(1.0, 10000.0)) ) - 1.0;
                type = MAX( 0, type );

                points[type].addVertex( p1 );
                points[type].addVertex( p2 );
                ofFloatColor c1, c2;
                c1.white; c2.white;
                c1.setHsb( ofRandom(1.0), 0.3, 0.6, 0.6 - type*0.1 );
                c2.setHsb( ofRandom(1.0), 0.3, 0.6, 0.6 - type*0.1 );

                points[type].addColor( c1 );
                points[type].addColor( c2 );
            
        }

        ofSetColor( 255 );
        for( int i=0; i<lines.size(); i++ ){
            glLineWidth( (lines.size()-i) *0.7);
            lines[i].setMode( OF_PRIMITIVE_LINES );
            lines[i].drawWireframe();
        }

        for( int i=0; i<points.size(); i++ ){
            glPointSize( points.size()-i );
            points[i].setMode( OF_PRIMITIVE_POINTS );
            points[i].draw();
        }
        
        glPointSize( 1 );
        ofSetColor( 50, 5 );
        mesh.drawVertices();

    }
    
	cam.end();
    
    ofSetColor( 0 );
    for( int i=0; i<points.size(); i++ ){
        ofDrawBitmapString( "point size" + ofToString(i) + " : " + ofToString( points[i].getNumVertices() ), 10, 20+20*i);
    }

    for( int i=0; i<lines.size(); i++ ){
        ofDrawBitmapString( "line width" + ofToString(i) + " : " + ofToString( lines[i].getNumVertices() ), 300, 20+20*i);
    }

}

void testApp::keyPressed( int key ){
}