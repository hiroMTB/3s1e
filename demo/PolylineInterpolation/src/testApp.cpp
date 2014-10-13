#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {

	ofSetFrameRate( 60 );
	isDrawingFirstPoly = true;
	isSecondPolyDrawn = false;
	isInterpolating = false;
	smooth = true;

}

//--------------------------------------------------------------
void testApp::update() {

	// During the interpolation
	if( isInterpolating ) {

		poly.clear();

		int numPoints = vertices1.size();
		for( int i = 0; i < numPoints; i++ ) {

			// Compute the point between vertices1[i] and vertices2[i]
			ofPoint p = vertices1[i] * ( 1 - interpolationRatio ) + vertices2[i] * interpolationRatio;

			// Add this point to the animated polyline
			poly.addVertex( p );
		}

		poly.setClosed(true);

		if( smooth ) {
			poly = poly.getSmoothed( 4 );
		}

		// Increase the animation progression
		if( interpolationRatio < 1.f ) {
			interpolationRatio += 0.01f;
		}

	}

}

//--------------------------------------------------------------
void testApp::draw() {

	// Draw the source and the destination polylines ( with alpha during the animation )
	ofSetColor( 255, 0, 0, isInterpolating ? 50 : 255 );
	poly1.draw();
	ofSetColor( 0, 0, 255, isInterpolating ? 50 : 255 );
	poly2.draw();

	// Draw the animated polyline
	if( isInterpolating ) {

		ofSetColor(
		    ofMap( interpolationRatio, 0, 1, 255, 0 ),
		    0,
		    ofMap( interpolationRatio, 0, 1, 0, 255 )
		);
		poly.draw();

	}

	// Draw the messages
	ofSetColor( 0 );
	if( isSecondPolyDrawn ) {
		ofDrawBitmapString( "Press space to start the interpolation or 'c' to clear the polylines.", 20, 20 );
	} else if( isDrawingFirstPoly ) {
		ofDrawBitmapString( "Draw the first closed polyline", 20, 20 );
	} else {
		ofDrawBitmapString( "Draw the second closed polyline", 20, 20 );
	}
	ofDrawBitmapString( string( "Press 's' to toggle smoothing " ) + ( smooth ? "off" : "on" ), 20, 40 );

}

//--------------------------------------------------------------
void testApp::keyPressed( int key ) {

	switch( key ) {

		case 'c':
			// Clear everything
			poly1.clear();
			poly2.clear();
			isDrawingFirstPoly = true;
			isSecondPolyDrawn = false;
			isInterpolating = false;
			break;

		case 's':
			smooth = ! smooth;
			break;

		case ' ':
			// Extract 300 points from the polylines
			// I use that instead getVertices() to get the same number of points for both the polylines,
			// and to have them regularly spaced
			int numPoints = 300;
			vertices1.resize( numPoints );
			vertices2.resize( numPoints );
			for( int i = 0; i < numPoints; i++ ) {
				float percent = ( float )i / ( float )( numPoints - 1 );
				vertices1[i] = poly1.getPointAtPercent( percent );
				vertices2[i] = poly2.getPointAtPercent( percent );
			}

			// Start the animation
			isInterpolating = true;
			interpolationRatio = 0.f;
			break;

	}

}

//--------------------------------------------------------------
void testApp::keyReleased( int key ) {

}

//--------------------------------------------------------------
void testApp::mouseMoved( int x, int y ) {

}

//--------------------------------------------------------------
void testApp::mouseDragged( int x, int y, int button ) {

	// Add this point to the current drawing polyline

	if( isSecondPolyDrawn ) {
		return;
	}

	if( isDrawingFirstPoly ) {
		poly1.addVertex( x, y );
	} else {
		poly2.addVertex( x, y );
	}

}

//--------------------------------------------------------------
void testApp::mousePressed( int x, int y, int button ) {

	// Add this point to the current drawing polyline

	if( isSecondPolyDrawn ) {
		return;
	}

	if( isDrawingFirstPoly ) {
		poly1.addVertex( x, y );
	} else {
		poly2.addVertex( x, y );
	}

}

//--------------------------------------------------------------
void testApp::mouseReleased( int x, int y, int button ) {

	if( isSecondPolyDrawn ) {
		return;
	}

	// Close (and smooth) the current drawing polyline

	if( isDrawingFirstPoly ) {

		poly1.close();
		if( smooth ) {
			poly1 = poly1.getSmoothed( 4 );
		}

	} else {

		poly2.close();
		if( smooth ) {
			poly2 = poly2.getSmoothed( 4 );
		}
		isSecondPolyDrawn = true;

	}

	isDrawingFirstPoly = ! isDrawingFirstPoly;

}

//--------------------------------------------------------------
void testApp::windowResized( int w, int h ) {

}

//--------------------------------------------------------------
void testApp::gotMessage( ofMessage msg ) {

}

//--------------------------------------------------------------
void testApp::dragEvent( ofDragInfo dragInfo ) {

}
