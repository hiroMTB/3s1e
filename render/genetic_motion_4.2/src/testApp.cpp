#include "testApp.h"

testApp::testApp(){
	ofSetWindowPosition(0, 0);
	bDraw_info = true;
    bStart = false;
    bAnimate = true;
    bRotate = false;
    bOrtho = false;
    bDraw_connection_between_agnet = true;
    bDraw_connection_inside_of_agent = false;
    bDraw_agent = true;
    center.set( 0, 0, 0 );
}

void testApp::setup(){

    connection_between_agent.setMode( OF_PRIMITIVE_LINES );
    connection_between_agent.setUsage( GL_DYNAMIC_DRAW );
    
	ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetVerticalSync( true );

}

void testApp::update(){
    int frame = ofGetFrameNum();
    
    if( bStart ){
        LineAgent l;
        l.setup( center );
        la.push_back( l );
    }
    
    if( bStart ){
        for ( int i=0; i<la.size(); i++ ) {
            if( ofGetKeyPressed() || ofRandom(1.0)<0.3 ||i==la.size()-1 ){
                la[i].dna.setBoundsMode( i%3 );
                la[i].dna.mutate(  ofMap(mouseX, 0, ofGetWidth(), 0, 0.5)*0.4 );
            }else{
                la[i].dna.setBoundsMode( i%3 );
                la[i].dna.setMateMode( floor(ofRandom(0,4)) );
                la[i].dna.mate( la[i+1].dna, ofMap(mouseX, 0, ofGetWidth(), 0, 0.1) *0.4 );
            }
            
            la[i].add_result();
            
            if( bAnimate ){
                la[i].animate();
                la[i].animate_noise( i );
            }
        }
    }
}

void testApp::draw(){
	
	ofBackground( 255 );
    ofSetColor( 255 );
    if( bOrtho ){
        ofSetupScreenOrtho();
    }
    
    ofPushMatrix();
    ofTranslate( 300, 100 );
    if( bRotate ){
        ofRotate( ofGetFrameNum()*0.05, 1, 0, 0 );
    }

    if( bDraw_connection_between_agnet ){
        draw_connection_between_agnet();
    }
    
    
    for ( int i=0; i<la.size(); i++ ) {
        if( bDraw_connection_inside_of_agent ){
            la[i].draw_connection_inside_of_agent();
        }

        if( bDraw_agent ){
            la[i].draw();
        }

    }
    
    ofPopMatrix();

    saver.save();
    
    draw_info();
}

void testApp::draw_connection_between_agnet(){
    if( !bStart )
        return;
    
    connection_between_agent.clear();
    
    int num_line = la.size() * 0.4;
    
    for( int i=0; i<num_line; i++ ){
        
        int agent1 = floor( ofRandom(0, la.size()) );
        int agent2 = floor( ofRandom(0, la.size()) );
    
        int nv1 = la[agent1].trail.getNumVertices();
        int nv2 = la[agent2].trail.getNumVertices();
    
        if( nv1==0 || nv2==0 ) break;
        
        int index1 = floor( ofRandom(0, nv1) );
        int index2 = floor( ofRandom(0, nv2) );
        
        ofVec3f p1 = la[agent1].trail.getVertex( index1 );
        ofVec3f p2 = la[agent2].trail.getVertex( index2 );
        
        float d = p1.distance( p2 );
        if( 30<d && d<100 ){
            connection_between_agent.addVertex( p1 );
            connection_between_agent.addVertex( p2 );
            connection_between_agent.addColor( la[agent1].trail.getColor( index1 )*0.7 );
            connection_between_agent.addColor( la[agent2].trail.getColor( index2 )*0.7 );
        }
    }
    
    glLineWidth( 1 );
    connection_between_agent.drawWireframe();
}

void testApp::draw_info(){
    if( !bDraw_info) return;
    ofSetColor( 200 );
    ofRect( 5, 5, 600, 300 );
    int y = 20;
    ofSetColor( 0 );
    stringstream ss;
    ss << "fps       : " << (int)ofGetFrameRate() << "\n";
    ss << "cur frame : " << saver.frame_cur << "\n";
    ss << "end frame : " << saver.frame_end << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
    ss << "space key : start genetic calculation\n";
    ss << "m     key : start animation\n";
    ss << "o     key : toggle ortho graphic view\n";
    ss << "i     key : draw connection line inside of line agent\n";
    ss << "b     key : draw connection line between line agents\n";
    ss << "r     key : toggle rotatation\n";
    ss << "a     key : draw line agent\n";
    ss << "I     key : draw info\n";
    ss << "S     key : save image\n";
    ss << "f     key : toggle fullscreen\n";
    ofDrawBitmapString( ss.str(), 20, 20 );
}

void testApp::keyPressed( int key ){

    switch( key ) {
		case ' ':
            bStart = !bStart;
            break;
    
        case 'f':
			ofSetWindowShape( 1920*2, 1080*2 );
			break;

        case 'm':
            bAnimate = !bAnimate;
            break;
        
        case 'o':
            bOrtho = !bOrtho;
            break;
            
       case 'i':
            bDraw_connection_inside_of_agent = !bDraw_connection_inside_of_agent;
            break;

        case 'b':
            bDraw_connection_between_agnet = !bDraw_connection_between_agnet;
            break;

        case 'r':
            bRotate = !bRotate;
            break;

        case 'a':
            bDraw_agent = !bDraw_agent;
            break;
        
        case 'I':
            bDraw_info = !bDraw_info;
            break;

        case 'S':
            saver.start( ofGetTimestampString(), "", 3000 );
            break;
            
        default:
            break;
	}
}
