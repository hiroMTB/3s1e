#include "testApp.h"

testApp::testApp(){
    ofSetWindowPosition( 0, 0 );
    bDraw_info = true;
    bStart = false;
}

void testApp::setup(){

	ofSetFrameRate( 60 );
    ofVec3f center( 0, 0, 0 );

    int res = 6000;
    for ( int i=0; i<res; i++ ) {
        LineAgent l;
        l.setup( center, 300, i, res );
        la.push_back( l );
	}
	
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
}

void testApp::update(){

    if( bStart ){
        for ( int i=0; i<la.size(); i++ ) {
            if( ofRandom(1.0)<0.3 ){
                la[i].dna.setBoundsMode( i%3 );
                la[i].dna.mutate( ofMap(mouseX, 0, ofGetWidth(), 0, 0.5)*0.4 );
            }else{
                la[i].dna.setBoundsMode( i%3 );
                la[i].dna.setMateMode( floor(ofRandom(0,4)) );
                if( i != la.size()-1){
                    la[i].dna.mate( la[i+1].dna, ofMap(mouseX, 0, ofGetWidth(), 0, 0.1) *0.4 );
                }else{
                    la[i].dna.mate( la[0].dna, ofMap(mouseX, 0, ofGetWidth(), 0, 0.1) *0.4 );
                }
            }
            
            la[i].update_agent();
            la[i].add_result();
            
            la[i].animate();
            la[i].animate_noise( i );
        }
    }
    
    
    lines.clear();
    int num_lines = la.size() * 0.06;
    for( int i=0; i<num_lines; i++ ){
        
        int la1 = ofRandom(0, la.size()-1 );
        int la2 = la1 + la.size()/2 + ofRandom( 0, la.size()*0.4 );
        la2 %= la.size();
    
        int index1 = ofRandom(0, la[la1].trail.getNumVertices() );
        int index2 = ofRandom(0, la[la2].trail.getNumVertices() );
        
        if( la[la1].trail.getNumVertices()>0 || la[la2].trail.getNumVertices()>0 ){
            ofVec3f p1 = la[la1].trail.getVertex( index1 );
            ofVec3f p2 = la[la2].trail.getVertex( index2 );
            float d = p1.distance(p2);
            if( 10<d && d<220 ){
                lines.addVertex( p1 );
                lines.addVertex( p2 );
                lines.addColor( la[la1].trail.getColor(index1) );
                lines.addColor( la[la2].trail.getColor(index2) );
            }
        }
    }
}

void testApp::draw(){
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
	ofBackground( 255 );
    
    ofSetColor( 255 );
    ofSetupScreenOrtho();

    ofPushMatrix(); {
        ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
        
        glLineWidth( 1 );
        lines.draw();

        for( int i=0; i<la.size(); i++ ) {
            la[i].draw();
        }
    } ofPopMatrix();

    saver.save();
    
    draw_info();
}

void testApp::draw_info(){
    if( !bDraw_info) return;
    ofSetColor( 200 );
    ofRect( 5, 10, 400, 200 );
    int y = 20;
    ofSetColor( 0 );
    stringstream ss;
    ss << "fps       : " << (int)ofGetFrameRate() << "\n";
    ss << "cur frame : " << saver.frame_cur << "\n";
    ss << "end frame : " << saver.frame_end << "\n";
    ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";

    ss << "space key : start\n";
    ss << "f     key : render resolution\n";
    ss << "I     key : draw info\n";
    ss << "S     key : save image\n";
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

        case 'I':
            bDraw_info = !bDraw_info;
            break;

        case 'S':
            bStart = true;
            saver.start( ofGetTimestampString(), "", 3000 );
            break;
        default:
            break;
	}
}
