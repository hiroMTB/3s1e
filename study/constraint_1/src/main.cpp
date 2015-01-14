#include "ofMain.h"
#include "testApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){

    ofAppGLFWWindow window;
    window.setNumSamples( 0 );
    ofSetupOpenGL( &window, 1920, 1080, OF_WINDOW );
	ofRunApp( new testApp());

}
