//
//  Branch.cpp
//
//  Created by hiroshi matoba on 1/3/15.
//
//

#include "Branch.h"
#include "ofApp.h"

int Branch::top_depth = -1;
ofVec3f Branch::root = ofVec3f(0,0,0);

void Branch::craete(Branch *_parent, ofVec3f _dirn, float _sp_len, float _sp_angle, float _fw_len, int _b_type, int _f_type ){

    bDrawShapeAsPoint = false;
    parent = _parent;
    dirn = _dirn;
    sp_len = _sp_len;
    sp_angle = _sp_angle;
    fw_len = _fw_len;
    b_type = _b_type;
    f_type = _f_type;
    
    if( parent == NULL ){
        depth = 0;
    }else{
        depth = parent->depth+1;
    }

    top_depth = MAX(top_depth, depth);
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );

    calc_posision();
    calc_shape();

    //cout << "create branch\n ST: " << st.x << ", " << st.y << "\nEND: " << end.x << ", " << end.y << endl;
}

void Branch::create_child(){

    ofRectangle canvas;
    canvas.set(0, 0, ofApp::app->territoryFbo.getWidth(), ofApp::app->territoryFbo.getHeight() );
    
    if( canvas.inside( end.x, end.y) == false ){
        return;
    }
    
    if(children.size() == 0 ){
        
        if( depth != top_depth && ofRandomuf() > 0.3 )
            return;
        
        if( depth < top_depth*0.9)
            return;
        
        int n = ofRandom(0, 5);
        float fw_len = ofRandom( 5, 10 );
        float sp_len = ofRandom( 5, 10);

        if( ofRandomuf()>0.95)
            fw_len *= 10;

        if( ofRandomuf()>0.98)
            sp_len *= 3.0;

        if( ofRandomuf()>0.98){
            sp_angle = 90;
            sp_len *= 20;
        }
        
        for (int i=0; i<n; i++) {
            float sp_angle = ofRandom(70, 90);

            Branch child;
            children.push_back( child );
            int id = children.size() -1;
            
            if( ofRandomuf()>0.5) sp_angle *= -1.0;
            
            children[id].craete(this, dirn, sp_len, sp_angle, fw_len, 0, 0 );
        }
    }else{
        for (int i=0; i<children.size(); i++) {
            children[i].create_child();
        }
    }
}

void Branch::calc_posision(){
    
    if( parent == NULL ){
        st = root;
        anchor = st + dirn.rotated(sp_angle, ofVec3f(0,0,1) );
        end = anchor + dirn * fw_len;
    }else{
        st = parent->end;
        anchor = st + dirn.rotated(sp_angle, ofVec3f(0,0,1) ) * sp_len;
        end = anchor + dirn * fw_len;
        
        int tryn = 0;
        ofRectangle canvas;
        canvas.set(0, 0, ofApp::app->territoryFbo.getWidth(), ofApp::app->territoryFbo.getHeight() );

        if( canvas.inside( end.x, end.y) ){
            
            while ( ofApp::app->territoryPix.getColor(end.x, end.y).a > 120 ) {
    
                sp_angle = ofRandom(-80, 80);
                sp_len = ofRandom(10, 20) + tryn;
                anchor = st + dirn.rotated(sp_angle, ofVec3f(0,0,1) ) * sp_len;
                end = anchor + dirn * fw_len;
                
                tryn++;
                if (tryn>10) {
                    sp_angle = 90;
                    sp_angle *= ofRandomuf()>0.5 ? 1 : -1;

                    if (ofRandomuf()>0.5) {
                        fw_len = ofRandom(50, 100);
                    }else{
                        sp_len = ofRandom(50, 100);
                    }

                    anchor = st + dirn.rotated(sp_angle, ofVec3f(0,0,1) ) * sp_len;
                    end = anchor + dirn * fw_len;
                    //break;
                }
                
                if( canvas.inside( end.x, end.y) == false )
                    break;
                
                if( tryn > 30)
                    break;
                
                cout << "retake " << tryn << endl;
            }
        }
    }
    
    ofApp::app->territoryFbo.begin();
    ofSetColor(0, 0, 0, 150);
    ofVec3f dir = end - st;
    dir.x *= 1.5;
    dir.y *= 1.5;
    ofRect( ofRectangle(ofRectangle(st, st+dir)) );
    ofApp::app->territoryFbo.end();
}

void Branch::calc_shape(){
    
    // clear
    lines.clearColors();
    lines.clearVertices();
    points.clearColors();
    points.clearVertices();
    
    // make sp shape
    
    switch( b_type ){
        
            
        // LINEAR
        case 0:
            
            if( bDrawShapeAsPoint ){
                
            }else{
                ofFloatColor c(0, ofRandom(0.3, 0.8));
                lines.addVertex( st );
                lines.addVertex( anchor );
                lines.addVertex( anchor );
                lines.addVertex( end );
                
                lines.addColor( c );
                lines.addColor( c );
                lines.addColor( c );
                lines.addColor( c );
            }
            
            break;

        case 1:
            
            break;

        case 2:
            
            break;
        
        case 3:
            
            break;
 
        case 4:
            
            break;
            
    }
    
    
}


void Branch::update(){

    if( parent ){
        if( parent->end.x != st.x && parent->end.y != st.y ){
            calc_posision();
            calc_shape();
        }
    }


    for (int i=0; i<children.size(); i++) {
        children[i].update();
    }
}


void Branch::draw(){
    
    if( lines.getNumVertices() != 0 )
        lines.draw();
    
    if( points.getNumVertices() != 0 )
        points.draw();
    
    for (int i=0; i<children.size(); i++) {
        children[i].draw();
    }
}

