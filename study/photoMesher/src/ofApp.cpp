#include "ofApp.h"
#include "ofxModifierKeys.h"
#include "ad_util.h"

void ofApp::setup(){

    bMove = false;
	bThruAccel = false;
	bThruSpeed = false;
	bDrawMesh = true;
	bDrawLineMesh = false;
    bBaseMove = false;
    bMakeHole = false;
    
    res = 8;
    extrusion = 300;
	friction = 0.99;
    threthold = 8;
	animSpeed = 1;
	
	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
    bg.set(0);
    
    camDist = 7800;
    cam.setPosition(0, 0, camDist);
    cam.lookAt(ofVec3f(0,0,0), ofVec3f(0,1,0));
    cam.setFov( 15 );
    
    img.loadImage( "img/losglaciares12.jpg");
    load_mesh( img );
    int nw = img.getWidth() / 2;
    int nh = img.getHeight() / 2;

    gn.setup();
    gn.create( nw, nh );
    gn.setShaderType( ofxGpuNoise::SHADER_TYPE_Cellular );
    gn.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn.setFreq( 0.004 );
    gn.setOctaves( 4 );

	gn2.setup();
    gn2.create( nw, nh );
    gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
	gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
	gn2.setFreq( 0.0005 );
	gn2.setOctaves( 2 );
	
    gn3.create( nw, nh );
    gn3.setFreq( 0.007 );
    gn3.setOctaves( 16 );
    gn3.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn3.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn3.setFrame(0.1);
    gn3.update();
    
	// gui
    {
//        int y = 100; int offsety = 30;
//        gui.setup( 12345, 5555 );
//        gui.addPatcher(0, 0);
//        gui.addOscOut(45, 10);
//        gui.addToggle("draw_mesh", 45, y+=offsety, &bDrawMesh);
//        gui.addToggle("draw_line_mesh", 45, y+=offsety, &bDrawLineMesh);
//
//        gui.addToggle("thruAccel", 45, y+=offsety, &bThruAccel);
//
//        gui.addFloat("extrusion", 45, y+=offsety, &extrusion);
//        gui.addFloat("friction", 45, y+=offsety, &friction);
//        gui.addFloat("speed", 45, y+=offsety,  &animSpeed);
//        gui.addColor("background", 45, y+=offsety, &bg);
//        gui.write( "_gui_main.maxpat" );
//        gui.open( "_gui_main.maxpat" );
    }
    
    //svg.load( ad_util::data_path + "svg/full.svg" );
    //int w = svg.getWidth() + 1;
    //int h = svg.getHeight() + 1;
    int w = 7909;  // 7908.01;
    int h = 4018;  //4017.48;
    exporter.setup( w, h, 25, GL_RGB, 4);
    //exporter.setup(1920, 1080, 25, GL_RGB, 4);
    exporter.setFilePattern( ofGetTimestampString() + "/frame_%05i." + "tif" );
    exporter.setAutoExit(true);
    exporter.setFrameRange(1,3000);
    
    ofSetWindowPosition(0, 0);
    ofSetWindowShape( exporter.getFbo().getWidth()/4, exporter.getFbo().getHeight()/4);
}

void ofApp::dragEvent( ofDragInfo info ){
    if( info.files.size() > 0 ){
        img.clear();
        img.loadImage(info.files[0]);
        load_mesh( img );
    }
}

void ofApp::load_mesh( ofFloatImage &img ){
    
    mesh.clear();
    mesh.setUsage( GL_DYNAMIC_DRAW );
    mesh.setMode( OF_PRIMITIVE_TRIANGLES );

	lines.clear();
	lines.setUsage( GL_DYNAMIC_DRAW );
	lines.setMode( OF_PRIMITIVE_LINES);

    hole_factors.clear();
    
    int ch = img.getPixelsRef().getNumChannels();
    mW = img.getWidth() / res;
    mH = img.getHeight() / res;
	
	float startx = -img.getWidth()*0.5;
	float starty = -img.getHeight()*0.5;
	
    int index = 0;
    for( int y=0; y<mH; y++ ){
        for( int x=0; x<mW; x++ ){
            
            ofVec2f sampling_point( x*res, y*res );
            ofFloatColor c = img.getPixelsRef().getColor( sampling_point.x, sampling_point.y );
            c.a = 0.7;
            float z = c.getBrightness() * extrusion;
			ofVec3f pos( startx + sampling_point.x, starty + sampling_point.y, z);

			mesh.addVertex( pos  );
            mesh.addColor( c );
			
			// create Line mesh
            // avoid moare
            lines.addVertex( pos );
            pos.y += ofSignedNoise(x*0.001) * 20.0;
            pos.x += ofSignedNoise(y*0.001) * 20.0;
            pos.z *= 0.9;
            //pos.z += ofSignedNoise(index*0.001) * 30.0;
            lines.addVertex( pos );
			
			lines.addColor( c );
			lines.addColor( c );
			
            hole_factors.push_back(0);
			index++;
        }
    }
	
    for( int y=0; y<mH-1; y++ ){
        for( int x=0; x<mW-1; x++ ){
            mesh.addIndex(x+y*mW);				// 0
            mesh.addIndex((x+1)+y*mW);			// 1
            mesh.addIndex(x+(y+1)*mW);			// 10

            mesh.addIndex((x+1)+y*mW);			// 1
            mesh.addIndex((x+1)+(y+1)*mW);		// 11
            mesh.addIndex(x+(y+1)*mW);			// 10
        }
    }
    
    speed.assign(mW*mH, ofVec3f(0,0,0) );
    accel.assign(mW*mH, ofVec3f(0,0,0) );
 
    speed_b.assign(mW*mH, ofVec3f(0,0,0) );
    accel_b.assign(mW*mH, ofVec3f(0,0,0) );
}

void ofApp::update(){
	//if( !exporter.isExporting() )
     //   gui.update();
	
    if( !bMove )
        return;

    gn.setFrame( ofGetFrameNum()*0.02 * animSpeed );
    gn.update();
    
    gn2.setFrame( ofGetFrameNum()*0.01 * animSpeed );
    gn2.update();
    
    gn3.setFrame( ofGetFrameNum() * 0.002 );
    gn3.update();
    
    int n = mesh.getNumVertices();
    if( 0 >= n )
        return;
    
    vector<ofVec3f> &vs = mesh.getVertices();
    vector<ofFloatColor> &cols = mesh.getColors();
    
    vector<ofVec3f> &lvs = lines.getVertices();
    vector<ofFloatColor> &lcols = lines.getColors();
    
    for( int y=0; y<mH; y++){
        for( int x=0; x<mW; x++){
            
            int index = x + y*mW;
            float rate = 1;
            
            float r = cols[index].r;
            float g = cols[index].g;
            float b = cols[index].b;
                
            rate = (r+g+b) * 0.333;
            

            float nR1 = gn.getNoisef( x, y, 0);
            float nG1 = gn.getNoisef( x, y, 1);
            float nB1 = gn.getNoisef( x, y, 2);
            
            float nR2 = gn2.getNoisef( x, y, 0);
            float nG2 = gn2.getNoisef( x, y, 1);
            float nB2 = gn2.getNoisef( x, y, 2);
            
            {
                float nR = nR1*nR2;
                float nG = nG1*nG2;
                float nB = nB1*nB2;
                
                if( !bThruAccel ){
                    accel[index].x = nR * rate * 0.2;
                    accel[index].y = nG * rate * 0.2;
                    accel[index].z = nB * rate * 0.1;
                    speed[index] += accel[index];
                }else{
                    speed[index].x = nR * rate * 100.0;
                    speed[index].y = nG * rate * 100.0;
                    speed[index].z = nB * rate * 100.0;
                }
                
                speed[index] *= friction;
                
                vs[index] += speed[index];
                
                
                if( abs(vs[index].z) > extrusion*5 ){
                    if( vs[index].z < 0 )
                        vs[index].z = -extrusion*5;
                    else
                        vs[index].z = extrusion*5;
                    
                    accel[index] *= 0.8;
                    speed[index] *= 0.8;
                }
                
                //vs[index] += ofVec3f( ofRandomf(),ofRandomf(),ofRandomf() );
                
                lvs[index*2] = vs[index];
            }
            
            
            if( bBaseMove ){
                
                float nR = nR1*nB2;
                float nG = nG1*nR2;
                float nB = nB1*nG2;
                
                //speed_b[index].x += nR * rate * 0.01;
                //speed_b[index].y += nG * rate * 0.01;
                //speed_b[index].z += nB * rate * 0.001;
                //lvs[index*2 + 1] += speed_b[index];
                lvs[index*2 + 1] = vs[index];
                lvs[index*2 + 1].z *= 0.6;
            }
            
            {
                float nR = nR1*nG2;
                float nG = nG1*nB2;
                float nB = nB1*nR2;
                
                rate *= 0.005;
                cols[index].r += (nR * rate);
                cols[index].g += (nG * rate);
                cols[index].b += (nB * rate);
                cols[index].a += (nR+nG+nB) * rate * 0.333 * 0.5;
                
                ofFloatColor lc =  ofFloatColor(1) - cols[index];
                lc.a *= 0.7;
                lcols[index*2] = lc;
                lcols[index*2+1] = lc;
            }
        }
    }
    
    set_indices();
}

void ofApp::set_indices(){
    
   if( bMakeHole ){
       
       mesh.clearIndices();
       vector<ofVec3f> &vs = mesh.getVertices();
       vector<ofFloatColor> &cols = mesh.getColors();
       
       int max_index = mW * mH -1;
       
       for( int y=0; y<mH; y++){
           for( int x=0; x<mW; x++){
               
               int index = x + y*mW;
               
               float hole = gn3.getNoiseuf(x,y,0);
               hole_factors[index] += hole + ofRandomuf()*0.22;

               float limit = 100+60*ofNoise(index);
               if( hole_factors[index] < limit ){
                   
                   if( x<mW-1 && y<mH-1){
                       mesh.addIndex( index );
                       mesh.addIndex( index+1 );
                       mesh.addIndex( index+mW );
                       
                       mesh.addIndex( index+1 );
                       mesh.addIndex( index+mW+1 );
                       mesh.addIndex( index+mW );
                   }
               }else{
                   
                   float exp = 6;
                   
                   vs[index].x += ofRandomf() * 6;
                   vs[index].y += ofRandomf() * 6;
                   vs[index].z += ofRandomuf() * exp/2;

                   if( index+1<=max_index ) vs[index+1].z += exp*2;
                   if( index+2<=max_index ) vs[index+2].z += exp;
                   if( index+3<=max_index ) vs[index+3].z += exp;
                   if( index+4<=max_index ) vs[index+4].z += exp/2;
                   
                   if( index-1>=0) vs[index-1].z += exp*3;
                   if( index-2>=0) vs[index-2].z += exp;
                   if( index-3>=0) vs[index-3].z += exp/2;
                   if( index-4>=0) vs[index-4].z += exp/4;
                   
                   if( index+mW<=max_index )    vs[index+mW].z += exp*2;
                   if( index+mW+1<=max_index )  vs[index+mW+1].z += exp;
                   if( index+mW+2<=max_index )  vs[index+mW-2].z += exp/2;
                   if( index+mW+3<=max_index )  vs[index+mW-3].z += exp/4;
                   
                   if( index+mW-1<=max_index )  vs[index+mW-1].z += exp;
                   if( index+mW-2<=max_index )  vs[index+mW-2].z += exp/2;
                   if( index+mW-3<=max_index )  vs[index+mW-3].z += exp/4;
                   
                   if( index-mW>=0 )    vs[index-mW].z += exp*2;
                   if( index-mW+1>=0 )  vs[index-mW+1].z += exp;
                   if( index-mW+2>=0 )  vs[index-mW+2].z += exp/2;
                   if( index-mW+3>=0 )  vs[index-mW+3].z += exp/4;
                   
                   if( index-mW-1>=0 )  vs[index-mW-1].z += exp;
                   if( index-mW-2>=0 )  vs[index-mW-2].z += exp/2;
                   if( index-mW-3>=0 )  vs[index-mW-3].z += exp/4;
                   cols[index].a    -= 0.05;

                   float red_rate = 1.0003;
                   if( index+1<=max_index ) cols[index+1].r  *= red_rate;
                   if( index+2<=max_index ) cols[index+2].r  *= red_rate;
                   if( index+3<=max_index ) cols[index+3].r  *= red_rate;
                   
                   if( index-1>=0 )         cols[index-1].r  += red_rate;
                   if( index-2>=0 )         cols[index-2].r  += red_rate;
                   if( index-3>=0 )         cols[index-3].r  += red_rate;
                   
                   if( index+mW>=max_index )    cols[index+mW].r += red_rate;
                   if( index+mW+1>=max_index )  cols[index+mW+1].r += red_rate;
                   if( index+mW+2>=max_index )  cols[index+mW+2].r += red_rate;
                   if( index+mW+3>=max_index )  cols[index+mW+3].r += red_rate;

                   if( index-mW>=0 )            cols[index-mW].r += red_rate;
                   if( index-mW-1>=0 )          cols[index-mW-1].r += red_rate;
                   if( index-mW-2>=0 )          cols[index-mW-2].r += red_rate;
                   if( index-mW-3>=0 )          cols[index-mW-3].r += red_rate;

               }
           }
       }
   }
}

void ofApp::draw(){
    
    ofPushMatrix(); {
            exporter.begin( cam ); {
                ofEnableDepthTest();
                ofEnableAlphaBlending();
                ofEnableSmoothing();
                ofBackground( bg );
                
                // cam.enableOrtho();
                //ofTranslate( -w/2, h/2 );
                //ofScale( 1, -1, -1);
                
                if( bDrawLineMesh ){
                    glPointSize( 1 );
                    glLineWidth( 1 );

                    lines.setMode( OF_PRIMITIVE_LINES);
                    lines.draw();
                    lines.setMode( OF_PRIMITIVE_POINTS);
                    lines.draw();
                }

                if( bDrawMesh ){
                    glPointSize( 1 );
                    glLineWidth( 1 );
                    if( mesh.getNumIndices() != 0)
                        mesh.drawWireframe();
                    mesh.drawVertices();
                }
                
                // guide
                if( !exporter.isExporting() ){
                    ofSetColor(255, 0, 0);
                    ofLine(0, 0, 300, 0);

                    ofSetColor(0, 255, 0);
                    ofLine(0, 0, 0, 300);

                    ofSetColor(0, 0, 255);
                    ofLine( ofVec3f(0,0,0), ofVec3f(0,0,300) );
                }
                
            } exporter.end();
        
    } ofPopMatrix();
    
    if( !exporter.isExporting() ){

        ofBackground( 0 );
        ofPushMatrix();
        //float s = ofGetWidth() / svg.getWidth();
        //ofScale(s, s);
        //ofTranslate( svg.getWidth()/2, svg.getHeight()/2 );
        //svg.draw();
        //ofPopMatrix();
        
        gn.draw( ofGetWidth()-200,  100, 0.05);
        gn2.draw( ofGetWidth()-200, 250, 0.05);
        gn3.draw( ofGetWidth()-200, 400, 0.05);

        ofPushMatrix();{
            ofSetColor( 255 );
            exporter.draw(0, 0);
        }ofPopMatrix();
    }
    
    stringstream ss;
	ss << "cam\n";
	ss << "pos        : " << cam.getPosition() << "\n";
	ss << "distance   : " << cam.getDistance() << "\n";
	ss << "lookAt dir : " << cam.getLookAtDir() << "\n";
	ss << "Up dir     : " << cam.getUpDir() << "\n";
    ss << "\n";
    ss << "gn " << gn.getNoiseParamString() << "\n";
    ss << "gn2 " << gn2.getNoiseParamString() << "\n";
    ss << "\n";
    ss << "exporter\n";
    ss << "export enable: " << exporter.isExporting() << "\n";
    ss << "export frame" << exporter.getFrameNum() << "\n";
    ss << "\n";
    ss << "Mesh Resolution" << res << "\n";
	ofSetColor( 255 );
	ofDrawBitmapString(ss.str(), 20, 20);
}

void ofApp::keyPressed( int key ){

    bool needUpdate = false;

    bool shift = ofGetModifierPressed( OF_KEY_SHIFT );
    bool alt = ofGetModifierPressed( OF_KEY_ALT );

    if( shift ){
        switch(key) {
            case OF_KEY_UP:
                extrusion+=10;
                needUpdate = true;
                break;
            case OF_KEY_DOWN:
                extrusion-=10;
                needUpdate = true;
                break;
            case OF_KEY_RIGHT:
                res++;
                needUpdate = true;
                break;
            case OF_KEY_LEFT:
                res--;
                res = max(res, 1);
                needUpdate = true;
                break;
                
            case 'S':
                ofSetWindowShape( 300, 500 );
                ofSetWindowPosition(0, 0);
                bMove = true;
                exporter.startExport();
                break;
                
            default:
                break;
        }
    }else{
        switch(key){
		
			case '1':
				cam.setPosition(camDist, 0, 0);
				cam.lookAt(ofVec3f(0,0,0), ofVec3f(0,1,0) );
				break;

			case '2':
				cam.setPosition(0, camDist, 0);
				cam.lookAt(ofVec3f(0,0,0), ofVec3f(0,0,1));
				break;
			
			case '3':
				cam.setPosition(0, 0, camDist);
				cam.lookAt(ofVec3f(0,0,0), ofVec3f(0,1,0));
				break;
				
			case 'l':
				bDrawLineMesh = ! bDrawLineMesh;
				break;

            case 'm':
                bDrawMesh = !bDrawMesh;
                break;
				
            case 'b':
                bBaseMove = !bBaseMove;
                break;
                
            case 'f':
                ofToggleFullscreen();
                break;
                
            case OF_KEY_RIGHT:
                {
                    int i = gn.getShaderType();
                    gn.setShaderType((ofxGpuNoise::ShaderType)++i);
                    break;
                }
            case OF_KEY_LEFT:
                {
                    int i = gn.getShaderType();
                    gn.setShaderType((ofxGpuNoise::ShaderType)--i);
                    break;
                }
                
            case OF_KEY_UP:
                gn.setFreq( gn.getFreq()*2.0 );
                break;
                
            case OF_KEY_DOWN:
                gn.setFreq( gn.getFreq()*0.5 );
                break;
                
            case 'd':
                {
                    ofxGpuNoise::ShaderDerivType deriv = gn.getShaderDerivType();
                    gn.setShaderDerivType( deriv==0 ? ofxGpuNoise::SHADER_DERIV_TYPE_YES : ofxGpuNoise::SHADER_DERIV_TYPE_NO );
                }
            case ' ':
                bMove = !bMove;
				break;
				
			case 'u':
				load_mesh( img );
                break;

                
            case 'h':
                bMakeHole = !bMakeHole;
                break;
    
        }
    }
    
    if( needUpdate )
        load_mesh( img );
}
