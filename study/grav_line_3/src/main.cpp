#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

int main( ){
    ofAppGLFWWindow window;
    window.setNumSamples(8);
    ofSetupOpenGL(&window, 1280, 720, OF_WINDOW);
    ofRunApp( ofApp::init());
}
