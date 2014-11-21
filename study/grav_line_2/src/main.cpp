#include "ofMain.h"
#include "ofApp2.h"
#include "ofAppGlutWindow.h"
#include "ofAppGLFWWindow.h"

int main( ){
    ofAppGLFWWindow window;
    window.setDoubleBuffering( true );
    window.setNumSamples( 8 );
    window.setAlphaBits( 8 );
    ofSetupOpenGL( &window, 1920, 1080, OF_FULLSCREEN );
    ofRunApp( new ofApp2() );
}
