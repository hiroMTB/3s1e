//
//  ofApp.cpp
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#include "ofApp.h"
#include "ad_util.h"

bool ofApp::bRender = true;
float ofApp::density = 0;

ofApp * ofApp::app = NULL;

void ofApp::setup(){
    
    mode = 0;
    bDraw_info = true;
    bStart = true;
    ofSetCircleResolution( 4 );
    ofSetVerticalSync( true );
    
    // ABC / KLM
    //grav_wall.impulse = -1.2;
    //density = 1.3;

    // DEF / HIJ
    grav_wall.impulse = -1.2;
    density = 1.1;

    // G
    //grav_wall.impulse = -4.5;
    //density = 3.3;
    
    load_noise();
    img.loadImage( ad_util::data_path +  "img/rk/Mx80_2_org.jpg");
    grav_wall.setup( &img );
    
    load_svg("");
    setup_window( win.x, win.y);
    
}

void ofApp::update(){
    
    gpu_noise.addFrame( 0.03 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    grav_wall.update();
    

    /*
     * 
     *  TIME LINE
     *
     */
    frame = exps[0].isExporting() ? exps[0].getFrameNum() : ofGetFrameNum();

    int modep = mode;
    if( frame < 50 ){
        mode = 0;
    }else if( frame < 750 ){
        mode = 1;
    }else if( frame < 1000 ){
        mode = 2;
    }
    
    bool changed = mode != modep;
    
    if( changed ){
        if( mode == 1 ){
            
            // Collision ON
            grav_wall.toggleCollision( 0.99 );
            
        }else if( mode == 2){
            
            // change grav dir
            grav_wall.toggleCollision( 0.15 );
            grav_wall.setGrav( -grav_wall.impulse );
            grav_wall.reset_attr();
            ad_grav_wall::gravType = 1;
        }
    }
    
    if( mode >= 2 ){
        grav_wall.change_attr();
    }
    
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofDisableAntiAliasing();
    ofDisableSmoothing();

    exps[0].begin();{
        ofBackground( 255 );
        grav_wall.draw_lines();
        //mask.draw(0, 0);
        
    }exps[0].end();

    exps[1].begin();{
        ofBackground( 255 );
        grav_wall.draw_points();
        mask.draw(0, 0);
    }exps[1].end();
    
    ofPushMatrix();{
        ofBackground(255);
        ofSetColor(255);
        exps[0].draw(0, 0);

        //exps[1].draw(0, 0);

        ofSetColor(155);
        draw_info();
        gpu_noise.draw(300, 10, 0.2);
    }ofPopMatrix();
}
