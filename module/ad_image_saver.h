//
//  ad_image_saver.h
//  line_variation
//
//  Created by hiroshi matoba on 10/9/14.
//
//

#pragma once

#include "ofMain.h"

//#ifdef AD_IMAGE_SAVER_USE_FBO

class ad_image_saver{
    
public:
    ad_image_saver();
    void start( string dir_name, string file_prefix, int frame_end, string file_extention=".tiff" );
    void save();
    
#ifdef AD_IMAGE_SAVER_USE_FBO
    void begin();
    void end();
    void draw_fbo();
    ofFbo fbo;
#endif

    bool bStart;
    int frame_cur;
    int frame_end;
    
    string file_prefix;
    string file_extention;
    string dir_name;
    ofImage img;
    ofDirectory dir;
};

