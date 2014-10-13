#include "testApp.h"

testApp::testApp(){
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
}

void testApp::setup(){

    num_agent = 3000;

    in_angle = ofRandom( -360, 360 );
    out_angle = ofRandom( -360, 360 );
    
    connection_between_agent.setMode( OF_PRIMITIVE_LINES );
    connection_between_agent.setUsage( GL_DYNAMIC_DRAW );
    
	ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetVerticalSync( true );

    glEnable( GL_MULTISAMPLE );
    GLint buf, sbuf;
    glGetIntegerv( GL_SAMPLE_BUFFERS, &buf );
    glGetIntegerv( GL_SAMPLES, &sbuf );
    cout << "Number of sample buffers is " + ofToString( buf ) << endl;
    cout << "Number of samples is " + ofToString( sbuf ) << endl;
}

void testApp::update(){

    float step = ofGetFrameNum();
    float step_angle = (out_angle-in_angle)/num_agent;
    int w = 0; //ofGetWidth();
    int h = 0; //ofGetHeight();
    
    if( la.size() < num_agent){
        LineAgent l;
        if( bSequencial_add){
            l.setup( in_angle + step_angle*step*sequencial_add_speed, initial_radius, center );
        }else{
            l.setup( ofRandom(in_angle, out_angle), initial_radius, center );
        }
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
    ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
    if( bRotate ){
        ofRotate( ofGetFrameNum()*0.05, 1, 0, 0 );
    }

    if( bDraw_connection_between_agnet ){
        draw_connection_between_agnet();
    }
    
    
    for ( int i=0; i<la.size(); i++ ) {
//        ofTranslate( 1, 1 );
        if( bDraw_agent ){
            if( ofRandom(1.0)>0.2 ){
                la[i].draw();
            }
        }
        if( bDraw_connection_inside_of_agent ){
            la[i].draw_connection_inside_of_agent();
        }
	}
    
    ofPopMatrix();

    saver.save();
    
    draw_info();
}

void testApp::draw_connection_between_agnet(){
    
    connection_between_agent.clear();
    
    int num_line = 2000;
    
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
        if( 30<d && d<180 ){
            connection_between_agent.addVertex( p1 );
            connection_between_agent.addVertex( p2 );
            connection_between_agent.addColor( la[agent1].trail.getColor( index1 )*0.1 );
            connection_between_agent.addColor( la[agent2].trail.getColor( index2 )*0.1 );
        }
    }
    
    glLineWidth( 1 );
    connection_between_agent.drawWireframe();
}

void testApp::change_settings(){
    in_angle = ofRandom( 0, 360 );
    out_angle = in_angle + ofRandom( -360, 360 );
    initial_radius = ofRandom( 10, 400 );
    sequencial_add_speed = ofRandom( 5, 10 );
    current_setting_start_frame = ofGetFrameNum();
    center.set( ofRandom(-200, 200), ofRandom(-200, 200), ofRandom(-10, 10) );
    
     //nice seting
//     in_angle = 360; //ofRandom( 0, 360 );
//     out_angle = -20; //ofRandom( 0, 360 );
//     initial_radius = 400; //ofRandom( 10, 400 );
//     sequencial_add_speed = 10; //ofRandom( 1, 10 );
//     current_setting_start_frame = ofGetFrameNum();
}

void testApp::draw_info(){
    if( !bDraw_info) return;
    ofSetColor( 200 );
    ofRect( 5, 10, 600, 300 );
    int y = 20;
    ofSetColor( 0 );
    ofDrawBitmapString( "space key : start genetic calculation", 20, y+=20 );
    ofDrawBitmapString( "m     key : start animation",           20, y+=20 );
    ofDrawBitmapString( "o     key : toggle ortho graphic view", 20, y+=20 );
    ofDrawBitmapString( "i     key : draw connection line inside of line agent", 20, y+=20 );
    ofDrawBitmapString( "b     key : draw connection line between line agents", 20, y+=20 );
    ofDrawBitmapString( "r     key : toggle rotatation",         20, y+=20 );
    ofDrawBitmapString( "a     key : draw line agent",           20, y+=20 );
    ofDrawBitmapString( "s     key : sequencial add",            20, y+=20 );
    ofDrawBitmapString( "I     key : draw info",                 20, y+=20 );
    ofDrawBitmapString( "c     key : change settings",           20, y+=20 );

    ofDrawBitmapString( "S     key : save image",                20, y+=20 );
}

void testApp::keyPressed( int key ){

    switch( key ) {
		case ' ':
            bStart = !bStart;
            break;
    
        case 'f':
//            ofToggleFullscreen();
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
            saver.start( ofGetTimestampString(), "gm2.1_", 500 );
            break;
            
        default:
            break;
	}
}
