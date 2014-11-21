//
//  ofApp2.h
//  grav_line_2
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#pragma once

#include "ofMain.h"
#include "ad_grav_line.h"
#include "ad_image_saver.h"

class ofApp2 : public ofBaseApp{
    
public:
    void setup(){
        bDraw_info = true;
        cam.setDistance(1000);
        cam.setLensOffset( ofVec2f(0, 1.2) );
        gravline.setup( &cam );
        
        ofVec2f p0(0,0);
        ofVec2f p1(-900,1000);
        ofVec2f p2(-100,800);
        ofVec2f p3(600,1200);
        ofVec2f p4(800,600);
        
        gravline.create_line(p0, p1);
        gravline.create_line(p0, p2);
        gravline.create_line(p0, p3);
        gravline.create_line(p0, p4);

        gravline.create_line(p1, p2);
        gravline.create_line(p2, p3);
        gravline.create_line(p3, p4);

        ofBackground(0);
    };
    
    void update(){
        gravline.update();
    }
    
    void draw(){
        ofEnableAlphaBlending();
        ofEnableAntiAliasing();
        ofEnableSmoothing();
        ofBackground( 255 );
        cam.begin();
        
        gravline.draw();
        cam.end();

        saver.save();
        draw_info();
    }

    void draw_info(){
        if ( !bDraw_info)
            return;
        
        stringstream ss;
        ss << "fps: " << ofToString( ofGetFrameRate(),0 ) << "\n";
        ss << "frame     : " << ofGetFrameNum() << "\n";
        ss << "cur frame : " << saver.frame_cur << "\n";
        ss << "end frame : " << saver.frame_end << "\n";
        ss << "resolution: " << ofGetWidth() << ", " << ofGetHeight() << "\n" << "\n";
//        ss << "num shapes: " << shapes.size() << "\n";
        ss << "I key:   toggle info " << "\n";
        ss << "\nCam\n";
        ss << "pos       : " << cam.getPosition().x << ", " << cam.getPosition().y << "," << cam.getPosition().z << "\n";
        ss << "target pos: " << cam.getTarget().getPosition().x << ", " << cam.getTarget().getPosition().y << "," << cam.getTarget().getPosition().z << "\n";
        ss << "dist      : " << cam.getDistance() << "\n";
        ss << "fov       : " << cam.getFov() << "\n";
        ss << "\n";

        ofSetColor( 0);
        ofDrawBitmapString( ss.str().c_str(), 10, 10 );
    };
    
    void keyPressed( int key ){
        switch( key ) {
            case 'f':
                ofToggleFullscreen();
                break;
            case 'S':
                saver.start(ofGetTimestampString(), "", 3000);
                break;
        };
    }
    
    bool bDraw_info;
    float frame;
    
    ofEasyCam cam;
    ad_grav_line gravline;

    ad_image_saver saver;
};
