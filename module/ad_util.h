//
//  ad_util.h
//  crater_1
//
//  Created by hiroshi matoba on 10/22/14.
//
//

#pragma once

#include "ofMain.h"

class ad_util{
    
public:

    ad_util(){};
    
    static string data_path;
    
    static string getFrameInfoString();
    
    static string getCameraInfoString( ofCamera &cam );

    static void draw_axis( float length=100 );
    
    static void draw_plane( float size= 100 );
};

