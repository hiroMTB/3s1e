#include "testApp.h"

testApp::testApp(){
	ofSetWindowPosition( 0, 0 );
	bDraw_info = true;
    bStart = false;
    bAnimate = true;
    bRotate = false;
    bOrtho = false;
    bDraw_connection_between_agnet = true;
    bDraw_connection_inside_of_agent = false;
    bDraw_agent = true;
    bSequencial_add = true;
    initial_radius = 200;
    current_setting_start_frame = 0;
    sequencial_add_speed = 1;
    center.set( 0, 0, 0 );
	current_la_pos.set( 0, 0, 0 );
}

void testApp::setup(){
	
    in_angle = ofRandom( -360, 360 );
    out_angle = ofRandom( -360, 360 );
    
    connection_between_agent.setMode( OF_PRIMITIVE_LINES );
    connection_between_agent.setUsage( GL_DYNAMIC_DRAW );
    
	ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    ofEnableSmoothing();
//    ofEnableAntiAliasing();
    ofSetVerticalSync( true );
    
    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    win.x = 1920*2;
    win.y = 1080*2;
    
    exporter.setup( win.x, win.y, 25, GL_RGB, 0 );
    exporter.setFilePattern(  dir_name + "/F_%05i.png");
    exporter.setFrameRange( 0, 3001 );
    exporter.setAutoExit( true );
    
    ofSetWindowShape(win.x/4, win.y/4);
    ofSetWindowPosition(0, 0);
}

void testApp::update(){
    int frame = ofGetFrameNum();
    float step_angle = (out_angle-in_angle)/ 3000.0;
    
    if( frame%50 == 0 ){
        change_settings();
    }
	
    if( bStart ){
		LineAgent l;
		if( bSequencial_add){
			l.setup( in_angle + step_angle*(float)frame*sequencial_add_speed, initial_radius, center );
		}else{
			l.setup( ofRandom(in_angle, out_angle), initial_radius, center );
		}
		la.push_back( l );
		
		current_la_pos = l.pos;
    }
    
    if( bStart ){
        for ( int i=0; i<la.size(); i++ ) {
            if( ofRandom(1.0)<0.3 ||i==la.size()-1 ){
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
	
    exporter.begin(); {
        ofBackground( 255 );
        ofSetColor( 255 );
        if( bOrtho ){
            ofSetupScreenOrtho();
        }
        
        ofPushMatrix();
        ofTranslate( win.x/2, win.y );
        if( bRotate ){
            ofRotate( ofGetFrameNum()*0.05, 1, 0, 0 );
        }
        
        if( bDraw_connection_between_agnet ){
            draw_connection_between_agnet();
        }
        
        
        for ( int i=0; i<la.size(); i++ ) {
            if( bDraw_agent ){
                //if( ofRandom(1.0)>0.2 ){
                la[i].draw();
                //}
            }
            if( bDraw_connection_inside_of_agent ){
                la[i].draw_connection_inside_of_agent();
            }
        }
        
        ofPopMatrix();
    }exporter.end();
    
    ofPushMatrix(); {
        ofBackground(0);
        ofSetColor(255);
        exporter.draw(0, 0);
    } ofPopMatrix();
    
    draw_info();
}

void testApp::draw_connection_between_agnet(){
    if( !bStart )
        return;
    
    connection_between_agent.clear();
    
    int num_line = la.size() * 0.1;
    
    for( int i=0; i<num_line; i++ ){
        
        int agent1 = floor( ofRandom(0, la.size()) );
		
		for( int j=0; j<300; j++ ){
			int agent2 = floor( ofRandom(0, la.size()) );
		
			int nv1 = la[agent1].trail.getNumVertices();
			int nv2 = la[agent2].trail.getNumVertices();
		
			if( nv1==0 || nv2==0 ) break;
			
			int index1 = floor( ofRandom(0, nv1) );
			int index2 = floor( ofRandom(0, nv2) );
			
			ofVec3f p1 = la[agent1].trail.getVertex( index1 );
			ofVec3f p2 = la[agent2].trail.getVertex( index2 );
			
			float d = p1.distance( p2 );
			if( 10<d && d<60 ){
				connection_between_agent.addVertex( p1 );
				connection_between_agent.addVertex( p2 );
				connection_between_agent.addColor( la[agent1].trail.getColor( index1 )*0.1 );
				connection_between_agent.addColor( la[agent2].trail.getColor( index2 )*0.1 );
			}
		}
    }
	
	for( int i=ofRandom(10, 100); i<la.size(); ){
		int last = la[i].trail.getNumVertices()-1;
		if( last>2 ){
			connection_between_agent.addVertex( ofVec3f(0,0,0) );
			connection_between_agent.addVertex( la[i].trail.getVertex(last) );
			connection_between_agent.addColor( ofFloatColor(0, ofRandom(0.2, 0.6)) );
			connection_between_agent.addColor( ofFloatColor(0, ofRandom(0.2, 0.6)) );
		}
		
		i+= ofRandom( 1, 200 );
	}
	
    glLineWidth( 1 );
    connection_between_agent.drawWireframe();
}

void testApp::change_settings(){
    in_angle = ofRandom( 0, 360 );
    out_angle = in_angle + ofRandom( 100, 360 );
    initial_radius = ofRandom( 300, 900 );
    sequencial_add_speed = ofRandom( 6, 8 );
    current_setting_start_frame = ofGetFrameNum();
    center.set( 0, 0, 0 );
}

void testApp::draw_info(){
    if( !bDraw_info) return;
    ofSetColor( 200 );
    ofRect( 5, 5, 600, 300 );
    int y = 20;
    ofSetColor( 0 );
    stringstream ss;
    ss << "fps       : " << (int)ofGetFrameRate() << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
    ss << "space key : start genetic calculation\n";
    ss << "m     key : start animation\n";
    ss << "o     key : toggle ortho graphic view\n";
    ss << "i     key : draw connection line inside of line agent\n";
    ss << "b     key : draw connection line between line agents\n";
    ss << "r     key : toggle rotatation\n";
    ss << "a     key : draw line agent\n";
    ss << "s     key : sequencial add\n";
    ss << "I     key : draw info\n";
    ss << "c     key : change settings\n";
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

        case 's':
            bSequencial_add = !bSequencial_add;
            break;

        case 'c':
            change_settings();
            break;

        case 'S':
            bStart = true;
            exporter.startExport();
            break;
            
        default:
            break;
	}
}
