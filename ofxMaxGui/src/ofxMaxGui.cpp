#include "ofxMaxGui.h"
#include "ofxJSON.h"

void ofxMaxGui::setup( string _file_name, float x, float y, float w, float h, int _osc_receive_port ){
	file_name = _file_name;
	osc_receive_port = _osc_receive_port;
	writer.create_patcher(x, y, w, h);
	Json::Value out = writer.create_bpatcher(45, 30, 150, 20, "_oscOut.maxpat" );
	out["box"]["args"] = osc_receive_port;
	writer.addObject( out );
	oscr.setup(osc_receive_port);
}

void ofxMaxGui::write(){
	code = writer.write(file_name);
}

void ofxMaxGui::addToggle ( string name, float x, float y, bool * toggle ){
	Json::Value obj = writer.create_bpatcher(x, y, def_w, def_h, "_toggle.maxpat");
	obj["box"]["args"] = name;
	writer.addObject( obj );
	toggles.insert(pair<string, bool*>(name, toggle));
}

void ofxMaxGui::addInt ( string name, float x, float y, int * i ){
	Json::Value obj = writer.create_bpatcher(x, y, def_w, def_h, "_int.maxpat");
	obj["box"]["args"] = name;
	writer.addObject( obj );
	intValues.insert(pair<string, int*>(name, i));
}

void ofxMaxGui::addFloat ( string name, float x, float y, float * f ){
	Json::Value obj = writer.create_bpatcher(x, y, def_w, def_h, "_float.maxpat");
	obj["box"]["args"] = name;
	writer.addObject( obj );
	floatValues.insert(pair<string, float*>(name, f));
}

void ofxMaxGui::addColor ( string name, float x, float y, ofFloatColor * color ){
	Json::Value obj = writer.create_bpatcher(x, y, def_w, def_h_long, "_colorf.maxpat");
	obj["box"]["args"] = name;
	writer.addObject( obj );
	colors.insert(pair<string, ofFloatColor*>(name, color));
}

void ofxMaxGui::addComment(float x, float y, float w, float h, string text){
	Json::Value obj = writer.create_comment(x, y, w, h, text);
	writer.addObject(obj);
}

/*void ofxMaxGui::addMessage(float x, float y, float w, float h, string text){
	Json::Value obj = writer.create_message(x, y, w, h, text);
	writer.addObject(obj);
}*/

void ofxMaxGui::update(){
	while ( oscr.hasWaitingMessages() ) {
		ofxOscMessage m;
		if( oscr.getNextMessage(&m) ){
			vector<string> adrs = ofSplitString(m.getAddress(), "/");

			/*
			 *	OSC message format
			 *	adrs[0] = ""
			 *	adrs[1] = fromMax
			 *	adrs[2] = bang/toggle/int/float/color
			 *	adrs[3] = name
			 */
			string &top = adrs[1];

			if( top == "fromMax"){
				string &type = adrs[2];
				string &name = adrs[3];
				
				if( type == "bang" ){
					ofNotifyEvent( bangs[name] );
				}else if( type == "toggle" ){
					*toggles[name] = (bool)m.getArgAsInt32(0);
				}else if( type == "int" ){
					*intValues[name] = m.getArgAsInt32(0);
				}else if( type== "float" ){
					*floatValues[name] = m.getArgAsFloat(0);
				}else if( type == "color"){
					colors[name]->r = m.getArgAsFloat(0);
					colors[name]->g = m.getArgAsFloat(1);
					colors[name]->b = m.getArgAsFloat(2);
				}else{
					ofLogNotice("strange Osc msg type : " + type);
				}
			}else{
				ofLogNotice("strange Osc msg: top address : " + top);
			}
		}
	}
}

void ofxMaxGui::open(){
	ofFile file;
	file.open( file_name, ofFile::ReadOnly );
	
	if( file.isFile() ){
		string command = "open " + ofToDataPath( file.getFileName(), true );
		system( command.c_str() );
	}
}

void ofxMaxGui::close(){
	ofFile file;
	file.open( file_name, ofFile::ReadOnly );
	
	if( file.isFile() ){
		string command = "close " + ofToDataPath( file.getFileName(), true );
		system( command.c_str() );
	}
}

string ofxMaxGui::getCode(){
	return code;
}

int ofxMaxGui::getOscReceivePort(){
	return osc_receive_port;
}

ofxMaxGui::~ofxMaxGui(){
	
	map<string, ofEvent<void> >::iterator itr = bangs.begin();
	for(; itr!=bangs.end(); itr++){
		//ofRemoveListener( itr->second, ?, ?);
	}
}
