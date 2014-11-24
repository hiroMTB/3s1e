#pragma once

#include <iostream>
#include <string>
#include <map>

#include "ofxMaxGuiWriter.h"
#include "ofxOsc.h"

#include "ofMain.h"

class ofxMaxGui{

public:
	
	ofxMaxGui(){};
	~ofxMaxGui();
	
	void setup( string file_name, float x, float y, float w, float h, int osc_receive_port=12345 );
	void write();
	void update();
	void open();
	void close();
	
	void addToggle	( string name, float x, float y, bool * toggle );
	void addInt		( string name, float x, float y, int * i );
	void addFloat	( string name, float x, float y, float * f );
	void addColor	( string name, float x, float y, ofFloatColor * color );
	void addComment ( float x, float y, float w, float h, string text);
	//void addMessage ( float x, float y, float w, float h, string text);

	template<class ListenerClass, typename ListenerMethod>
	void addBang ( string name, float x, float y, ListenerClass *listener, ListenerMethod method){
		Json::Value obj = writer.create_bpatcher(x, y, def_w, def_h, "_bang.maxpat");
		obj["box"]["args"] = name;
		writer.addObject( obj );
		bangs.insert( pair<string, ofEvent<void> > (name, ofEvent<void>()) );
		ofAddListener( bangs[name], listener, method );
	}
	
	string getCode();
	int getOscReceivePort();
private:

	static const int def_w = 350;
	static const int def_h = 20;
	static const int def_h_long = 60;
	
	int osc_receive_port;
	string file_name;
	string code;
	
	map<string, ofEvent<void> > bangs;
	map<string, bool*>	toggles;
	map<string, int*>	intValues;
	map<string, float*>	floatValues;
	map<string, ofFloatColor*> colors;
	
	ofxMaxGuiWriter writer;
	ofxOscReceiver oscr;
};


/*
	TODO
 
	Now message object does nothing.
	Nice to send string value to ofApp.
	
	1. make _string.maxpat
	2. add map<string, ofEvent<string> > strings;
	3. handle osc messages in updat loop

	Q. 
		We need callback? or jujst string pointer?
	    We should handle user push event.
 */

/*
	Noitce
	Poco::priorityEvent can not copy (private copy constructor, =operarot lock).
	So ofEvent also can not copy (actually you can but lose callback info)
	First insert ofEvent to std::map	and then call ofAddListener.

	Bad example
	ofEvent<void> event;
	ofAddListener(event, listener, method);
	myMap.insert( pair<string, ofEvent<void> > (name, event) );
*/