#include "ofMain.h"
#include "ofAppGLFWWindow.h"
#include "ofApp.hpp"

int main(){
    //ofPtr<ofBaseRenderer> renderer(new ofGLProgrammableRenderer(false));
    //ofSetCurrentRenderer(renderer, false);
    //ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup
    
    
    ofAppGLFWWindow window;
    window.setNumSamples( 0 );
    ofSetupOpenGL( &window, 1920,1080, OF_WINDOW);
    ofRunApp( new ofApp() );
}
