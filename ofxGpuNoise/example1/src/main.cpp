#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
//========================================================================
int main( ){

    ofAppGLFWWindow window;
    window.setNumSamples( 8 );
    window.setVerticalSync( true );
    window.setDoubleBuffering( true );
	ofSetupOpenGL( &window, 1920, 1080, OF_WINDOW );
    //ofSetupOpenGL( 1920, 1080, OF_WINDOW );
	ofRunApp( new ofApp());
}
