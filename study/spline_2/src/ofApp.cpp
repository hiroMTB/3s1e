#include "ofApp.h"
#include "ad_util.h"

//#define RENDER 1

#define ANTI_SWING 1

void ofApp::setup_export_layer( int w, int h, int num ){
    
    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    exporter.setup(w, h, 25, GL_RGB, 4 );
    exporter.setFilePattern(  dir_name + "/F_%05i.png");
    exporter.setFrameRange( 1, 751 );
    exporter.setAutoExit( true );
    
#ifdef RENDER
    ofSetWindowShape(100, 100);
    ofSetWindowPosition(0, 0);
    exporter.startExport();
#else
    ofSetWindowShape(w, h);
    ofSetWindowPosition(0, 0);
#endif

}

void ofApp::setup(){
    
    bDebugDraw = false;
    
    ofSeedRandom( 12345 );
    
#ifdef RENDER
    scale = 600.0;
#else
    scale = 300.0;
#endif
    
    img.loadImage( ad_util::data_path + "img/rk/Mx80_2_org.jpg" );
    
    
    for (int i=0; i<10; i++) {
    
#pragma mark WHIP
        whips.push_back( Whip() );
        Whip & w = whips[whips.size()-1];
        w.freq = 0.032 + ofRandomf()*0.01;
        w.amp = 1.0 + ofRandomf()*0.1;

        w.length = ofNoise(3, i*0.011)* 0.03 + 0.03f;
        
        RopeSimulation * rs = new RopeSimulation(
                                 100,						// 80 Particles (Masses)
                                 0.05,					// Each Particle Has A Weight Of 50 Grams
                                 20000.0f,				// springConstant In The Rope
                                 w.length,                      // Normal Length Of Springs In The Rope
                                 0.1,					// Spring Inner Friction Constant
                                 Vector3D(0, -9.81f, 0), // Gravitational Acceleration
                                 0.001f,					// Air Friction Constant
                                 0.0f,					// Ground Repel Constant
                                 0,					// Ground Slide Friction Constant
                                 0,					// Ground Absoption Constant
                                 -1111.5f,					// Height Of Ground
                                 ofSignedNoise(1, i*0.1)*HALF_PI// - HALF_PI/2
                                );
                                        
                                        
        ropesim.push_back( rs );
        
        
        ofxSimpleSpline * ss = new ofxSimpleSpline();
        vector<ofVec3f> * cv = new vector<ofVec3f>();
        cv->resize(5 + 1);
        ss->setSubdivisions( 10 );
        ss->setControlVertices( *cv );

        curve.push_back( ss );
        cvs.push_back( cv );
        
        vector<ofFloatColor> c;
        cols.push_back( c );
        
        list<ofxSimpleSpline> sps;
        curve_history.push_back(sps);
    }

#ifdef RENDER
    win.x = 1920*2;
    win.y = 1080*2;
#else
    win.x = 1920;
    win.y = 1080;
#endif
    setup_export_layer( win.x, win.y, 1);
    
}

void ofApp::update(){

    // rope sim
    float dt = 0.006;
    float maxPossible_dt = 0.002f;

    // This Is Needed To Prevent Pass Over Of A Non-Precise dt Value
    int numOfIterations = (int)(dt / maxPossible_dt) + 1;					// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt
    if (numOfIterations != 0)												// Avoid Division By Zero
        dt = dt / numOfIterations;											// dt Should Be Updated According To numOfIterations
    
    for (int i=0; i<ropesim.size(); i++) {

        Vector3D swing;

        const Whip & w = whips[i];
        
        swing.x = sin( ofGetFrameNum() * w.freq) * w.amp;

        ropesim[i]->setRopeConnectionVel( swing );
        
        for (int a = 0; a < numOfIterations; ++a)
            ropesim[i]->operate(dt);
    
        // add result
        vector<ofVec3f> * cv = curve[i]->controlVertices;
        int id = 0;
        for (int a = 0; a<ropesim[i]->numOfMasses; ++a){
            if( a%20 == 0 || a ==ropesim[i]->numOfMasses-1){
                
                Mass* mass1 = ropesim[i]->getMass(a);
                Vector3D* pos1 = &mass1->pos;
                ofVec3f p(pos1->x, pos1->y, 0); //pos1->z);
                p *= scale;
                if( ofGetFrameNum() == 0 ){
                    cv->at(id) = p;
                }else{
                    ofVec3f newp = cv->at(id)*0.9 + p*0.1;
                    cv->at(id) = newp;
                }
                id++;

            }
        }
        
#pragma mark IMG_SETTING
        // img
        //          w       : 20000 px
        //          h       : 1920  px
        //          total   : 38,400,000 px
        // curve
        //          num curves : 100 - 10,000 splines ?
        //          Fbo num Vertices : 500v (100 control x 5 sub)
        //
        // total    : num curve * 500
        // range    : 50000 - 5,000,000 px
        int nv = curve[i]->lineVbo.getNumVertices();
        
        float start_a = 0.1;
        float max_a = 0.4 + 0.8/curve.size();
        float step_a = (max_a-start_a) / nv;
        cols[i].clear();
        
        int index = i*nv + 10000;
        int x = index*100 % (int)img.getWidth();
        int y = index / (int)img.getHeight();
        ofFloatColor c = img.getColor( x, y );
        c = ofFloatColor(1) - c;
        
        if( ofNoise(3,i*0.5) > 0.92){
            start_a += 0.4;
            c.r += 0.3;
        }

        if( ofNoise(2,i*0.5) > 0.92){
            start_a += 0.6;
        }
        
        for (int j=0; j<nv; j++) {
            x %= (int)img.getWidth();
            y %= (int)img.getHeight();
            c.a = start_a + j*step_a;
            cols[i].push_back( c );
        }
        
        curve[i]->setControlVertices( *cvs[i] );
        curve[i]->lineVbo.setColorData(&cols[i][0], cols[i].size(), GL_DYNAMIC_DRAW );
        curve[i]->update();

    }

    
    // History
    hisnum = 3;
    if( ofGetFrameNum() % 5 == 0 ){
        for (int i=0; i<curve.size(); i++) {
            curve_history[i].push_back( *curve[i] );
            
            int nh = curve_history[i].size();
            if( nh>=hisnum ) {
                curve_history[i].pop_front();
            }
        }
    }
    
}

void ofApp::draw(){

    exporter.begin(); {
    
        ofEnableAlphaBlending();
        
        float phase = 0;
        float angle = 0;
        
        ofBackground(255);
        ofTranslate( 100, win.y/2 - phase/2 );
        ofRotateZ(90);
        
        glLineWidth(1);
        glPointSize(1);
        
        
#pragma mark ORIGINAL_WHIP
        {
            ofEnableAntiAliasing();
            // Start Drawing The Rope.
            if( bDebugDraw ){
                for (int i=0; i<ropesim.size(); i++) {
                    
                    Mass* mass1 = ropesim[i]->getMass(0);
                    Vector3D* pos1 = &mass1->pos;
                    
                    ofPushMatrix();
                    ofTranslate(phase/ropesim.size()*i, 0 );
#ifdef ANTI_SWING
                    ofVec3f swing(pos1->x, pos1->y, pos1->z);
                    swing *= scale;
                    ofTranslate( -swing );
#endif
                    glLineWidth(1);
                    glBegin(GL_LINE_STRIP);
                    for (int a = 0; a < ropesim[i]->numOfMasses*0.9; ++a){
                        Mass* mass1 = ropesim[i]->getMass(a);
                        Vector3D* pos1 = &mass1->pos;
                        glColor4f(0, 0, 0.1, 0.001 + a*0.0005);
                        glVertex3f(pos1->x*scale, pos1->y*scale, pos1->z*scale);
                    }
                    glEnd();
                    
                    ofPopMatrix();
                }
            }
        }
        
#pragma mark PREP_LINE
        if( 0 ){
            float start_a = 30;
            float max_a = 70;
            
            ofDisableAntiAliasing();
            for (int i=0; i<curve.size(); i++) {
                
                ofVec3f swing = curve[i]->controlVertices->at(0);
                ofPushMatrix();
                ofTranslate(phase/curve.size()*i, 0 );
                ofRotateZ( i*angle/curve.size() - angle*0.5);
                
                int num_cv = curve[i]->controlVertices->size();
                for (int j=1; j<num_cv; j++) {
                    float step_a = (max_a-start_a) / curve[i]->controlVertices->size();

                    ofVec3f p1 = curve[i]->controlVertices->at( j );
#ifdef ANTI_SWING
                    p1 -= swing;
#endif
                    ofVec3f p2(0, p1.y, 0);
                    ofSetColor(10,  start_a + j*step_a);
                    ofSetLineWidth(1);
                    ofLine(p1, p2);
                }
                ofPopMatrix();
            }
        }
        
        
#pragma mark SPLINE_NOW
        {
            ofEnableAntiAliasing();
            ofSetLineWidth(1);
            
            int draw_curve_num = ofGetFrameNum()*4;
            
            for (int i=0; i<curve.size(); i++) {
                
                if( i>= draw_curve_num )
                    break;
                
                ofPushMatrix();
                ofTranslate(phase/curve.size()*i, 0 );
                int angleId = ofNoise(9, i*0.5)*curve.size();
                ofRotateZ( angleId*angle/curve.size() - angle*0.5);
                
#ifdef ANTI_SWING
                ofVec3f swing = curve[i]->controlVertices->at(0);
                ofTranslate(-swing);
#endif
                int nv = curve[i]->lineVbo.getNumVertices();
                
                int vCount = draw_curve_num - i;
                if( vCount > 0){
                    curve[i]->draw( GL_LINE_STRIP, MIN(vCount, nv) );
                }
                ofPopMatrix();
            }
        }
        
#pragma mark HISTORY
        if( 0 ){
            ofDisableAntiAliasing();
            glPointSize(1);
            for (int i=0; i<curve_history.size(); i++) {
                ofPushMatrix();
                ofTranslate(phase/curve_history.size(), 0 );
                list<ofxSimpleSpline>::iterator itr = curve_history[i].begin();
                for (; itr!=curve_history[i].end(); itr++) {
                    ofPushMatrix();
                    
                    int angleId = ofNoise(9, i*0.5)*curve.size();
                    ofRotateZ( angleId*angle/curve.size() - angle*0.5);

#ifdef ANTI_SWING
                    ofVec3f swing = itr->controlVertices->at(0);
                    ofTranslate( -swing );
#endif
                    itr->draw( GL_POINTS );
                    ofPopMatrix();
                }
                ofPopMatrix();
            }
        }

        
#pragma mark CP_POINTS
        if( 0 ){
            ofDisableAntiAliasing();
            float start_a = 50;
            float max_a = 150;
            for (int i=0; i<curve.size(); i++) {
                float step_a = (max_a-start_a) / curve[i]->controlVertices->size();
                ofPushMatrix();
                ofTranslate(phase/curve.size()*i, 0 );
                
                int angleId = ofNoise(9, i*0.5)*curve.size();
                ofRotateZ( angleId*angle/curve.size() - angle*0.5);

#ifdef ANTI_SWING
                ofVec3f swing = curve[i]->controlVertices->at(0);
                ofTranslate(-swing);
#endif
                
                glPointSize(2);
                for (int j=1; j<cvs[i]->size(); j++) {
                    ofSetColor(0, start_a + j*step_a );
                    ofVec3f &p = cvs[i]->at(j);
                    glBegin(GL_POINTS);
                    glVertex3f( p.x, p.y, p.z );
                    glEnd();
                    //ofCircle( cvs[i]->at(j), 1);
                }
                ofPopMatrix();
            }
        }

    } exporter.end();

#ifndef RENDER
    ofPushMatrix(); {
        ofClear(0);
        ofBackground(255);
        ofSetColor(255);
        exporter.draw(0, 0);

        ofSetColor(50);
        ofDrawBitmapString( ad_util::getFrameInfoString(), 20, 20 );
    } ofPopMatrix();
#endif
    
}

void ofApp::keyPressed(int key){
    
#ifndef RENDER
    switch (key) {
        case 'd':
            bDebugDraw = !bDebugDraw;
            break;
            
        default:
            break;
    }
#endif
}


