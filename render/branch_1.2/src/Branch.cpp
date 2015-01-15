//
//  Branch.cpp
//
//  Created by hiroshi matoba on 1/3/15.
//
//

#include "Branch.h"
#include "ofApp.h"
#include "ofxTransitions.h"

int Branch::top_depth = 0;
int Branch::total_bnum = 0;
int Branch::active_total= 0;
float Branch::dist_limit = 100;

ofVec3f Branch::root = ofVec3f(0,0,0);

void Branch::craete(Branch *_parent, ofVec3f _dirn ){
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    bDrawShapeAsPoint = false;
    bMainBranch = false;
    parent = _parent;
    dirn = _dirn;
    depth = 0;
    
    b_type = round( ofRandomuf() * 19);
    
    if( parent ){
        depth = parent->depth+1;
    }else{
        depth = 0;
    }
    
    bid = total_bnum;
    
    // static param
    top_depth = MAX(top_depth, depth);
    total_bnum++;
    
    color.set(1);
    int w = ofApp::app->img.getWidth();
    int h = ofApp::app->img.getHeight();
    int x = bid%w;
    int y = bid/w;
    color = ofApp::app->img.getColor( x, y );
    
    if( ofRandomuf()>0.99 ){
        color.set(0.8,0,0.5 );
    }
    
    if( ofRandomuf()>0.99 ){
        color.set(1,1);
    }

    if( ofRandomuf()>0.8 ){
        color.set(0,1);
    }

    color.a = 0.1 + ofRandomuf()*0.8;

    
    set_param();
//    int trial = 0;
//    while ( ++trial<10 && !check_territory() ) {
//        set_param();
//    }
    
    calc_posision();
    calc_shape();

//    ofApp::app->territoryFbo.begin();
//    ofSetColor(0, 0, 0, 80);
//    ofCircle( end, 30 );
//    ofRect( ofRectangle( st, end) );
//    ofApp::app->territoryFbo.end();
    
    if( bMainBranch )
        active_total++;
}

void Branch::set_param(){
    
    switch (b_type) {
        case 0:
            sp_angle = ofRandom(0, 90);
            sp_len = ofRandom( 10, 20 );
            fw_len = ofRandom( 10, 20 );
            break;
            
        case 1:
            sp_angle = 90;
            sp_len = ofRandom( 10, 20 );
            fw_len = ofRandom( 5, 8 );
            break;
            
        case 2:
        case 3:
        case 4:
            sp_angle = ofRandom(15, 50);
            sp_len = ofRandom( 15, 40 );
            fw_len = ofRandom( 15, 40);
            break;
            
        // Box A
        case 5:
            fw_len = ofRandom( 10, 20);
            sp_len = ofRandom( 10, 30);
            sp_angle = ofRandom(10, 90);
            break;
            
        // Reg Box
        case 6:
            fw_len = ofRandom( 10, 20);
            sp_len = ofRandom( 10, 40);
            sp_angle = ofRandom(10, 90);
            break;

        case 9:
            fw_len = ofRandom( 10,50);
            sp_len = ofRandom( 10, 40);
            sp_angle = ofRandom(10, 90);
            break;
            
            
        case 10:
        case 11:
        case 12:
        case 13:
            sp_angle = ofRandom(0,90);
            sp_len = ofRandom( 15, 25 );
            fw_len = ofRandom( 10, 20 );
            break;
            
        default:
            sp_angle = 90;
            sp_len = ofRandom( 15, 25 );
            fw_len = ofRandom( 10, 20 );
            break;
    }
    
    // adjust
    float anim_rate = (float)ofGetFrameNum()/ofApp::app->max_frame;
    anim_rate += 0.6;
    sp_len *= MIN( 3, ofRandom(1, 2) * anim_rate);
    fw_len *= MIN( 3, ofRandom(1, 2) * anim_rate);

    
    // X jump
    if( ofRandomuf()>0.995 ){
        if( b_type != 6 && b_type != 9 && b_type != 7 && b_type != 8 ){
            fw_len *= ofRandom(2, 8);

            if( ofRandomuf() > 0.1 ){
                bMainBranch = true;
            }
            
            if( ofRandomuf()>0.9 ){
                fw_len *= ofRandom(3, 6);
                sp_len *= 2;
            }
        }
    }
    // Y jump
    if( ofRandomuf()>0.98){
        sp_len *= ofRandom(3, 10);

        float r = ofRandomuf();
        if( r<0.1 ){
            sp_angle = ofRandom(45, 90);
        }else if( r<0.35 ){
            sp_angle = 45;
            float r2 = ofRandomuf();
            if( r2>0.3 ){
                b_type = floor(ofRandom(2,5));
            }
            
        }else if( r<0.8 ){
            sp_angle = 90;
        }else{
            sp_angle = 135;
            sp_len *= 0.9;
        }
        
        if( ofRandomuf() > 0.2 ){
            bMainBranch = true;
        }
    }
    
    if( parent==NULL ){
//        sp_angle -= 44;
        sp_len += ofRandom(20,50);
        fw_len += ofRandom(20,50);
//        b_type = 0;
    }
    
    sp_angle *= ofRandomuf()>0.5 ? 1: -1;
}

bool Branch::check_territory(){
    return ofApp::app->territoryPix.getColor(end.x, end.y).a < 100;
}

void Branch::create_child(){

    //if( root.distance( end ) > dist_limit )
      //  return;
    
    bool top = depth == top_depth;
    
    if ( top ) {
        bMainBranch = true;
    }
    
    if( bMainBranch ){
        
        if( top )
            if( ofRandomuf() > 0.5 ) return;
        
        Branch child;
        child.craete(this, dirn );
        children.push_back( child );
    
        // inheirt main branch
        int son = ofNoise(bid)*10 + 3;
        
        if( bMainBranch && children.size() > son ){
            int id = ofRandom(0, children.size()-1);
            children[id].bMainBranch = true;
            bMainBranch = false;
            return;
        }
    }else{
        for (int i=0; i<children.size(); i++) {
            children[i].create_child();
        }
    }
}

void Branch::update(){
    
//    if( bMainBranch)
//        active_total++;
//    
//    if( active_total > 12 ){
//        bMainBranch = false;
//    }
//    
//    //if( parent ){
//        //calc_posision();
//        calc_shape();
//    //}
//    
//    for (int i=0; i<children.size(); i++) {
//        children[i].update();
//    }
}

void Branch::calc_posision(){
    
    if( parent ){
        st = parent->end;
    }else{
        st = root;
    }
    
    anchor = st + dirn.rotated(sp_angle, ofVec3f(0,0,1) ) * sp_len;
    end = anchor + dirn * fw_len;
}

void Branch::calc_shape(){
    
    ofFloatColor c = color;
    if (bMainBranch) {
        c.set( ofRandomuf()*0.3+0.7, 0, 0.2 );
        c.a = ofRandomuf()*0.3 + 0.5;
    }
    
    // clear
    lines.clearColors();
    lines.clearVertices();
    points.clearColors();
    points.clearVertices();
    
    // make sp shape
    switch( b_type ){
        
        // LINEAR
        case 0:
        case 1:
        {
            int num_line = 1;
            if( ofNoise(bid*0.4) > 0.6 ){
                num_line += ofNoise(bid*0.4)*10;
            }
            
            float adder = ofNoise(bid*3.5) * 3 + 1;
            for (int k=0; k<num_line; k++) {
                ofVec2f offset(k*adder, 0);
                lines.addVertex( st + offset);
                lines.addVertex( anchor + offset );
                lines.addVertex( anchor + offset );
                lines.addVertex( end + offset );
                
                lines.addColor( c );
                lines.addColor( c );
                lines.addColor( c );
                lines.addColor( c );
            }
        }
            break;

        // SplineA
        case 2:
        {
            ofxTransitions tr;
            ofVec3f dir = anchor - st;
            float width = dir.x;
            float height = dir.y;

            int num_line = 1;
            if( ofNoise(bid)>0.85)
                num_line = ofNoise(bid)*7 + 1;
            
            int step = width * 0.5;
            for (int k=0; k<num_line; k++) {
             
                float offsetx = k*4;
                ofVec3f current = st;
                
                for (int i=0; i<step; i++) {
                    float x = width * ((float)i/step) + offsetx;
                    float y = tr.easeInOutCubic(i, 0, height, step);
                    lines.addVertex( current );
                    current = st + ofVec3f(x, y, 0);
                    lines.addVertex( current );
                    lines.addColor( c );
                    lines.addColor( c );
                }
                
                lines.addVertex( current );
                lines.addVertex( anchor );

            }

            lines.addColor( c );
            lines.addColor( c );
            
            lines.addVertex( anchor );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
            break;

        // SplineB
        case 3:
        {
            ofxTransitions tr;
            ofVec3f dir = anchor - st;
            float width = dir.x;
            float height = dir.y;
            
            int num_line = 1;
            if( ofNoise(bid*0.4)>0.8)
                num_line = ofNoise(bid)*7 + 1;

            int step = width * 0.5;

            for (int k=0; k<num_line; k++) {
            
                float offsetx = k*4;
                ofVec3f current = st;
                
                for (int i=0; i<step; i++) {
                    float x = width * ((float)i/step) + offsetx;
                    float y = tr.easeInOutSine(i, 0, height, step);
                    lines.addVertex( current );
                    current = st + ofVec3f(x, y, 0);
                    lines.addVertex( current );
                    lines.addColor( c );
                    lines.addColor( c );
                }
                lines.addVertex( current );
                lines.addVertex( anchor );
                lines.addColor( c );
                lines.addColor( c );
            }
            
            lines.addVertex( anchor );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
            break;
        
        // Spline C
        case 4:
        {
            ofxTransitions tr;
            ofVec3f current = st;
            ofVec3f dir = anchor - st;
            float width = dir.x;
            float height = dir.y;
            int step = width * 0.5;
            for (int i=0; i<step; i++) {
                float x = width * ((float)i/step);
                float y = 0;

                int easeType = round(ofNoise(bid*3) * 7);
                switch (easeType) {
                    case 0: y = tr.easeInOutQuart(i, 0, height, step); break;
                    case 1: y = tr.easeInOutExpo(i, 0, height, step); break;
                    case 2: y = tr.easeInOutSine(i, 0, height, step); break;
                    case 3: y = tr.easeInOutBack(i, 0, height, step); break;
                    case 4: y = tr.easeInOutQuart(i, 0, height, step); break;
                    case 5: y = tr.easeInOutExpo(i, 0, height, step); break;
                    default: y = tr.easeInOutCirc(i, 0, height, step); break;
                }
                lines.addVertex( current );
                current = st + ofVec3f(x, y, 0);
                lines.addVertex( current );
                lines.addColor( c );
                lines.addColor( c );
            }
            
            lines.addVertex( current );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            lines.addVertex( anchor );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
        break;
 
        // Box A, cylinder
        case 5:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            
            float space = dir.length()*0.2;
            float bh = space/2;
            bh = MIN(bh, 6);
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);

            // move a bit
            float speed = ofNoise(bid);
            bend.x -= ofNoise(bid*0.1, speed*0.05)*dir.x*0.3;
            
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            ofVec3f p3 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p4 = bend + ofVec3f(0,-bh/2,0);

            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );

            // L
            lines.addVertex( p1 );
            lines.addVertex( p2 );

            // top
            lines.addVertex( p1 );
            lines.addVertex( p3 );

            // bottom
            lines.addVertex( p2 );
            lines.addVertex( p4 );

            // R
            lines.addVertex( p3 );
            lines.addVertex( p4 );

            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
        
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
            lines.addColor( c );
        }
            
        break;
            
            
        // reg Box
        case 6:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            
            int num = ofNoise(bid*0.03) * 20 + 1;
            for (int i=0; i<num; i++) {
            
                float space = dir.length()*(0.4 * ofRandomf()*0.3);
                float bw = dir.length() - space*2;
                float bh = dir.length()*0.2;
                ofVec3f bst = anchor + ofVec3f(space,0,0);
                ofVec3f bend = end - ofVec3f(space,0,0);
                ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
                ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
                ofVec3f p3 = bend + ofVec3f(0,bh/2,0);
                ofVec3f p4 = bend + ofVec3f(0,-bh/2,0);
                
                // L space
                lines.addVertex( anchor );
                lines.addVertex( bst );
                
                // L
                lines.addVertex( p1 );
                lines.addVertex( p2 );
                
                // top
                lines.addVertex( p1 );
                lines.addVertex( p3 );
                
                // bottom
                lines.addVertex( p2 );
                lines.addVertex( p4 );
                
                // R
                lines.addVertex( p3 );
                lines.addVertex( p4 );
                
                // R space
                lines.addVertex( bend );
                lines.addVertex( end );
             
                for (int k=0; k<6*2; k++) {
                    lines.addColor( c );
                }

            }
        }
        break;
            
        // Triangle <|
        case 7:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );

            ofVec3f dir = end - anchor;
            float space = dir.length()*0.3;
            float bh = dir.length()*0.4 / 1.732 * 2.0;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            ofVec3f p1 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bend + ofVec3f(0,-bh/2,0);
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );

            // /
            lines.addVertex( bst );
            lines.addVertex( p1 );

            //
            lines.addVertex( bst );
            lines.addVertex( p2 );

            // |
            lines.addVertex( p1 );
            lines.addVertex( p2 );

            
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );

            for (int i=0; i<6*2; i++) {
                lines.addColor( c );
            }
        }
            break;
            
        // Triangle rev
        case 8:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            
            ofVec3f dir = end - anchor;
            float space = dir.length()*0.3;
            float bh = dir.length()*0.4 / 1.732 * 2.0;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            
            // |
            lines.addVertex( p1 );
            lines.addVertex( p2 );

            // /
            lines.addVertex( p1 );
            lines.addVertex( bend );
            
            //
            lines.addVertex( p2 );
            lines.addVertex( bend );
            
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            
            for (int i=0; i<6*2; i++) {
                lines.addColor( c );
            }
        }
        break;
            
        // diamond
        case 9:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            float space = dir.length()*0.3;
            float bh = dir.length()*0.25;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            ofVec3f mid = (bend + bst) *0.5;
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            lines.addColor( c );
            lines.addColor( c );

            int step = ofNoise( bid*10) * 6;
            float addery = bh * 0.4;
            float x = 0;
            float y = -addery * (step+1.0)/2.0;
            for (int i=0; i<step; i++) {
                y += addery;
                if( ofNoise(bid)>0.8 ){
                    x += ofSignedNoise( i + bid*0.1) * dir.x*0.15;
                    y += ofSignedNoise(i*2 + bid*0.1) * bh*0.15;
                }
                lines.addVertex( bst );
                lines.addVertex( mid + ofVec2f(x, y) );
                lines.addVertex( mid + ofVec2f(x, y) );
                lines.addVertex( bend );

                lines.addColor( c );
                lines.addColor( c );
                lines.addColor( c );
                lines.addColor( c );
            }
            
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
            
        }
        break;

        // dot
        case 10:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );

            ofVec3f dir = end - anchor;
            float space = dir.length()*0.01;
            float bh = dir.length()*0.2;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);

            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            lines.addColor( c );
            lines.addColor( c );
            
            int step = dir.x * 0.5;
            ofVec3f current = bst;
            ofVec3f adder = dir / step;
            for (int i=0; i<step; i++) {
                current += adder;
                lines.addVertex( current );
                lines.addColor( c );
            }
            
            
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
            break;

    
        // | | |
        case 11:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            float space = dir.length()*0.23;
            float bw = dir.x - space*2;
            float bh = bw*0.12;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            lines.addColor( c );
            lines.addColor( c );
            
            int step = bw * 0.5;
            ofVec3f current = bst;
            float adderx = bw / step;

            for (int i=0; i<step; i++) {
                current.x += adderx;
                float y = ofSignedNoise(i*0.1) * bh/2 * 0.8;
                lines.addVertex( current + ofVec2f(0, y) );
                lines.addVertex( current - ofVec2f(0, y) );
                lines.addColor( c );
                lines.addColor( c );
            }
            
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
        break;
            
        // wave
        case 12:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            float space = dir.length()*0.1;
            float bw = dir.length() - space*2;
            float bh = dir.length()*0.2;
            bh = MIN(bh, 20);
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            lines.addColor( c );
            lines.addColor( c );
            
            int step = bw;
            
            ofVec3f current = bst;
            float adderx = bw / step;
            
            float freq1 = ofNoise(bid*0.8) * ofNoise(bid)*2.0;
            float freq2 = ofNoise(bid*0.2) * ofNoise(bid)*2.0;
            
            if( ofNoise(bid*3) > 0.4){
                for (int i=0; i<step; i++) {
                    float y = (sin(i*freq1) * sin(i*freq2));
                    y = round(y*4) *bh/8;
                    lines.addVertex( current );
                    lines.addVertex( current + ofVec2f(0, y) );
                    lines.addColor( c );
                    lines.addColor( c );
                    current.x += adderx;
                }
            }else{
                float pasty = 0;
                for (int i=0; i<step; i++) {
                    float y = (sin(i*freq1) * sin(i*freq2));
                    y =  round(y*4) *bh/8;

                    lines.addVertex( current + ofVec2f(0, pasty) );
                    lines.addVertex( current + ofVec2f(0, y) );
                    lines.addColor( c );
                    lines.addColor( c );
                    current.x += adderx;
                    pasty = y;
                }
            }
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
        break;

        // switch
        case 13:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            float space = dir.length()*0.4;
            float bw = dir.x - space*2;
            float bh = dir.length()*0.15;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            lines.addColor( c );
            lines.addColor( c );

            // noise
            float y = (round)(ofRandomf())*bh/2*1.5;
            lines.addVertex( bst + ofVec2f(0, y) );
            lines.addVertex( bend );
            lines.addColor( c );
            lines.addColor( c );
            
            lines.addVertex( bend + ofVec2f(0, bh/4) );
            lines.addVertex( bend - ofVec2f(0, bh/4) );
            lines.addColor( c );
            lines.addColor( c );
            
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
        break;
            
        // =
        case 14:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            float space = dir.length()*0.4;
            float bh = dir.length()*0.1;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            lines.addColor( c );
            lines.addColor( c );
            
            // L wall
            lines.addVertex( bst + ofVec2f(0, bh/2) );
            lines.addVertex( bst - ofVec2f(0, bh/2));
            lines.addColor( c );
            lines.addColor( c );
            
            int step = ofNoise( bid*0.01) * 10;
            
            for (int i=0; i<step+1; i++) {
                float x = 0;
                float y = -bh/2 + bh/step*i;
                lines.addVertex( bst + ofVec2f(x, y));
                lines.addVertex( bend + ofVec2f(x, y) );
                lines.addColor( c );
                lines.addColor( c );
            }
            
            // R wall
            lines.addVertex( bend + ofVec2f(0, bh/2) );
            lines.addVertex( bend - ofVec2f(0, bh/2));
            lines.addColor( c );
            lines.addColor( c );

            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
            
        }
        break;
            
        // space with dot -* *-
        case 15:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            float space = dir.length()*0.3;
            float bh = dir.length()*0.1;
            ofVec3f bst = anchor + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( anchor );
            lines.addVertex( bst );
            lines.addColor( c );
            lines.addColor( c );
            
            points.addVertex( bst );
            points.addVertex( bend );
            points.addColor( c );
            points.addColor( c );
            
            // R space
            lines.addVertex( bend );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
        
            break;
            
        // circle
        case 16:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );

            ofVec3f dir = end - anchor;

            int num = ofNoise(bid*1.33) * 30;
            for (int k=0; k<num; k++) {

                int radius = dir.length()*(0.1+ofRandomuf()*0.3);
                radius = MIN(radius, 50);
                
                int step = radius*5;
                float adder = TWO_PI/step;
                ofVec2f anchor_t = anchor + ofVec2f( ofRandomf() ,ofRandomf())*30;
                for (int i=0; i<step; i++) {
                    if( ofRandomuf()> 0.7)
                        continue;

                    float rad1 = i*adder;
                    ofVec2f p( cos(rad1), sin(rad1) );
                    lines.addVertex( anchor_t + p * radius);
                    lines.addVertex( anchor_t + p * (radius+1) );
                    lines.addColor( c );
                    
                    lines.addColor( c );
                }
            }
            
            lines.addVertex( anchor );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
        }
        
            break;
            
            // Arc
        case 17:
        {
            lines.addVertex( st );
            lines.addVertex( anchor );
            lines.addColor( c );
            lines.addColor( c );
            
            ofVec3f dir = end - anchor;
            
            int num = 1;
            for (int k=0; k<num; k++) {
                
                int radius = dir.length()*(0.5 + ofRandomuf()*2);
                int step = radius*3;
                float adder = TWO_PI/step;
                
                step -= ofRandomuf() * step * 0.3;
                float st_rad = ofRandomuf() * TWO_PI;
                
                ofVec2f p;
                for (int i=0; i<step; i++) {
                    
                    if( ofRandomuf()> 0.8)
                        continue;

                    float rad = st_rad + i*adder;

                    
                    p.set( cos(rad), sin(rad) );
                    lines.addVertex( anchor + p * radius);
                    lines.addVertex( anchor + p * (radius+1) );
                    lines.addColor( c );
                    lines.addColor( c );
                    
                    if( ofRandomuf()< 0.4)
                        continue;
                }
                
//                lines.addVertex( anchor + p * radius);
//                p.set( cos(st_rad), sin(st_rad) );
//                lines.addVertex( anchor + p * radius);
                
                lines.addColor( c );
                lines.addColor( c );
                
            }
            
            lines.addVertex( anchor );
            lines.addVertex( end );
            lines.addColor( c );
            lines.addColor( c );
            
        }
            
        break;
            
        default:
            break;
    }
}

void Branch::draw(){

    glLineWidth(1);
    glPointSize(1);
    
    if( lines.getNumVertices() != 0 )
        lines.draw();
    
    if( points.getNumVertices() != 0 )
        points.draw();
    
    for (int i=0; i<children.size(); i++) {
        children[i].draw();
    }
    
    if(!bMainBranch){
        ofFill();
        ofSetLineWidth(0);
        ofSetColor( color );
        ofLine( end+ofVec3f(0,2,0), end-ofVec3f(0,2,0) );
    }
}

void Branch::clear(){
    
    for (int i=0; i<children.size(); i++) {
        children[i].clear();
        children.clear();
    }
}
