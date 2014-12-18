//
//  ad_util.cpp
//  crater_1
//
//  Created by hiroshi matoba on 10/22/14.
//
//

#include "ad_util.h"
#include "ofMain.h"

string ad_util::data_path = "../../../../common_data/";

string ad_util::getFrameInfoString(){
    
    stringstream ss;
    ss << "fps   : " << ofGetFrameRate() << "\n";
    ss << "frame : " << ofGetFrameNum() << "\n";
    return ss.str();
}

string ad_util::getCameraInfoString( ofCamera &cam ){
    
    stringstream ss( "Camera Settings\n" );
    ss << "Position           : " << cam.getPosition() << "\n";
    ss << "Orientation(Euler) : " << cam.getOrientationEuler() << "\n";
    ss << "UpDir              : " << cam.getUpDir() << "\n";
    ss << "LookAtDir          : " << cam.getLookAtDir() << "\n";
    ss << "Fov                : " << cam.getFov() << "\n";
    ss << "LensOffset         : " << cam.getLensOffset() << "\n";
    ss << "AspectRatio        : " << cam.getAspectRatio() << "\n";
    return ss.str();
}

void ad_util::draw_axis( float length ){
    
    ofPushMatrix(); {
        
        // X axis
        ofSetColor(255, 0, 0);
        ofLine(0, 0, length, 0);
        
        // Y axis
        ofSetColor(0, 255, 0);
        ofLine(0, 0, 0, length );
        
        // Z axis
        ofSetColor(0, 0, 255);
        ofLine(0, 0, 0, 0, 0, length );
        
    } ofPopMatrix();
}

void ad_util::draw_plane( float size ){

    ofPushMatrix(); {
        ofNoFill();
        ofSetColor( 200 );
        ofSetRectMode( OF_RECTMODE_CENTER );
        ofRotate(90, 1, 0, 0);
        ofRect(0, 0, 0, size, size);
        ofSetRectMode( OF_RECTMODE_CORNER );
    } ofPopMatrix();
}