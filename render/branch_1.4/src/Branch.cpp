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
int Branch::max_f_type = 6;
float Branch::dist_limit = 100;

ofVec3f Branch::root = ofVec3f(0,0,0);
vector<Branch*> Branch::allb;

Branch::Branch()
:bInit(false),
speed(ofVec2f(0,0))
{
}

void Branch::craete(Branch *_parent, ofVec3f _dirn ){
    
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES );
    points.setUsage( GL_DYNAMIC_DRAW );
    points.setMode( OF_PRIMITIVE_POINTS );
    
    bInit = true;
    bDrawShapeAsPoint = false;
    bMainBranch = false;
    bStation = false;

    parent = _parent;
    dirn = _dirn;
    depth = 0;
    node_anim_time = 0;
    
    b_type = round( ofRandom(2, max_b_type) );
    f_type = round( ofRandom(1, max_f_type) );

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
    color.a = 0.5 + ofRandomuf()*0.4;
    
    if( ofRandomuf()>0.99 ){
        color.set(0.8,0,0.8 );
    }

    if( ofRandomuf()>0.99 ){
        color.set(0.1,0.8);
    }

    set_param();
    calc_posision();
    
    calc_shape();
    
    
    initpos = st;
    
    allb.push_back(this);
}

void Branch::set_param(){
    
    float nr = ofNoise( color.r, total_bnum*0.9);
    float ng = ofNoise( color.g, total_bnum*0.9);
    float nb = ofNoise( color.b, total_bnum*1.5);
    fw_len  = nr*nr*80 + 15;
    sp_len = ng*ng*80 + 15;
    sp_angle = nb*80;
    
#pragma mark JUMP
    // X jump
    bool xjump = ofRandomuf()>0.975;
    if( xjump ){
        fw_len *= ofRandom(2, 7);
        if( ofRandomuf() > 0.1 ){
            bMainBranch = true;
        }
    }else{
        fw_len *= 0.5;
    }
    
    // Y jump
    bool yjump = ofRandomuf()>0.96;

    if( yjump ){
        sp_len *= ofRandom(2, 10);

        float r = ofRandomuf();
        if( r<0.2 ){
            sp_angle = ofRandom(5, 60);
        }else if( r<0.35 ){
            sp_angle = 45;
            
            float r = ofRandomuf();
            if( r>0.3 ){
                //b_type = floor(ofRandom(2,5));
            }
            
        }else if( r<0.75 ){
            sp_angle = 60;
        }else{
            sp_angle = 45;
            sp_len *= 0.9;
        }
        
        if( ofRandomuf() > 0.1 ){
            bMainBranch = true;
        }
    }
    
    node_anim_speed = ofRandom(1.5, 5);
    
#pragma mark STATION_SETTING
    if( parent!=NULL && parent->bStation ==false ){
        if( yjump || xjump )
            bStation = ofRandomf() > 0.95;
    }

    if( bStation ){
        num_son = 5;
    }else{
        num_son = ofNoise(bid)*2 + 1;
    }
    
    if( parent && parent->bStation ){
        sp_angle = ofRandom(0,20);
        sp_len += ofRandom(50,120);
        fw_len += ofRandom(50,120);
        node_anim_speed = 1;
    }

#pragma mark ROOT_SETTINGS
    if( parent==NULL ){
        sp_angle = ofRandom(0, 30);
        sp_len += ofRandom(50,120);
        fw_len += ofRandom(50,120);
        st = root;
        f_type = ofRandom(0,max_f_type);
        node_anim_speed = 0.3;
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
        if(bStation) n = 2;
        for (int i=0; i<n; i++) {
            children.push_back( Branch() );
            children[children.size()-1].craete(this, dirn );
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
    
    if( active_total > 220 ){
        bMainBranch = false;
    }

    for (int i=0; i<children.size(); i++) {
        children[i].update();
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
        
//        if( speed.x == -123){
//            float xamp = 0.5;
//            float yamp = 0.2;
//            speed.x = ofNoise(bid*0.35 + 4) * xamp;
//            speed.y = ofSignedNoise(bid*0.5) * yamp;
//        }

        speed.x = ofNoise( st.x * 1.7) * 0.1;
        speed.y += ofSignedNoise(st.y*0.01)*0.001;
        
        if( ofRandomuf()>0.1 ){
            if( parent && !parent->bStation){
                st += speed;
            }
            anchor += speed;
            end += speed;
        }
    }
    
    if( 1 ){
        if( ofRandomf() > 0.999){
            if( fw_len>40 ){
                fw_len *= ofRandom(0.97, 1.03);
                calc_posision();
            }
        }

        if( ofRandomf() > 0.999){
            sp_angle += ofRandom(-1, 1);
            calc_posision();
        }
        
        if( ofRandomf() > 0.999){
            if( sp_len>40 ){
                sp_len *= ofRandom(0.97, 1.03);
                calc_posision();
            }
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
//        {
//            lines.addVertex( sp ); lines.addColor( c );
//            lines.addVertex( ap ); lines.addColor( c );
//        }
//            break;

        // LINEAR, dotted
        case 1:
//        {
//            ofVec3f dir = ap - sp;
//            int step = dir.length();
//            if( step%2 !=0 ) step++;
//            
//            int n_step = MIN(node_anim_time, step);
//            ofVec3f adder = dir / step;
//            for(int i=0; i<step; i++){
//                lines.addVertex( sp + adder*i); lines.addColor( c );
//            }
//        }
//        break;

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
            
            int step = width;
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
            bool bDot = false; //bid%2==0;

            ofxTransitions tr;
            ofVec3f dir = ap - sp;
            float width = dir.x;
            float height = dir.y;
            
            int num_line = 1;
            if( ofNoise(bid*0.4)>0.8)
                num_line = ofNoise(bid)*5 + 1;

            int step = width;
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
            bool bDot = false; //bid%2==0;
            
            ofxTransitions tr;
            ofVec3f current = sp;
            ofVec3f dir = ap - sp;
            float width = dir.x;
            float height = dir.y;
            int step = width;
            int n_step = MIN(node_anim_time, step);
            
            for (int i=0; i<n_step; i++) {
                float x = width * ((float)i/step);
                float y = 0;

                int easeType = round(ofNoise(bid*3) * 3);
                switch (easeType) {
                    case 0: y = tr.easeInOutQuart(i, 0, height, step); break;
                    case 1: y = tr.easeInOutExpo(i, 0, height, step); break;
                    //case 2: y = tr.easeInBack(i, 0, height, step); break;
                    //case 3: y = tr.easeOutBack(i, 0, height, step); break;
                    case 2: y = tr.easeInOutQuart(i, 0, height, step); break;
                    case 3: y = tr.easeInOutExpo(i, 0, height, step); break;
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
//        {
//            lines.addVertex( ap ); lines.addColor( c );
//            lines.addVertex( ep ); lines.addColor( c );
//        }

        break;
        
    
        // | | |
        case 1:
        {
            float space = len*0.23;
            float bw = dir.x - space*2;
            bw = MIN(bw, 30);
            float bh = bw*0.15;
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            int step = bw;
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
        case 2:
        {
            float space = len*0.1;
            float bw = len - space*2;
            float bh = len*0.15;
            bw = MIN(bw,20);
            bh = MIN(bh,5);
            
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            int step = bw;
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

            
        // space with dot -* *-
        case 3:
        {
            float space = 5;
            ofVec3f bst = ap + ofVec3f(space,0,0);
            ofVec3f bend = ep - ofVec3f(space,0,0);
            
            // L space
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( bst ); lines.addColor( c );
            
            points.addVertex( bst ); points.addColor( c );
            points.addVertex( bend ); points.addColor( c );
            
            // R space
            lines.addVertex( bend ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
        
            break;
            
        // circle
        case 4:
        {
            int radius = len*0.6;
            int step = radius*5;
            float adder = TWO_PI/step;
            int n_step = MIN(step, node_anim_time*3 );

            for (int i=0; i<n_step; i++) {
                if( ofRandomuf()> 0.99)
                    continue;
                float rad1 = i*adder;
                ofVec2f p( cos(rad1), sin(rad1) );
                lines.addVertex( ap + p * radius); lines.addColor( c );
                lines.addVertex( ap + p * (radius+1) ); lines.addColor( c );
                
                if( ofNoise(bid)>0.65 ){
                    break;
                }
            }
           
            lines.addVertex( ap ); lines.addColor( c );
            lines.addVertex( ep ); lines.addColor( c );
        }
    
        break;
    
        // Arc
        case 5:
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
            
        default:
            break;
    }
    
}

void Branch::draw(){
    
    glLineWidth(1);
    glPointSize(2);
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

void Branch::update_s(){
    
    active_total = 0;
    dist_limit = ofApp::app->max_dist * (float)ofGetFrameNum()/ofApp::app->max_frame;
    
    int n= allb.size()-1;

//    if( n < 50 )
//        return;
//    
//    int nline = n * 0.1;
//    for (int i=0; i<nline; i++) {
//        
//        int id1 = ofRandomuf() * n;
//        int id2 = ofRandomuf() * n;
//        int id3 = ofRandomuf() * n;
//        
//        ofVec3f &v1 = allb[id1]->end;
//        ofVec3f &v2 = allb[id2]->anchor;
//        ofVec3f &v3 = allb[id3]->st;
//        ofFloatColor c(1,0,0);
//
//        if( allb[id1]->bInit )
//            allb[id1]->add_line(v1, v2, v3, c, 0, 0);
//    }
}
