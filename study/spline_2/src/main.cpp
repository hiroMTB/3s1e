#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

int main( ){
    
    ofAppGLFWWindow window;
    window.setNumSamples( 4 );
    ofSetupOpenGL(&window, 2000, 1000, OF_WINDOW);
    ofRunApp( new ofApp() );
}
