#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

int main( ){
    ofAppGLFWWindow window;
    window.setNumSamples(8);
    ofSetupOpenGL(&window, 1920, 1080, OF_WINDOW);
    ofRunApp( ofApp::init());
}
