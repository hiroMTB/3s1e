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
int Branch::max_b_type = 4;
int Branch::max_f_type = 15;
float Branch::dist_limit = 100;

ofVec3f Branch::root = ofVec3f(0,0,0);
vector<Branch*> Branch::nonp;

void Branch::craete(Branch *_parent, ofVec3f _dirn ){
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    bDrawShapeAsPoint = false;
    bMainBranch = false;
    bStation = false;

    parent = _parent;
    dirn = _dirn;
    depth = 0;
    node_anim_time = 0;
    
    b_type = round( ofRandomuf() * max_b_type);
    f_type = round( ofRandomuf() * max_f_type);

    if( parent )
        depth = parent->depth+1;
    
    bid = total_bnum;
    
    // static param
    top_depth = MAX(top_depth, depth);
    total_bnum++;
    
    color.set(1);
    int w = ofApp::app->img.getWidth();
    int h = ofApp::app->img.getHeight();
    int x = bid*3%w;
    int y = bid*3/w;
    color = ofApp::app->img.getColor( x, y );
    color.a = 0.5 + ofRandomuf()*0.7;
    
    if( ofRandomuf()>0.99 ){
        color.set(0.8,0,0.8 );
    }

    if( ofRandomuf()>0.99 ){
        color.set(0,1);
    }

    set_param();

    calc_posision();
    calc_shape();

    if( bMainBranch )
        active_total++;
    
    initpos = st;
    speed.set(-123,-123);

}

void Branch::set_param(){
    
    fw_len = ofRandom( 20, 70);
    sp_len = ofRandom( 20, 70);
    sp_angle = ofRandom(30, 100);
    
#pragma mark JUMP
    // X jump
    bool xjump = ofRandomuf()>0.99;
    if( xjump ){
        if( b_type != 6 && b_type != 9 && b_type != 7 && b_type != 8 ){
            fw_len *= ofRandom(2, 10);

            if( ofRandomuf() > 0.1 ){
                bMainBranch = true;
            }
        }
    }else{
        fw_len *= 0.5;
    }
    
    // Y jump
    bool yjump = ofRandomuf()>0.98;

    if( yjump ){
        sp_len *= ofRandom(2, 7);

        float r = ofRandomuf();
        if( r<0.2 ){
            sp_angle = ofRandom(45, 90);
        }else if( r<0.35 ){
            sp_angle = 45;
            
            float r = ofRandomuf();
            if( r>0.3 ){
                b_type = floor(ofRandom(2,5));
            }
            
        }else if( r<0.75 ){
            sp_angle = 90;
        }else{
            sp_angle = 135;
            sp_len *= 0.9;
        }
        
        if( ofRandomuf() > 0.2 ){
            bMainBranch = true;
        }
    }
    
    node_anim_speed = ofRandom(0.3, 2);
    
#pragma mark STATION_SETTING
    if( parent!=NULL && parent->bStation ==false ){
        if( yjump || xjump )
            bStation = ofRandomf() > 0.95;
    }

    if( bStation ){
        num_son = 5;
    }else{
        num_son = ofNoise(bid)*6 + 3;
    }
    
    if( parent && parent->bStation ){
        sp_angle = ofRandom(20,90);
        sp_len += ofRandom(30,150);
        fw_len += ofRandom(30,150);
        b_type = ofRandom(0, max_b_type);
    }

#pragma mark ROOT_SETTINGS
    if( parent==NULL ){
        sp_angle = 0;
        sp_len += ofRandom(70,150);
        fw_len += ofRandom(70,150);
        st = root;
        b_type = ofRandom(0,max_b_type);
        node_anim_speed = 1;
    }
    
    sp_angle *= ofRandomuf()>0.5 ? 1: -1;
}

void Branch::create_child(){

    if( root.distance( st ) > dist_limit*ofRandom(0.9,1.1)  )
      return;

    
    bool top = depth == top_depth;
    
    if ( top ) {
        bMainBranch = true;
    }
    
    if( bMainBranch ){
        
        if( !bStation && ofRandomuf() > 0.3 ) return;
   
        int n = 1;
        if(bStation) n = ofRandomuf()*5 + 5;
        for (int i=0; i<n; i++) {
            Branch child;
            child.craete(this, dirn );
            children.push_back( child );
        }
        
        // inheirt main branch        
        if( bMainBranch && children.size() > num_son ){
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

    if( bMainBranch ){
        active_total++;
    }
    
    animate();
    calc_shape();
    
    if( active_total > 12 ){
        bMainBranch = false;
    }

    for (int i=0; i<children.size(); i++) {
        children[i].update();
    }
    
    
    if( parent == NULL){
        nonp.push_back(this);
    }
}

void Branch::revive(){
    if( depth > top_depth*0.2 ){
        if( ofRandomuf()>0.95 ){
            bMainBranch = true;
        }
    }
}

void Branch::animate(){
    if( 1 ){
        node_anim_time += node_anim_speed;
        
        if( speed.x == -123){
            float xamp = 0.5;
            float yamp = 0.1;
            speed.x = ofNoise(bid*0.35 + 4) * xamp;
            speed.y = ofSignedNoise(bid*0.5) * yamp;
        }
        
        if( parent && !parent->bStation){
            st += speed;
        }
        
        anchor += speed;
        end += speed;
    }
    
    if( 1 ){
        
        if( ofRandomf() > 0.993){
            fw_len *= 0.8;
            calc_posision();
        }

        if( ofRandomf() > 0.993){
            sp_angle += ofRandom(-10, 10);
            calc_posision();
        }
        
        if( ofRandomf() > 0.993){
            sp_len *= 0.8;
            calc_posision();
        }
        
        if( ofRandomf() > 0.999){
            node_anim_time = 0 + ofRandom(10, 30);
            calc_posision();
        }
    }
}

void Branch::calc_posision(){
    
    if( parent ){
        st = parent->end;
    }
    
    anchor = st + dirn.rotated(sp_angle, ofVec3f(0,0,1) ) * sp_len;
    end = anchor + dirn * fw_len;
}

void Branch::calc_shape(){
    
    ofFloatColor c = color;
    
    // clear
    lines.clearColors();
    lines.clearVertices();
    points.clearColors();
    points.clearVertices();
    
    add_line(st, anchor, end, c, b_type, f_type );
    
//    if( parent ) {
//        if( parent->depth == depth-1){
//            ofVec2f dir = st - parent->end;
//            if( dir.length() < 3) return;
//
//            ofVec2f at = parent->end;
//            at.y += dir.y;
//            ofFloatColor ct = c;
//            ct.a *= 0.8;
//            add_line(parent->end, at, st, ct, round(ofRandom(0, max_b_type)), 0 );
//        }
//    }
    
    if (bStation) {
        points.addVertex( st );
        points.addColor( c );
    }
    
}

void Branch::add_line(const ofVec2f &sp, const ofVec2f &ap, const ofVec2f &ep, const ofFloatColor &c, int btype, int ftype){
    
    // make sp shape
    switch( btype ){
        
        // LINEAR
        case 0:
        {
            lines.addVertex( sp ); lines.addColor( c );
            lines.addVertex( ap ); lines.addColor( c );
        }
            break;

        // LINEAR, dotted
        case 1:
        {
            ofVec3f dir = ap - sp;
            int step = dir.length();
            if( step%2 !=0 ) step++;
            
            int n_step = MIN(node_anim_time, step);
            ofVec3f adder = dir / step;
            for(int i=0; i<step; i++){
                lines.addVertex( sp + adder*i); lines.addColor( c );
            }
        }
        break;

        // SplineA
        case 2:
        {
            ofxTransitions tr;
            ofVec3f dir = ap - sp;
            float width = dir.x;
            float height = dir.y;

            int num_line = 1;
            if( ofNoise(bid)>0.85)
                num_line = ofNoise(bid)*5 + 1;
            
            int step = width * 0.5;
            for (int k=0; k<num_line; k++) {
             
                float offsetx = k*4;
                ofVec3f current = sp;

                int n_step = MIN(node_anim_time, step);

                for (int i=0; i<n_step; i++) {
                    float x = width * ((float)i/step) + offsetx;
                    float y = tr.easeInOutCubic(i, 0, height, step);
                    lines.addVertex( current );
                    current = sp + ofVec3f(x, y, 0);
                    lines.addVertex( current );
                    lines.addColor( c );
                    lines.addColor( c );
                }
                
                lines.addVertex( current );
                lines.addVertex( ap );
                lines.addColor( c );
                lines.addColor( c );
            }
        }
        break;

        // SplineB
        case 3:
        {
            bool bDot = bid%2==0;

            ofxTransitions tr;
            ofVec3f dir = ap - sp;
            float width = dir.x;
            float height = dir.y;
            
            int num_line = 1;
            if( ofNoise(bid*0.4)>0.8)
                num_line = ofNoise(bid)*5 + 1;

            int step = width * 0.5;
            int n_step = MIN(node_anim_time, step);

            for (int k=0; k<num_line; k++) {
            
                float offsetx = k*4;
                ofVec3f current = sp;
                
                for (int i=0; i<n_step; i++) {

                    float x = width * ((float)i/step) + offsetx;
                    float y = tr.easeInOutSine(i, 0, height, step);

                    if( bDot && i%2==0){
                        current = sp + ofVec3f(x, y, 0);
                    }else{
                        lines.addVertex( current );
                        current = sp + ofVec3f(x, y, 0);
                        lines.addVertex( current );
                        lines.addColor( c );
                        lines.addColor( c );
                    }
                }
                lines.addVertex( current );
                lines.addVertex( ap );
                lines.addColor( c );
                lines.addColor( c );
            }
        }
            break;
        
        // Spline C
        case 4:
        {
            bool bDot = bid%2==0;
            
            ofxTransitions tr;
            ofVec3f current = sp;
            ofVec3f dir = ap - sp;
            float width = dir.x;
            float height = dir.y;
            int step = width * 0.5;
            int n_step = MIN(node_anim_time, step);
            
            for (int i=0; i<n_step; i++) {
                float x = width * ((float)i/step);
                float y = 0;

                int easeType = round(ofNoise(bid*3) * 5);
                switch (easeType) {
                    case 0: y = tr.easeInOutQuart(i, 0, height, step); break;
                    case 1: y = tr.easeInOutExpo(i, 0, height, step); break;
                    case 2: y = tr.easeInBack(i, 0, height, step); break;
                    case 3: y = tr.easeOutBack(i, 0, height, step); break;
                    case 4: y = tr.easeInOutQuart(i, 0, height, step); break;
                    case 5: y = tr.easeInOutExpo(i, 0, height, step); break;
                    default: y = tr.easeInOutCirc(i, 0, height, step); break;
                }
                
                if ( bDot && i%2==0 ) {
                    current = sp + ofVec3f(x, y, 0);
                }else{
                    lines.addVertex( current ); lines.addColor( c );
                    current = sp + ofVec3f(x, y, 0);
                    lines.addVertex( current ); lines.addColor( c );
                }
            }
            
            lines.addVertex( current );
            lines.addVertex( ap );
            lines.addColor( c );
            lines.addColor( c );
        }
        break;
    }
    
    
    /*
     *
     *
     *      NODE
     *
     *
     */
    if( node_anim_time < 1 ){
        return;
    }
    
    ofVec3f dir = ep - ap;
    float len = dir.length();
    
    switch (ftype) {

        // Nothing
        case 0:
        {
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }

        break;
        
        // Box A, cylinder
        case 1:
        {

            float space = len*0.2;
            float bh = space/2;
            bh = MIN(bh, 6);
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);

            // move a bit
            float speed = ofNoise(bid)*0.05;
            bend.x -= ofNoise(bid*0.1, ofGetFrameNum()*speed*0.05)*dir.x*0.3;
            
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            ofVec3f p3 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p4 = bend + ofVec3f(0,-bh/2,0);

            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );

            // L
            lines.addVertex( p1 ); lines.addColor( c );
            lines.addVertex( p2 ); lines.addColor( c );

            // top
            lines.addVertex( p1 ); lines.addColor( c );
            lines.addVertex( p3 ); lines.addColor( c );

            // bottom
            lines.addVertex( p2 ); lines.addColor( c );
            lines.addVertex( p4 ); lines.addColor( c );

            // R
            lines.addVertex( p3 ); lines.addColor( c );
            lines.addVertex( p4 ); lines.addColor( c );

            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
            
        break;
            
            
        // reg Box
        case 2:
        {
            float space = len*0.4;
            float bw = len - space*2;
            float bh = len*0.2;
            bh = MIN(bh, 20);
            
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            ofVec3f p3 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p4 = bend + ofVec3f(0,-bh/2,0);
            
            // L space
            lines.addVertex( ap );  lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            // L
            lines.addVertex( p1 );  lines.addColor( c );
            lines.addVertex( p2 );  lines.addColor( c );
            
            // top
            lines.addVertex( p1 );  lines.addColor( c );
            lines.addVertex( p3 );  lines.addColor( c );
            
            // bottom
            lines.addVertex( p2 );  lines.addColor( c );
            lines.addVertex( p4 );  lines.addColor( c );
            
            // R
            lines.addVertex( p3 );  lines.addColor( c );
            lines.addVertex( p4 );  lines.addColor( c );
            
            // R space
            lines.addVertex( bend );    lines.addColor( c );
            lines.addVertex( ep );  lines.addColor( c );
        }
        break;
            
        // Triangle <|
        case 3:
        {
            float space = len*0.3;
            float bh = len*0.4 / 1.732 * 2.0;
            bh = MIN(bh, 20);
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            ofVec3f p1 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bend + ofVec3f(0,-bh/2,0);
            
            // L space
            lines.addVertex( ap );  lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );

            // /
            lines.addVertex( bst ); lines.addColor( c );
            lines.addVertex( p1 );  lines.addColor( c );

            //
            lines.addVertex( bst ); lines.addColor( c );
            lines.addVertex( p2 );  lines.addColor( c );

            // |
            lines.addVertex( p1 );  lines.addColor( c );
            lines.addVertex( p2 );  lines.addColor( c );

            
            // R space
            lines.addVertex( bend );    lines.addColor( c );
            lines.addVertex( ep );  lines.addColor( c );
        }
        break;
            
        // Triangle rev
        case 4:
        {
            float space = len*0.3;
            float bh = len*0.4 / 1.732 * 2.0;
            bh = MIN(bh, 20);
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            
            // L space
            lines.addVertex( ap );  lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            // |
            lines.addVertex( p1 );  lines.addColor( c );
            lines.addVertex( p2 );  lines.addColor( c );

            // /
            lines.addVertex( p1 );  lines.addColor( c );
            lines.addVertex( bend );lines.addColor( c );
            
            //
            lines.addVertex( p2 );  lines.addColor( c );
            lines.addVertex( bend );lines.addColor( c );
            
            // R space
            lines.addVertex( bend );lines.addColor( c );
            lines.addVertex( ep );  lines.addColor( c );
            
        }
        break;
            
        // diamond
        case 5:
        {
            float space = len*0.3;
            float bh = len*0.25;
            bh = MIN(bh, 20);
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            ofVec3f mid = (bend + bst) *0.5;
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );

            int step = ofNoise( bid*10) * 6;
            float addery = bh * 0.4;
            float x = 0;
            float y = -addery * (step+1.0)/2.0;
            for (int i=0; i<step; i++) {
                y += addery;
                if( ofNoise(bid)>0.8 ){
                    x += ofSignedNoise( i + bid*0.1, ofGetFrameNum()*0.05) * dir.x*0.15;
                    y += ofSignedNoise(i*2 + bid*0.1, ofGetFrameNum()*0.1) * bh*0.15;
                }
                lines.addVertex( bst ); lines.addColor( c );
                lines.addVertex( mid + ofVec2f(x, y) );lines.addColor( c );
                lines.addVertex( mid + ofVec2f(x, y) ); lines.addColor( c );
                lines.addVertex( bend ); lines.addColor( c );

            }
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        break;

        // dot
        case 6:
        {
            float space = len*0.01;
            float bh = len*0.2;
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);

            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            int step = dir.x * 0.5;
            step = MIN(node_anim_time, step);

            ofVec3f current = bst;
            ofVec3f adder = dir / step;
            
            for (int i=0; i<step; i++) {
                current += adder;
                lines.addVertex( current ); lines.addColor( c );
            }
            
            if(step % 2 != 0){
                lines.addVertex( current ); lines.addColor( c );
            }
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
            break;

    
        // | | |
        case 7:
        {
            float space = len*0.23;
            float bw = dir.x - space*2;
            float bh = bw*0.15;
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            int step = bw * 0.5;
            ofVec3f current = bst;
            float adderx = bw / step;

            for (int i=0; i<step; i++) {
                current.x += adderx;
                float y = ofSignedNoise(i, ofGetFrameNum()*0.1) * bh/2 * 0.8;
                lines.addVertex( current + ofVec2f(0, y) ); lines.addColor( c );
                lines.addVertex( current - ofVec2f(0, y) ); lines.addColor( c );
            }
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        break;
            
        // wave
        case 8:
        {
            float space = len*0.1;
            float bw = len - space*2;
            float bh = len*0.15;
            bh = MIN(bh,10);
            
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            int step = bw/2;
            step = MIN(node_anim_time, step);

            ofVec3f current = bst;
            float adderx = bw / step;
            
            float freq1 = ofNoise(bid, ofGetFrameNum()*0.8) * ofNoise(bid)*2.0;
            float freq2 = ofNoise(bid, ofGetFrameNum()*0.2) * ofNoise(bid)*2.0;
            
            if( ofNoise(bid*3) > 0.4){
                for (int i=0; i<step; i++) {
                    float y = (sin(i*freq1) * sin(i*freq2));
                    y = round(y) * bh/2;
                    lines.addVertex( current ); lines.addColor( c );
                    lines.addVertex( current + ofVec2f(0, y) ); lines.addColor( c );
                    current.x += adderx;
                }
            }else{
                float pasty = 0;
                for (int i=0; i<step; i++) {
                    float y = (sin(i*freq1) * sin(i*freq2))*bh/2;
                    y = round(y) * bh/2;
                    lines.addVertex( current + ofVec2f(0, pasty) ); lines.addColor( c );
                    lines.addVertex( current + ofVec2f(0, y) ); lines.addColor( c );
                    current.x += adderx;
                    pasty = y;
                }
            }
            // base lineR space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        break;

        // switch
        case 9:
        {
            float space = len*0.4;
            float bw = dir.x - space*2;
            float bh = len*0.15;
            bh = MIN(bh, 10);
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );

            for (int i=0; i<5; i++) {
                // noise
                float y = ofRandomf()*bh/2*1.5;
                lines.addVertex( bst ); lines.addColor( c );
                lines.addVertex( bend + ofVec2f(0, y) ); lines.addColor( c );
            }
            lines.addVertex( bend + ofVec2f(0, bh/4) ); lines.addColor( c );
            lines.addVertex( bend - ofVec2f(0, bh/4) ); lines.addColor( c );
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        break;
            
        // =
        case 10:
        {
            float space = len*0.4;
            float bh = len*0.1;
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            // L wall
            lines.addVertex( bst + ofVec2f(0, bh/2) ); lines.addColor( c );
            lines.addVertex( bst - ofVec2f(0, bh/2)); lines.addColor( c );
            
            int step = ofNoise( bid, ofGetFrameNum()*0.01) * 10;
            
            for (int i=0; i<step+1; i++) {
                float x = 0;
                float y = -bh/2 + bh/step*i;
                lines.addVertex( bst + ofVec2f(x, y)); lines.addColor( c );
                lines.addVertex( bend + ofVec2f(x, y) ); lines.addColor( c );
            }
            
            // R wall
            lines.addVertex( bend + ofVec2f(0, bh/2) ); lines.addColor( c );
            lines.addVertex( bend - ofVec2f(0, bh/2)); lines.addColor( c );

            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
            
        }
        break;
            
        // space with dot -* *-
        case 11:
        {
            float space = len*0.3;
            float bh = len*0.1;
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            points.addVertex( bst ); lines.addColor( c );
            points.addVertex( bend ); lines.addColor( c );
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        
            break;
            
        // circle
        case 12:
        {
            int radius = len*0.6;
            int step = radius*5;
            float adder = TWO_PI/step;
            int n_step = MIN(step, node_anim_time*3 );

            for (int i=0; i<n_step; i++) {
                if( ofRandomuf()> 0.95)
                    continue;
                float rad1 = i*adder;
                ofVec2f p( cos(rad1), sin(rad1) );
                lines.addVertex( ap + p * radius); lines.addColor( c );
                lines.addVertex( ap + p * (radius+1) ); lines.addColor( c );
            }
           
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
    
        break;
    
        // Arc
        case 13:
        {
            int radius = len;
            int step = radius*3;
            float adder = TWO_PI/step;
            float st_rad = -TWO_PI/4 * round(ofSignedNoise(bid+4+initpos.x*3)*4.0);
            step *= 0.25;
            step = MIN(node_anim_time, step);

            ofVec2f p;
            for (int i=0; i<step+1; i++) {
                float rad = st_rad + i*adder;
                
                p.set( cos(rad), sin(rad) );
                lines.addVertex( ap + p * radius); lines.addColor( ofFloatColor(0,1) );
                lines.addVertex( ap + p * (radius+1) ); lines.addColor( ofFloatColor(0,1) );
            }

            if( round(ofNoise(bid+4+initpos.x)) < 0.2){
                lines.addVertex( ap + p * (radius) ); lines.addColor( c );
                lines.addVertex( ap ); lines.addColor( c );
            }
            
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
            
        }
        break;
            
        // spike
        case 14:
        {
            float space = len*0.2;
            float bw = len - space*2;
            float bh = len;
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            int step = bw;
            step += step%2;

            float max = step*step;
            float x, y;

            int n_step = MIN(node_anim_time/2, step);

            int minus = ofNoise( initpos.x, initpos.y)>0.5 ?  1 : -1;
            for (int i=0; i<n_step; i++) {
                float id = i;
                x = (float)id * bw/(float)step;
                y = (float)(id*id)/max * bh;
                y *= minus;
                lines.addVertex( bst + ofVec2f(x, y) ); lines.addColor( c );
                lines.addVertex( bst + ofVec2f(x+1, y) ); lines.addColor( c );
            }

            lines.addVertex( bst + ofVec2f(x, y) ); lines.addColor( c );
            lines.addVertex( bst + ofVec2f(x, 0) ); lines.addColor( c );

            points.addVertex( bst + ofVec2f(x, 0)); lines.addColor( c );
            points.addVertex( bend ); lines.addColor( c );
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        break;
            
        // random point
        case 15:
        {
            float space = len*0.3;
            float bw = len - space*2;
            float bh = len*2;
            bw = MIN(bw, 15);
            bh = MIN(bh, 38);
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            float minus = bid%2==1 ? 1: -1;
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );

            for (int i=0; i<len/3; i++) {
                int x = ofRandomuf() * bw;
                int y = ofRandomuf() * bh + 1;
                y *= minus;
                points.addVertex( bst + ofVec2f(x, y)); points.addColor( c );
            }
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        break;

        
        default:
            break;
    }
    
}

void Branch::draw(){

    glLineWidth(1);
    glPointSize(1);
    ofSetColor( 255 );

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
