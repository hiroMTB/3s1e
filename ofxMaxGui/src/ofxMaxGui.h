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
        
        

        {
            //
            //  Load : _oscOut.maxpat
            //
            Json::Value box;
            box["box"]["maxclass"] = "bpatcher";
            box["box"]["presentation_rect"][0] = 45.0;
            box["box"]["presentation_rect"][1] = 30.0;
            box["box"]["presentation_rect"][2] = 150.0;
            box["box"]["presentation_rect"][3] = 90.0;
            box["box"]["numinlets"] = 0;
            box["box"]["numoutlets"] = 0;
            box["box"]["args"] = 9999;
            box["box"]["patching_rect"][0] = 45.0;
            box["box"]["patching_rect"][1] = 30.0;
            box["box"]["patching_rect"][2] = 150.0;
            box["box"]["patching_rect"][3] = 90.0;
            box["box"]["presentation"] = 1;
            box["box"]["id"] = "obj-1";
            box["box"]["name"] = "_oscOut.maxpat";
            root["patcher"]["boxes"][0] = box;
        }
        
        {
            //
            //  Load : _oscInTester.maxpat
            //
            Json::Value box;
            box["box"]["maxclass"] = "bpatcher";
            box["box"]["presentation_rect"][0] = 225.0;
            box["box"]["presentation_rect"][1] = 30.0;
            box["box"]["presentation_rect"][2] = 300.0;
            box["box"]["presentation_rect"][3] = 90.0;
            box["box"]["numinlets"] = 0;
            box["box"]["numoutlets"] = 0;
            box["box"]["args"] = 9999;
            box["box"]["patching_rect"][0] = 225.0;
            box["box"]["patching_rect"][1] = 30.0;
            box["box"]["patching_rect"][2] = 300.0;
            box["box"]["patching_rect"][3] = 90.0;
            box["box"]["presentation"] = 1;
            box["box"]["id"] = "obj-2";
            box["box"]["name"] = "_oscInTester.maxpat";
            root["patcher"]["boxes"][1] = box;
        }
        
        
        {
            //
            //  Load : _bang.maxpat
            //
            Json::Value box;
            box["box"]["maxclass"] = "bpatcher";
            box["box"]["presentation_rect"][0] = 45.0;
            box["box"]["presentation_rect"][1] = 135.0;
            box["box"]["presentation_rect"][2] = 160.0;
            box["box"]["presentation_rect"][3] = 20.0;
            box["box"]["numinlets"] = 0;
            box["box"]["numoutlets"] = 0;
            box["box"]["args"] = "bang1";
            box["box"]["patching_rect"][0] = 45.0;
            box["box"]["patching_rect"][1] = 135.0;
            box["box"]["patching_rect"][2] = 160.0;
            box["box"]["patching_rect"][3] = 20.0;
            box["box"]["presentation"] = 1;
            box["box"]["id"] = "obj-3";
            box["box"]["name"] = "_bang.maxpat";
            root["patcher"]["boxes"][2] = box;
        }
        
        {
            //
            //  Load : _toggle.maxpat
            //
            Json::Value box;
            box["box"]["maxclass"] = "bpatcher";
            box["box"]["presentation_rect"][0] = 45.0;
            box["box"]["presentation_rect"][1] = 165.0;
            box["box"]["presentation_rect"][2] = 160.0;
            box["box"]["presentation_rect"][3] = 20.0;
            box["box"]["numinlets"] = 0;
            box["box"]["numoutlets"] = 0;
            box["box"]["args"] = "toggle1";
            box["box"]["patching_rect"][0] = 45.0;
            box["box"]["patching_rect"][1] = 165.0;
            box["box"]["patching_rect"][2] = 160.0;
            box["box"]["patching_rect"][3] = 20.0;
            box["box"]["presentation"] = 1;
            box["box"]["id"] = "obj-4";
            box["box"]["name"] = "_toggle.maxpat";
            root["patcher"]["boxes"][3] = box;
        }
        
        
        {
            //
            //  Load : _int.maxpat
            //
            Json::Value box;
            box["box"]["maxclass"] = "bpatcher";
            box["box"]["presentation_rect"][0] = 45.0;
            box["box"]["presentation_rect"][1] = 195.0;
            box["box"]["presentation_rect"][2] = 350.0;
            box["box"]["presentation_rect"][3] = 20.0;
            box["box"]["numinlets"] = 0;
            box["box"]["numoutlets"] = 0;
            box["box"]["args"] = "int1";
            box["box"]["patching_rect"][0] = 45.0;
            box["box"]["patching_rect"][1] = 195.0;
            box["box"]["patching_rect"][2] = 350.0;
            box["box"]["patching_rect"][3] = 20.0;
            box["box"]["presentation"] = 1;
            box["box"]["id"] = "obj-5";
            box["box"]["name"] = "_int.maxpat";
            root["patcher"]["boxes"][4] = box;
        }
        
        {
            //
            //  Load : _float.maxpat
            //
            Json::Value box;
            box["box"]["maxclass"] = "bpatcher";
            box["box"]["presentation_rect"][0] = 45.0;
            box["box"]["presentation_rect"][1] = 225.0;
            box["box"]["presentation_rect"][2] = 350.0;
            box["box"]["presentation_rect"][3] = 20.0;
            box["box"]["numinlets"] = 0;
            box["box"]["numoutlets"] = 0;
            box["box"]["args"] = "float1";
            box["box"]["patching_rect"][0] = 45.0;
            box["box"]["patching_rect"][1] = 225.0;
            box["box"]["patching_rect"][2] = 350.0;
            box["box"]["patching_rect"][3] = 20.0;
            box["box"]["presentation"] = 1;
            box["box"]["id"] = "obj-6";
            box["box"]["name"] = "_float.maxpat";
            root["patcher"]["boxes"][5] = box;
        }
        
        {
            //
            //  Load : _color.maxpat
            //
            Json::Value box;
            box["box"]["maxclass"] = "bpatcher";
            box["box"]["presentation_rect"][0] = 45.0;
            box["box"]["presentation_rect"][1] = 255.0;
            box["box"]["presentation_rect"][2] = 350.0;
            box["box"]["presentation_rect"][3] = 60.0;
            box["box"]["numinlets"] = 0;
            box["box"]["numoutlets"] = 0;
            box["box"]["args"] = "color1";
            box["box"]["patching_rect"][0] = 45.0;
            box["box"]["patching_rect"][1] = 255.0;
            box["box"]["patching_rect"][2] = 350.0;
            box["box"]["patching_rect"][3] = 60.0;
            box["box"]["presentation"] = 1;
            box["box"]["id"] = "obj-7";
            box["box"]["name"] = "_colorf.maxpat";
            root["patcher"]["boxes"][6] = box;
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
    
    
    void make_msgBox(){
        // message box test
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
            //root["patcher"]["boxes"][i] = box;
        }
    }
};