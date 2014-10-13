#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofPolyline poly1;             // Source polyline
		ofPolyline poly2;             // Destination polyline
		ofPolyline poly;              // Intermediate polyline
		vector< ofPoint > vertices1;  // Points extracted from the source polyline
		vector< ofPoint > vertices2;  // Points extracted from the destination polyline

		bool isDrawingFirstPoly;      // True when drawing the source polyline
		bool isSecondPolyDrawn;       // True after the destination polyline was drawn
		bool isInterpolating;         // True during the morphing animation
		float interpolationRatio;     // From 0 to 1 during the animation
		bool smooth;                  // Smooth the polylines

};
