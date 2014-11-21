//
//  ofxMaxGui.h
//  ofxMaxGuiSample
//
//  Created by hiroshi matoba on 10/2/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxJSON.h"

class ofxMaxGui{
    
public:
    ofxMaxGui(){};
    ~ofxMaxGui(){};
    
    string file_name;
    
    string create( string _file_name){
        file_name = _file_name;
        
        Json::Value root;
        root["patcher"]["fileversion"] = 1;
        root["patcher"]["appversion"]["major"] = 5;
        root["patcher"]["appversion"]["minor"] = 1;
        root["patcher"]["appversion"]["revision"] = 9;

        root["patcher"]["rect"][0] = 1280.0;
        root["patcher"]["rect"][1] = 44.0;
        root["patcher"]["rect"][2] = 1280.0;
        root["patcher"]["rect"][3] = 1366.0;
        
        for( int i=0; i<100; i++ ){
            Json::Value box;
            box["box"]["maxclass"] = "message";
            box["box"]["text"] = "num " + ofToString( i );
            box["box"]["numinlets"] = 2;
            box["box"]["numoutlets"] = 1;
            box["box"]["patching_rect"][0] = 210.0 + i%10*100;
            box["box"]["patching_rect"][1] = 210.0 + i/10*100;
            box["box"]["patching_rect"][2] = 50.0;
            box["box"]["patching_rect"][3] = 18.0;
            box["box"]["id"] = "obj-1";
            box["box"]["fontname"] = "Arial";
            box["box"]["outlettype"] = "";
            box["box"]["fontsize"] = 12;
            root["patcher"]["boxes"][i] = box;
        }
        
        /*
         *  write
         */
        ofFile file;
        Json::StyledWriter writer;
        file.open( file_name, ofFile::WriteOnly );
        file << writer.write( root ) << endl;
        file.close();
        
        /*
         *      get result string
         */
        ofFile reader( file_name );
        ofBuffer buf = reader.readToBuffer();
        string code = buf.getText();
        reader.close();
        
        return code;
    }
    
    void open(){
        ofFile file;
        file.open( file_name, ofFile::ReadOnly );

        if( file.isFile() ){
            string command = "open " + ofToDataPath( file.getFileName(), true );
            system( command.c_str() );
        }
    }
};