//
//  ad_graph.cpp
//  expl_1.1
//
//  Created by hiroshi matoba on 1/12/15.
//
//

#include "ad_graph.h"
#include "ofxTransitions.h"

int ad_graph::max_btype = 3;
int ad_graph::max_ftype = 15;

void ad_graph::add_branch( ofVboMesh &mesh, const ofVec2f &st, const ofVec2f &end, ofFloatColor c, int btype, bool dot ){
    
    if( btype == -1 ){
        btype = round( ofRandom(0, max_btype) );
    }
    
    switch( btype ){
            
        // LINEAR
        case 0:
        {
            if(!dot){
                mesh.addVertex( st ); mesh.addColor( c );
                mesh.addVertex( end ); mesh.addColor( c );
            }else{
                ofVec3f dir = end - st;
                int step = dir.length();
                step += step%2;
                ofVec3f adder = dir / step;
                for(int i=0; i<step; i++){
                    mesh.addVertex( st + adder*i); mesh.addColor( c );
                }
            }
        }
       break;
            
        // Curve A
        case 1:
        {
            ofxTransitions tr;
            ofVec3f dir = end - st;
            float width = dir.x;
            float height = dir.y;
            
            int step = abs(width*2);
            ofVec3f current = st;
            
            for (int i=0; i<step; i++) {
                float x = width * ((float)i/step);
                float y = tr.easeInOutCubic(i, 0, height, step);
                mesh.addVertex( current );
                current = st + ofVec3f(x, y, 0);
                mesh.addVertex( current );
                mesh.addColor( c );
                mesh.addColor( c );
            }
            
//            mesh.addVertex( current );
//            mesh.addVertex( end );
//            mesh.addColor( c );
//            mesh.addColor( c );
        }
            break;
    
        // Curve B
        case 2:
        {
            ofxTransitions tr;
            ofVec3f dir = end - st;
            float width = dir.x;
            float height = dir.y;
            
            int step = abs( width*2 );
            
            ofVec3f current = st;
                
            for (int i=0; i<step; i++) {
                
                float x = width * ((float)i/step);
                float y = tr.easeInOutSine(i, 0, height, step);
                
                if( dot && i%2==0){
                    current = st + ofVec3f(x, y, 0);
                }else{
                    mesh.addVertex( current );
                    current = st + ofVec3f(x, y, 0);
                    mesh.addVertex( current );
                    mesh.addColor( c );
                    mesh.addColor( c );
                }
            }
            mesh.addVertex( current );
            mesh.addVertex( end );
            mesh.addColor( c );
            mesh.addColor( c );
        }
        break;
    
        // Curve C
        case 3:
        {
            ofxTransitions tr;
            ofVec3f current = st;
            ofVec3f dir = end - st;
            float width = dir.x;
            float height = dir.y;
            int step = abs(width * 3);
            
            for (int i=0; i<step; i++) {
                float x = width * ((float)i/step);
                float y = tr.easeOutBack(i, 0, height, step);
                
                if ( dot && i%2==0 ) {
                    current = st + ofVec3f(x, y, 0);
                }else{
                    mesh.addVertex( current ); mesh.addColor( c );
                    current = st + ofVec3f(x, y, 0);
                    mesh.addVertex( current ); mesh.addColor( c );
                }
            }
            
            mesh.addVertex( current );
            mesh.addVertex( end );
            mesh.addColor( c );
            mesh.addColor( c );
        }
        break;
    }
}


void ad_graph::add_flower(ofVboMesh &mesh, const ofVec2f &st, const ofVec2f &end, ofFloatColor c, int ftype ){

    if( ftype == -1 ){
        ftype = round( ofRandom(0, max_ftype) );
    }
    
    ofVec3f dir = end - st;
    float len = dir.length();
    
    switch (ftype) {
            
        // Nothing
        case 0:
        {
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
        break;
            
        // Box A, cylinder
        case 1:
        {
            
            float space = len*0.2;
            float bh = space/2;
            bh = MIN(bh, 6);
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            ofVec3f p3 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p4 = bend + ofVec3f(0,-bh/2,0);
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            // L
            mesh.addVertex( p1 ); mesh.addColor( c );
            mesh.addVertex( p2 ); mesh.addColor( c );
            
            // top
            mesh.addVertex( p1 ); mesh.addColor( c );
            mesh.addVertex( p3 ); mesh.addColor( c );
            
            // bottom
            mesh.addVertex( p2 ); mesh.addColor( c );
            mesh.addVertex( p4 ); mesh.addColor( c );
            
            // R
            mesh.addVertex( p3 ); mesh.addColor( c );
            mesh.addVertex( p4 ); mesh.addColor( c );
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
            
        break;
            
            
        // reg Box
        case 2:
        {
            float space = len*0.4;
            float bw = len - space*2;
            float bh = len*0.2;
            bh = MIN(bh, 20);
            
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            ofVec3f p3 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p4 = bend + ofVec3f(0,-bh/2,0);
            
            // L space
            mesh.addVertex( st );  mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            // L
            mesh.addVertex( p1 );  mesh.addColor( c );
            mesh.addVertex( p2 );  mesh.addColor( c );
            
            // top
            mesh.addVertex( p1 );  mesh.addColor( c );
            mesh.addVertex( p3 );  mesh.addColor( c );
            
            // bottom
            mesh.addVertex( p2 );  mesh.addColor( c );
            mesh.addVertex( p4 );  mesh.addColor( c );
            
            // R
            mesh.addVertex( p3 );  mesh.addColor( c );
            mesh.addVertex( p4 );  mesh.addColor( c );
            
            // R space
            mesh.addVertex( bend );    mesh.addColor( c );
            mesh.addVertex( end );  mesh.addColor( c );
        }
        break;
            
        // Triangle <
        case 3:
        {
            float space = len*0.3;
            float bh = len*0.4 / 1.732 * 2.0;
            bh = MIN(bh, 20);
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            ofVec3f p1 = bend + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bend + ofVec3f(0,-bh/2,0);
            
            // L space
            mesh.addVertex( st );  mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            // /
            mesh.addVertex( bst ); mesh.addColor( c );
            mesh.addVertex( p1 );  mesh.addColor( c );
            
            //
            mesh.addVertex( bst ); mesh.addColor( c );
            mesh.addVertex( p2 );  mesh.addColor( c );
            
            // |
            mesh.addVertex( p1 );  mesh.addColor( c );
            mesh.addVertex( p2 );  mesh.addColor( c );
            
            
            // R space
            mesh.addVertex( bend );    mesh.addColor( c );
            mesh.addVertex( end );  mesh.addColor( c );
        }
        break;
            
        // Triangle rev
        case 4:
        {
            float space = len*0.3;
            float bh = len*0.4 / 1.732 * 2.0;
            bh = MIN(bh, 20);
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            ofVec3f p1 = bst + ofVec3f(0,bh/2,0);
            ofVec3f p2 = bst + ofVec3f(0,-bh/2,0);
            
            // L space
            mesh.addVertex( st );  mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            // |
            mesh.addVertex( p1 );  mesh.addColor( c );
            mesh.addVertex( p2 );  mesh.addColor( c );
            
            // /
            mesh.addVertex( p1 );  mesh.addColor( c );
            mesh.addVertex( bend );mesh.addColor( c );
            
            //
            mesh.addVertex( p2 );  mesh.addColor( c );
            mesh.addVertex( bend );mesh.addColor( c );
            
            // R space
            mesh.addVertex( bend );mesh.addColor( c );
            mesh.addVertex( end );  mesh.addColor( c );
            
        }
        break;

            
        // Diamond
        case 5:
        {
            float space = len*0.3;
            float bh = len*0.25;
            bh = MIN(bh, 20);
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            ofVec3f mid = (bend + bst) *0.5;
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            int step = 6;
            float addery = bh * 0.4;
            float x = 0;
            float y = -addery * (step+1.0)/2.0;
            for (int i=0; i<step; i++) {
                y += addery;

                mesh.addVertex( bst ); mesh.addColor( c );
                mesh.addVertex( mid + ofVec2f(x, y) );mesh.addColor( c );
                mesh.addVertex( mid + ofVec2f(x, y) ); mesh.addColor( c );
                mesh.addVertex( bend ); mesh.addColor( c );
            }
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
            break;
            
        // Dot
        case 6:
        {
            float space = len*0.01;
            float bh = len*0.2;
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            int step = dir.x * 0.5;
            
            ofVec3f current = bst;
            ofVec3f adder = dir / step;
            
            for (int i=0; i<step; i++) {
                current += adder;
                mesh.addVertex( current ); mesh.addColor( c );
            }
            
            if(step % 2 != 0){
                mesh.addVertex( current ); mesh.addColor( c );
            }
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
            break;
            
            
        // | | |
        case 7:
        {
            float space = len*0.23;
            float bw = dir.x - space*2;
            float bh = bw*0.15;
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            int step = bw * 0.5;
            ofVec3f current = bst;
            float adderx = bw / step;
            
            for (int i=0; i<step; i++) {
                current.x += adderx;
                float y = ofSignedNoise(i, ofGetFrameNum()*0.1) * bh/2 * 0.8;
                mesh.addVertex( current + ofVec2f(0, y) ); mesh.addColor( c );
                mesh.addVertex( current - ofVec2f(0, y) ); mesh.addColor( c );
            }
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
            break;
            
        // Wave
        case 8:
        {
            float space = len*0.1;
            float bw = len - space*2;
            float bh = len*0.15;
            bh = MIN(bh,10);
            
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            int step = bw/2;
            
            ofVec3f current = bst;
            float adderx = bw / step;
            
            float freq1 = ofRandomf()*10.0;
            float freq2 = ofRandomf()*0.5;
            
            if( ofRandomf() > 0.5){
                for (int i=0; i<step; i++) {
                    float y = (sin(i*freq1) * sin(i*freq2));
                    y = round(y) * bh/2;
                    mesh.addVertex( current ); mesh.addColor( c );
                    mesh.addVertex( current + ofVec2f(0, y) ); mesh.addColor( c );
                    current.x += adderx;
                }
            }else{
                float pasty = 0;
                for (int i=0; i<step; i++) {
                    float y = (sin(i*freq1) * sin(i*freq2))*bh/2;
                    y = round(y) * bh/2;
                    mesh.addVertex( current + ofVec2f(0, pasty) ); mesh.addColor( c );
                    mesh.addVertex( current + ofVec2f(0, y) ); mesh.addColor( c );
                    current.x += adderx;
                    pasty = y;
                }
            }
            // base lineR space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
        break;
            
        // Switch
        case 9:
        {
            float space = len*0.4;
            float bw = dir.x - space*2;
            float bh = len*0.15;
            bh = MIN(bh, 10);
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            for (int i=0; i<5; i++) {
                // noise
                float y = ofRandomf()*bh/2*1.5;
                mesh.addVertex( bst ); mesh.addColor( c );
                mesh.addVertex( bend + ofVec2f(0, y) ); mesh.addColor( c );
            }
            mesh.addVertex( bend + ofVec2f(0, bh/4) ); mesh.addColor( c );
            mesh.addVertex( bend - ofVec2f(0, bh/4) ); mesh.addColor( c );
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
            break;
            
        // =
        case 10:
        {
            float space = len*0.4;
            float bh = len*0.1;
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            // L wall
            mesh.addVertex( bst + ofVec2f(0, bh/2) ); mesh.addColor( c );
            mesh.addVertex( bst - ofVec2f(0, bh/2)); mesh.addColor( c );
            
            int step = ofRandomuf() * 10;
            
            for (int i=0; i<step+1; i++) {
                float x = 0;
                float y = -bh/2 + bh/step*i;
                mesh.addVertex( bst + ofVec2f(x, y)); mesh.addColor( c );
                mesh.addVertex( bend + ofVec2f(x, y) ); mesh.addColor( c );
            }
            
            // R wall
            mesh.addVertex( bend + ofVec2f(0, bh/2) ); mesh.addColor( c );
            mesh.addVertex( bend - ofVec2f(0, bh/2)); mesh.addColor( c );
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
            
        }
            break;
            
        // space with dot -* *-
        case 11:
        {
            float space = len*0.3;
            float bh = len*0.1;
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            mesh.addVertex( bst ); mesh.addColor( c );
            mesh.addVertex( bst + ofVec2f(2,0) ); mesh.addColor( c );

            mesh.addVertex( bend - ofVec2f(2,0)); mesh.addColor( c );
            mesh.addVertex( bend ); mesh.addColor( c );
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
            
        break;
            
        // Circle
        case 12:
        {
            float radius = len*0.3;
            int step = radius*3;
            
            float adder = TWO_PI/step;
            
            for (int i=0; i<step; i++) {
                float rad1 = i*adder;
                ofVec2f p( cos(rad1), sin(rad1) );
                mesh.addVertex( st + p*radius); mesh.addColor( c );

                float rad2 = rad1 + adder;
                ofVec2f p2( cos(rad2), sin(rad2) );
                mesh.addVertex( st + p2*radius); mesh.addColor( c );
            }
            
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
        break;
            
        // Circle 1/4
        case 13:
        {
            float radius = len*0.5;
            int step = radius*10;
            float adder = TWO_PI/step;
            float st_rad = -TWO_PI/4 * round(ofRandomuf()*4.0);
            step *= 0.25;
            
            ofVec2f p;
            for (int i=0; i<step+1; i++) {
                float rad1 = st_rad + i*adder;
                p.set( cos(rad1), sin(rad1) );
                mesh.addVertex( st + p * radius); mesh.addColor( ofFloatColor(0,1) );

                float rad2 = rad1 + adder;
                p.set( cos(rad2), sin(rad2) );
                mesh.addVertex( st + p*radius); mesh.addColor( c );
            }
            
            if( round(ofRandomuf()) < 0.4){
                mesh.addVertex( st + p * (radius) ); mesh.addColor( c );
                mesh.addVertex( st ); mesh.addColor( c );
            }
            
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
            
        }
        break;
            
        // Spike
        case 14:
        {
            float space = len*0.4;
            float bw = len - space*2;
            float bh = len;
            float minusx = dir.x < 0 ? -1: 1;
            
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            int step = bw*20;
            step += step%2;
            
            float max = step*step;
            float x, y;
            
            int minusy = ofRandomuf()>0.5 ?  1 : -1;
            for (int i=0; i<step; i++) {
                float id = i;
                x = (float)id * bw/(float)step;
                y = (float)(id*id)/max * bh;
                //x *= minusx;
                y *= minusy;
                mesh.addVertex( bst + ofVec2f(x, y) ); mesh.addColor( c );
                mesh.addVertex( bst + ofVec2f(x+1, y) ); mesh.addColor( c );
            }
            
            mesh.addVertex( bst + ofVec2f(x, y) ); mesh.addColor( c );
            mesh.addVertex( bst + ofVec2f(x, 0) ); mesh.addColor( c );
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
        break;
            
        // Random point
        case 15:
        {
            float space = len*0.3;
            float bw = len - space*2;
            float bh = len*2;
            bw = MIN(bw, 15);
            bh = MIN(bh, 38);
            ofVec3f bst = st + ofVec3f(space,0,0);
            ofVec3f bend = end - ofVec3f(space,0,0);
            
            float minus = ofRandomuf()>0.5 ? 1: -1;
            
            // L space
            mesh.addVertex( st ); mesh.addColor( c );
            mesh.addVertex( bst ); mesh.addColor( c );
            
            for (int i=0; i<len/3; i++) {
                int x = ofRandomuf() * bw;
                int y = ofRandomuf() * bh + 1;
                y *= minus;
                mesh.addVertex( bst + ofVec2f(x, y)); mesh.addColor( c );
                mesh.addVertex( bst + ofVec2f(x+2, y)); mesh.addColor( c );
            }
            
            // R space
            mesh.addVertex( bend ); mesh.addColor( c );
            mesh.addVertex( end ); mesh.addColor( c );
        }
        break;            
    }
}




