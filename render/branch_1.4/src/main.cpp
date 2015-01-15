#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

int main( ){

    ofAppGLFWWindow window;
    window.setNumSamples( 4 );
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);
    ofApp::init();
    ofRunApp( ofApp::app );
}
