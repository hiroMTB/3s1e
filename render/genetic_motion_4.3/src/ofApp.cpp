#include "ofApp.h"

ofApp::ofApp(){
	
	//ofSetLogLevel( OF_LOG_VERBOSE );
		
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

	string shader_path = ofToDataPath("") + "../../../../../app/ofxGpuNoise/libs/shader/";
	noise.setup( shader_path );
	noise.setOctaves( 4 );
	noise.setShaderType( ofxGpuNoise::SHADER_TYPE_Perlin );
	noise.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_NO );
	noise.setSendSamplingPoints( false );
	noise.setSamplingPointsScale( 0.001 );
	noise.create( 512, 512 );
}

void ofApp::setup(){

    num_agent = 3000;
    change_settings();
    
    //connection_between_agent.setMode( OF_PRIMITIVE_LINES );
    connection_between_agent.setUsage( GL_DYNAMIC_DRAW );

    connection_between_pivot.setMode( OF_PRIMITIVE_LINES );
    connection_between_pivot.setUsage( GL_DYNAMIC_DRAW );

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

void ofApp::update(){
	
	noise.setFrame( ofGetFrameNum() * 0.03 );
	noise.update();
	
    float step = ofGetFrameNum() - current_setting_start_frame;
    float step_angle = (out_angle-in_angle) / sequencial_add_speed;
    int w = 0;
    int h = 0;
    
    float now_angle = in_angle + step_angle*step;
    if( now_angle > out_angle ){
        change_settings();
    }
    
    //if( la.size() < num_agent){
        LineAgent l;
        if( bSequencial_add){
            l.setup( now_angle, initial_radius );
        }else{
            l.setup( ofRandom(in_angle, out_angle), initial_radius );
        }
        la.push_back( l );
    //}
    
    if( bStart ){
        
//        if( step > 100)
//            change_settings();
        
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
//                la[i].animate_noise( i );
            }
        }
        
        if( bDraw_connection_between_agnet ){
            connection_between_agent.clear();
            connection_between_pivot.clear();

                int num_line = 2000;
            
                for( int i=0; i<num_line; i++ ){
                
                int agent1 = ofRandom(0, la.size() );
                int agent2 = ofRandom(0, la.size() );
                
                int nv1 = la[agent1].trail.getNumVertices();
                int nv2 = la[agent2].trail.getNumVertices();
                
                if( nv1==0 || nv2==0 ) break;
                
                int index1 = ofRandom( 0, nv1 );
                int index2 = ofRandom( 0, nv2 );
                
                ofVec3f p1 = la[agent1].trail.getVertex( index1 );
                ofVec3f p2 = la[agent2].trail.getVertex( index2 );
                
                float d = p1.distance( p2 );
                if( 100<d && d<150 ){
                    connection_between_agent.addVertex( p1 );
                    connection_between_agent.addVertex( p2 );
                    connection_between_agent.addColor( la[agent1].trail.getColor( index1 ) );
                    connection_between_agent.addColor( la[agent2].trail.getColor( index2 ) );
                
                    if( ofRandom(1.0) > 0.9 ){
                        // pivot
                        connection_between_pivot.addVertex( center );
                        connection_between_pivot.addVertex( p1 );
                        connection_between_pivot.addColor( la[agent1].trail.getColor( index1 ) );
                        connection_between_pivot.addColor( la[agent2].trail.getColor( index2 ) );
                    }
                }
            }
            
            vector<ofVec3f> test_point;
            test_point.push_back( center );
            for( int i=0; i<la.size(); i++ ){
                for( int j=0; j<la[i].trail.getNumVertices(); j++ ){
                    if( ofRandom( 1.0) > 0.92 )
                        test_point.push_back( la[i].trail.getVertex( j ) );
                }
            }
            
            for( int i=0; i<test_point.size(); i++ ){
                ofVec3f &p1 = test_point[i];

                int num_line = ofRandom(1, 5);
                
                multimap<float, ofVec3f> near_p;
                pair<float, ofVec3f> pair1( 999999999999, ofVec3f(0,0,0) );
                for( int line=0; line<num_line; line++ ){
                    near_p.insert( pair1 );
                }
                
                for( int j=0; j<test_point.size(); j++ ){
                    if( i == j )
                        continue;
                    ofVec3f &p2 = test_point[j];
                    float dist = p1.distance( p2 );
                    multimap<float, ofVec3f>::iterator itr = near_p.end();
                    itr--;
                    if( dist < itr->first ){
                        std::pair<float, ofVec3f> pair2( dist, p2 );
                        near_p.insert( pair2 );
                        
                        multimap<float, ofVec3f>::iterator end = near_p.end();
                        near_p.erase( --end );
                    }
                }
                
                multimap<float, ofVec3f>::iterator itr = near_p.begin();

                for(; itr!=near_p.end(); itr++ ){
//                    if( i==0  && ofRandom(1.0)>0.9 ){
//                        ofVec3f dir = p2 - p1;
//                        p1 = dir.normalize() * 200;
//                    }

                    ofVec3f p2 = itr->second;
                    connection_between_agent.addVertex( p1 );
                    connection_between_agent.addVertex( p2 );
                    connection_between_agent.addColor( ofFloatColor( ofRandom(0.2,0.4), ofRandom(0.2, 0.4) ) );
                    connection_between_agent.addColor( ofFloatColor( ofRandom(0.2,0.4), ofRandom(0.2, 0.4) ) );
                }
            }
        }
    }
}

void ofApp::draw(){
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
	ofBackground( 255 );
    ofSetColor( 255 );
    if( bOrtho ){
        ofSetupScreenOrtho();
    }
    
    ofPushMatrix();
    ofTranslate( ofGetWidth()/2, ofGetHeight()-100 );
    if( bRotate ){
        ofRotate( ofGetFrameNum()*0.1, 1, 0, 0 );
    }

    if( bDraw_connection_between_agnet ){
        glLineWidth( 1 );
        connection_between_agent.setMode( OF_PRIMITIVE_LINES );
        connection_between_agent.draw();

        glPointSize( 5 );
        connection_between_agent.setMode( OF_PRIMITIVE_POINTS );
        connection_between_agent.drawVertices();

        glLineWidth( 1 );
        connection_between_pivot.draw();
    }    

    glPointSize( 2 );
    for ( int i=0; i<la.size(); i++ ) {
        if( bDraw_agent ){
            la[i].draw();
        }
        if( bDraw_connection_inside_of_agent ){
            la[i].draw_connection_inside_of_agent();
        }
	}
    
//    draw_vector_graphics();
    
    ofPopMatrix();
    
    saver.save();
    
    draw_info();

	noise.draw(5, 5, 0.5 );
}

void ofApp::draw_vector_graphics(){
    
    if( la.size() < 300 )
        return;
    
    ofSetColor( 5, 255 );
    ofNoFill();
    
    int num_line = 3;
    
    for( int i=0; i<num_line; i++ ){
        int s_la = i * 100;
        int c1_la = ofRandom( 0, 100 );
        int c2_la = ofRandom( 0, 100 );
        
        int s_max = la[s_la].trail.getNumVertices();
        int c1_max = la[c1_la].trail.getNumVertices();
        int c2_max = la[c2_la].trail.getNumVertices();
        if( s_max>0 && c1_max>0 && c2_max>0 ) {
            int s_ind = 0;
            int c1_ind = 0;
            int c2_ind = 0;
            
            ofVec3f s = la[s_la].trail.getVertex( s_ind );
            ofVec3f c1 = la[c1_la].trail.getVertex( c1_ind );
            ofVec3f c2 = la[c2_la].trail.getVertex( c2_ind );
            ofVec3f end = center;

            ofxVectorGraphics vg;
            vg.beginShape();
            vg.bezier(s.x, s.y, c1.x, c1.y, c2.x, c2.y, end.x, end.y );
            vg.endShape();
        }
    }
}

void ofApp::change_settings(){

    initial_radius = ofRandom(100, 500) + 400;
    sequencial_add_speed = ofRandom( 200, 300 );
    current_setting_start_frame = ofGetFrameNum();
    
    center.set( 0, 0 );

    if( ofRandom(1.0)>0.5 ) {
        in_angle = -20;
        out_angle = -160;
    }else{
        in_angle = -160;
        out_angle = -20;
    }
    
    bSequencial_add = 1;
}

void ofApp::draw_info(){
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

void ofApp::keyPressed( int key ){

    switch( key ) {
		case ' ':
            bStart = !bStart;
            break;
    
        case 'f':
            ofToggleFullscreen();
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
