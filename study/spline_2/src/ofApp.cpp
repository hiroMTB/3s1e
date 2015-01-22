#include "ofApp.h"
#include "ad_util.h"

//#define RENDER 1
//#define HISTORY
#define CP_POINTS
//#define PREP_LINE
#define ANTI_SWING
//#define ORIGINAL_WHIP

void ofApp::setup_export_layer( int w, int h, int num ){
    
    string dir_name = ofGetTimestampString("%m%d_%H%M_%S");
    
    exporter.setup(w, h, 25, GL_RGB, 4 );
    exporter.setFilePattern(  dir_name + "/F_%05i.png");
    exporter.setFrameRange( 1, 3001 );
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
    
    bDebugDraw = true;
    ofSeedRandom( 12345 );
    
    img.loadImage( ad_util::data_path + "img/rk/Mx80_2_org.jpg" );
    
#ifdef RENDER
    win.x = 1920*2;
    win.y = 1080*2;
    scale = 200.0;
#else
    win.x = 1920;
    win.y = 1080;
    scale = 100.0;
#endif
    
    setup_export_layer( win.x, win.y, 1);
    sim_frame = 0;
    
    for (int i=0; i<30; i++) {
        add_whip();
    }
}

void ofApp::add_whip(){
    
    int i = whips.size();
    whips.push_back( Whip() );
    
    Whip & w = whips[whips.size()-1];
    w.age = 0;
    
    w.freq = 0.033 + ofRandomuf()*0.02;
    w.amp = 0.33 + ofRandomuf()*0.02;
    w.amp *= 5.0;
    
    if( ofRandomuf()>0.91 ){
        w.freq *= ofRandom(1.2, 1.8);
    }
    
    if( ofRandomuf()>0.91 ){
        w.amp *= ofRandom(1.2, 1.8);
    }
    
    w.length = ofNoise(3, i*0.011)* 0.01 + 0.01f;
    
    
    int num_joint = 800;
    curve_resolution = 1;
    
    RopeSimulation * rs = new RopeSimulation(
                                             num_joint,						// 80 Particles (Masses)
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
                                             1//ofSignedNoise(1, i*0.1)*HALF_PI// - HALF_PI/2
                                             );
    
    
    ropesim.push_back( rs );
    
    ofxSimpleSpline * ss = new ofxSimpleSpline();
    vector<ofVec3f> * cv = new vector<ofVec3f>();
    cv->resize(num_joint/curve_resolution + 1);
    ss->setSubdivisions( 5 );
    ss->setControlVertices( *cv );
    
    curve.push_back( ss );
    cvs.push_back( cv );
    
    vector<ofFloatColor> c;
    cols.push_back( c );
    
#ifdef HISTORY
    list<ofxSimpleSpline> sps;
    curve_history.push_back(sps);
#endif
}

void ofApp::update(){

    for (int i=0; i<10; i++) {
        add_whip();
    }
    
    float dt = 0.004;
    float maxPossible_dt = 0.002f;
    int numOfIterations = (int)(dt / maxPossible_dt) + 1;
    if (numOfIterations != 0)
        dt = dt / numOfIterations;

    // rope sim
    for( int h=0; h<2; h++ ){
        
        for (int i=0; i<ropesim.size(); i++) {

            whips[i].age++;
            Vector3D swing;
            const Whip & w = whips[i];
            swing.x = sin( (float)whips[i].age * w.freq) * w.amp;
            
            ropesim[i]->setRopeConnectionPos( swing );
            
            for (int a=0; a<numOfIterations; ++a)
                ropesim[i]->operate(dt);
        }
    }
    
    for (int i=0; i<ropesim.size(); i++) {
    
        // add result
        vector<ofVec3f> * cv = curve[i]->controlVertices;
        int id = 0;
        for (int a=0; a<ropesim[i]->numOfMasses; ++a){

            
            if( a%curve_resolution == 0 || a ==ropesim[i]->numOfMasses-1){
                
                Mass* mass1 = ropesim[i]->getMass(a);
                Vector3D* pos1 = &mass1->pos;
                ofVec3f p(pos1->x, pos1->y, pos1->z);
                p *= scale;
                if( whips[i].age <= 2 ){
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
        
        float start_a = 0.3;
        float max_a = 0.7;
        float step_a = (max_a-start_a) / nv;
        cols[i].clear();
        
        int index = i*nv + 10000;
        int x = index*100 % (int)img.getWidth();
        int y = index / (int)img.getHeight();
        ofFloatColor c = img.getColor( x, y );
        //c = ofFloatColor(1) - c;
        
        if( ofNoise(3,i*0.5) > 0.92){
            start_a = ofRandom(0.7, 0.8);
            c.r += 0.5;
        }

        if( ofNoise(2,i*0.5) > 0.92){
            start_a = 0.9;
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

#ifdef HISTORY
    hisnum = 1;
    if( ofGetFrameNum() % 10 == 0 ){
        for (int i=0; i<curve.size(); i++) {
            curve_history[i].push_back( *curve[i] );
            
            int nh = curve_history[i].size();
            if( nh>=hisnum ) {
                curve_history[i].pop_front();
            }
        }
    }
#endif
    
}

void ofApp::draw(){

    exporter.begin(); {
    
        ofEnableAlphaBlending();
        
        float phase = 1;
        float angle = 0;
        
        ofBackground(255);
        ofTranslate( 100, win.y/2 );
        ofRotateZ( 90);
        
        glLineWidth(1);
        glPointSize(1);
        
        
#ifdef ORIGINAL_WHIP
        {
            ofEnableAntiAliasing();
            
            int draw_curve_num = ofGetFrameNum();
            
            if( bDebugDraw ){
                for (int i=0; i<ropesim.size(); i++) {
                    
                    if( i>draw_curve_num )
                        break;

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
                    for (int a=0; a<ropesim[i]->numOfMasses*0.8; a++){
                        Mass* mass1 = ropesim[i]->getMass(a);
                        Vector3D* pos1 = &mass1->pos;
                        glColor4f(0, 0, 0, 0.01 + a*0.001);
                        glVertex3f(pos1->x*scale, pos1->y*scale, pos1->z*scale);
                    }
                    glEnd();
                    
                    ofPopMatrix();
                }
            }
        }
#endif
        
        if( 1 ){
            ofEnableAntiAliasing();
            ofSetLineWidth(1);
            
            int draw_curve_num = ofGetFrameNum();
            
            for (int i=0; i<curve.size(); i++) {
                
                if( i>draw_curve_num )
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
                
                int vCount = 10 + ofGetFrameNum(); ///curve_resolution;
                if( vCount > 0){
					curve[i]->draw();
                }
                ofPopMatrix();
            }
        }
        
#ifdef PREP_LINE
        if( 1 ){
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
                    const ofFloatColor &c = cols[i][j];
                    ofSetColor(c, start_a + j*step_a);
                    ofSetLineWidth(1);
                    ofLine(p1, p2);
                }
                ofPopMatrix();
            }
        }
#endif

#ifdef HISTORY
        if( 1 ){
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
#endif
        
        
#ifdef CP_POINTS
        if( 1 ){
            ofDisableAntiAliasing();
            float start_a = 0;
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
                
                for (int j=1; j<cvs[i]->size(); j++) {
                
                    int vCount = ofGetFrameNum();; // / curve_resolution;
                    if( j < vCount ){
                        
                        const ofFloatColor &c = cols[i][j];
                        ofSetColor(ofFloatColor(1)-c, start_a + j*step_a );
                        ofVec3f &p = cvs[i]->at(j);
                        glPointSize(2);
                        glBegin(GL_POINTS);
                        glVertex3f( p.x, p.y, p.z );
                        glEnd();

                        if( ofRandomuf()>0.3){
                            ofSetColor( c, 0.8 );
                            ofVec3f bigp = p * 1.25;
                            glPointSize(3);
                            glBegin(GL_POINTS);
                            glVertex3f( bigp.x, bigp.y, bigp.z );
                            glEnd();
                        }
                    }
                }
                ofPopMatrix();
            }
        }
#endif
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


