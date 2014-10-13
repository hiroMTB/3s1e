//
//  ad_image_saver.cpp
//  line_variation
//
//  Created by hiroshi matoba on 10/9/14.
//
//

#include "ad_image_saver.h"

ad_image_saver::ad_image_saver()
:
frame_cur( 0 ),
frame_end( 0 ),
bStart( false ){
    
}

void ad_image_saver::start( string _dir_name, string _file_prefix, int _frame_end, string _file_extention ){
    
    dir_name = _dir_name;
    file_prefix = _file_prefix;
    frame_end = _frame_end;
    file_extention = _file_extention;
    
    frame_cur = 0;
    bStart = true;
    
#ifdef AD_IMAGE_SAVER_USE_FBO
    fbo.allocate( ofGetWidth(), ofGetHeight(), GL_RGBA );
#endif
    
    cout << "<<<" << endl;
    cout << "<<< image saver start at : " + ofGetTimestampString() << endl;
    cout << "<<<" << endl;
}

void ad_image_saver::save(){
    
    if( !bStart ){
        return;
    }
    
    string file_name = file_prefix + ofToString( frame_cur, 0, 6, '0' ) + file_extention;
    
    img.clear();
    img.grabScreen( 0, 0, ofGetWidth(), ofGetHeight() );
    img.saveImage( dir_name + "/" + file_name );
    
    if( frame_cur < frame_end ){
        frame_cur++;
    }else{
        cout << "///" << endl;
        cout << "/// image saver end at : " + ofGetTimestampString() << endl;
        cout << "///" << endl;
        bStart = false;
    }
}

#ifdef AD_IMAGE_SAVER_USE_FBO

void ad_image_saver::begin(){
    if( !bStart ){
        return;
    }
    
    fbo.begin();
}

void ad_image_saver::end(){
    
    if( !bStart ){
        return;
    }
    
    fbo.end();
    
    ofPixels& pix = img.getPixelsRef();
    fbo.readToPixels( pix );
    
    string file_name = file_prefix + ofToString( frame_cur, 0, 6, '0' ) + file_extention;
    img.saveImage( dir_name + "/" + file_name );
    
    if( frame_cur < frame_end ){
        frame_cur++;
    }else{
        cout << "///" << endl;
        cout << "/// image saver[FBO] end at : " + ofGetTimestampString() << endl;
        cout << "///" << endl;
        bStart = false;
    }
    
}

void ad_image_saver::draw_fbo(){
    ofSetColor( 255 );
    fbo.draw( 0, 0 );
}

#endif