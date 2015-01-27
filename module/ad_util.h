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
	
	static void push_notification( string title, string msg );
    
    // does not work
    // info.plist is set when we compile.
/*    static void get_app_name(){
        NSString *ns_appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
        NSString *ns_appName = [entry objectForKey:(id)kCGWindowOwnerName];
        appName = [ns_appName UTF8String];
        
        string appName = ofToDataPath("", true);
        vector<string> pv = ofSplitString(appName, "/");
        pv[pv.size()-1 - ]
        
        cout << "App Name : " << appName << endl;
    };
    */
};

