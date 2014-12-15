#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
int main( ){

    ofAppGLFWWindow w;
    w.setNumSamples(8);
	ofSetupOpenGL(&w, 1920,1080, OF_WINDOW);
	ofRunApp( new ofApp());

}
