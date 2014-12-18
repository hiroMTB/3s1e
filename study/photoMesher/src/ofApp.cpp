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
    bTriangleTess = false;
    
    frame = -1;
    res = 8;
    extrusion = 300;
	friction = 0.99;
    threthold = 8;
	animSpeed = 1;
	
	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
    bg.set(0);
    
    camDist = 7300;
    cam.setPosition(1, 0, camDist);
    cam.lookAt(ofVec3f(0,0,0), ofVec3f(0,1,0));
    cam.setFov( 15 );
    
    // 01, 03, 06, 10, 12
    img.loadImage( "img/losglaciares10.jpg");
    load_mesh( img );
    int nw = img.getWidth() / 2;
    int nh = img.getHeight() / 2;

    gn.setup();
    gn.create( nw, nh );
    gn.setShaderType( ofxGpuNoise::SHADER_TYPE_Cellular );
    gn.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn.setFreq( 0.004 );
    gn.setOctaves( 4 );
    gn.setFrame( ofRandomuf()*100 );
    gn.update();
    
    gn2.setup();
    gn2.create( nw, nh );
    gn2.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
	gn2.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
	gn2.setFreq( 0.0001 );
	gn2.setOctaves( 4 );
    gn2.setFrame( ofRandomuf()*100 );
    gn2.update();
    
    gn3.create( nw, nh );
    gn3.setFreq( 0.02 );
    gn3.setOctaves( 6 );
    gn3.setShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin );
    gn3.setShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES );
    gn3.setFrame( ofRandomuf()*100 );
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
    
    //ofxSVG svg;
    //svg.load( ad_util::data_path + "svg/v1/ABC.svg" );
    //int w = svg.getWidth() + 1;
    //int h = svg.getHeight() + 1;
    int w = 5000;  // 7908.01;
    int h = 2500;  //4017.48;
    exporter.setup( w, h, 25, GL_RGBA, 4);
//    exporter.setup(1920, 1080, 25, GL_RGB, 4);
    exporter.setFilePattern( ofGetTimestampString() + "/frame_%05i." + "tif" );
    exporter.setAutoExit(true);
    exporter.setFrameRange(1,3000);
    
    ofSetWindowPosition(0, 0);
    ofSetWindowShape( exporter.getFbo().getWidth()/2, exporter.getFbo().getHeight()/2);
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

    if( bTriangleTess )
        mesh.setMode( OF_PRIMITIVE_TRIANGLES );
    else
        mesh.setMode( OF_PRIMITIVE_LINES );

#ifdef USE_LINE_MESH
	lines.clear();
	lines.setUsage( GL_DYNAMIC_DRAW );
	lines.setMode( OF_PRIMITIVE_LINES);
#endif
    
    hole_factors.clear();
    
    int ch = img.getPixelsRef().getNumChannels();
    mW = img.getWidth() / res;
    mH = img.getHeight() / res;
	
	float startx = -img.getWidth()*0.5;
	float starty = -img.getHeight()*0.5;
	
    int index = 0;
    float rnd = ofRandomuf() * 100.0;
    for( int y=0; y<mH; y++ ){
        for( int x=0; x<mW; x++ ){
            
            ofVec2f sampling_point( x*res, y*res );
            ofFloatColor c = img.getPixelsRef().getColor( sampling_point.x, img.getHeight()-sampling_point.y );
            c.a = 0.7;
            //float z = c.getBrightness() * extrusion;
            float z = 0;
			ofVec3f pos( startx + sampling_point.x, starty + sampling_point.y, z);

			mesh.addVertex( pos  );
            mesh.addColor( c );
			
#ifdef USE_LINE_MESH
            // create prep Line mesh
            lines.addVertex( pos );
            pos.y += ofSignedNoise(x*0.001) * 20.0;
            pos.x += ofSignedNoise(y*0.001) * 20.0;
            pos.z *= 0.9;
            //pos.z += ofSignedNoise(index*0.001) * 30.0;
            lines.addVertex( pos );
			
			lines.addColor( c );
			lines.addColor( c );
#endif
            
            hole_factors.push_back(600*ofNoise(rnd+x*0.1, rnd+y*0.1) + 3);
			index++;
        }
    }
	
    for( int y=0; y<mH-1; y++ ){
        for( int x=0; x<mW-1; x++ ){
            
            if( bTriangleTess ){
                mesh.addIndex(x+y*mW);				// 0
                mesh.addIndex((x+1)+y*mW);			// 1
                mesh.addIndex(x+(y+1)*mW);			// 10

                //mesh.addIndex((x+1)+y*mW);		// 1
                //mesh.addIndex((x+1)+(y+1)*mW);	// 11
                //mesh.addIndex(x+(y+1)*mW);		// 10
            }else{
                mesh.addIndex(x+y*mW);				// 0
                mesh.addIndex((x+1)+y*mW);			// 1
            }
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
    
    frame++;

    if( frame > 200 ) bMakeHole = true;

    gn.addFrame( 0.02 * animSpeed );
    gn.update();
    
    gn2.addFrame( 0.012 * animSpeed );
    gn2.update();
    
    gn3.addFrame( 0.002 );
    gn3.update();
    
    int n = mesh.getNumVertices();
    if( 0 >= n )
        return;
    
    vector<ofVec3f> &vs = mesh.getVertices();
    vector<ofFloatColor> &cols = mesh.getColors();

    for( int y=0; y<mH; y++){
        for( int x=0; x<mW; x++){
            
            int index = x + y*mW;
            float rate = 1;
            
            float r = cols[index].r*2 - 1;
            float g = cols[index].g*2 - 1;
            float b = cols[index].b*2 - 1;
            
            rate = (g+b) * 0.06;

            float nR1 = gn.getNoisef( x, y, 0);
            float nG1 = gn.getNoisef( x, y, 1);
            float nB1 = gn.getNoisef( x, y, 2);
            
            float nR2 = gn2.getNoisef( x, y, 0) * 0.1;
            float nG2 = gn2.getNoisef( x, y, 1) * 0.1;
            float nB2 = gn2.getNoisef( x, y, 2) * 0.1;
            
            {
                float nR = nR1 + nR2;
                float nG = nG1 + nG2;
                float nB = nB1 + nB2;
                
                if( !bThruAccel ){
                    accel[index].x = nR * rate * 0.3;
                    accel[index].y = nG * rate * 0.3;
                    //accel[index].z = nB * rate * 0.05;
                    speed[index] += accel[index];
                }else{
                    speed[index].x = nR * rate * 0.02;
                    speed[index].y = nG * rate * 0.02;
                    //speed[index].z = nB * rate * 0.02;
                }
                
                speed[index] *= friction;
                vs[index] += speed[index];
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
            }
        }
    }
    
    set_indices();
    update_line_mesh();
}

void ofApp::set_indices(){
    
   if( bMakeHole ){
       
       mesh.clearIndices();
       mesh.setMode( OF_PRIMITIVE_LINES );
       
       hole_index.clear();
       
       vector<ofVec3f> &vs = mesh.getVertices();
       vector<ofFloatColor> &cols = mesh.getColors();
       
       int max_index = mW * mH -1;
       
       for( int y=0; y<mH; y++){
           for( int x=0; x<mW; x++){
               
               int index = x + y*mW;
               
               float n1r = gn.getNoiseuf(x,y,0);
               float n1g = gn.getNoiseuf(x,y,1);
               float n1b = gn.getNoiseuf(x,y,2);

               float n2r = gn2.getNoiseuf(x,y,0);
               float n2g = gn2.getNoiseuf(x,y,1);
               float n2b = gn2.getNoiseuf(x,y,2);

               float n3r = gn3.getNoiseuf(x,y,0);
               float n3g = gn3.getNoiseuf(x,y,1);
               float n3b = gn3.getNoiseuf(x,y,2);

               float hole  = n1r*n1r*n1r + n3r*n3g*n3b  + ofRandomuf()*0.1;
               hole_factors[index] -= hole;

               if( hole_factors[index] > 0 ){
               //if( hole > 0.1 ){
                   
                   if( x<mW-1 && y<mH){
                       if ( bTriangleTess ) {
                           mesh.addIndex( index );
                           mesh.addIndex( index+1 );
                           mesh.addIndex( index+mW );
                           
                           //mesh.addIndex( index+1 );
                           //mesh.addIndex( index+mW+1 );
                           //mesh.addIndex( index+mW );
                       }else{
                           // line
                           mesh.addIndex( index );
                           mesh.addIndex( index+1 );
                           
                           // +
                           if( ofRandomf()*0.01+n1r > 0.8 ){
                               
                               int id = index + mW;
                               if( id <= max_index ){
                                   mesh.addIndex( index );
                                   mesh.addIndex( id );
                               }
                               
                               id = index - mW;
                               if( id >= 0){
                                   mesh.addIndex( index );
                                   mesh.addIndex( id );
                               }
                           }
                       }
                       
                       // debug
                       //cols[index].set(0, 1, 0 ,1);
                   }
               }else{
                   
                   if (cols[index].a <= 0) {
                       continue;
                   }

                   hole_index.push_back( index );

                   // debug
                   //cols[index].set(1, 0, 0, 1);
                   
                   ofVec3f rv( n3r-0.5, n3b-0.5, 0);
                   rv *= 0.05;
                   rv.y += 0.001;
                   speed[index] += rv;
                   
                   vs[index].x += gn.getNoisef(index, 1);
                   vs[index].y += gn.getNoiseuf(index, 2);
                   cols[index].a    -= 0.0001;

                   float red_rate = ofRandomuf()*0.01;
                   cols[index].r += red_rate;
                   
                   if( 1 ){
                       float low_limit = 10;
                       int aid;
                       
                       // RIGHT
                       aid = index + 1;
                       if( aid <=max_index && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r  += red_rate;
                       }
                       
                       aid = index + 2;
                       if( aid<=max_index && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r  += red_rate/2;
                       }

                       // LEFT
                       aid = index - 1;
                       if( aid >=0 && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                            cols[aid].r  += red_rate;
                       }
                       
                       aid = index - 2;
                       if( aid>=0 && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r  += red_rate/2;
                       }


                       // UP
                       aid = index + mW;
                       if( index+mW<=max_index && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate;
                       }

                       aid = index + mW*2;
                       if( aid<=max_index && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate/2;
                       }

                       aid = index + mW + 1;
                       if( aid <=max_index && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate/2;
                       }

                       aid = index + mW - 1;
                       if( aid<=max_index && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate/2;
                       }
                       
                       // DOWN
                       aid = index - mW;
                       if( aid>=0 && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate;
                       }

                       aid = index - mW*2;
                       if( aid>=0 && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate/2;
                       }
                       
                       aid = index - mW + 1;
                       if( aid>=0 && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate/2;
                       }

                       aid = index - mW - 1;
                       if( aid>=0 && hole_factors[aid]>low_limit ){
                           vs[aid] += rv;
                           cols[aid].r += red_rate/2;
                       }
                   }
               }
           }
       }
   }
}

#ifdef USE_LINE_MESH
void ofApp::update_line_mesh(){
    
    vector<ofVec3f> &vs = mesh.getVertices();
    vector<ofFloatColor> &cols = mesh.getColors();
    
    vector<ofVec3f> &lvs = lines.getVertices();
    vector<ofFloatColor> &lcols = lines.getColors();
    
    int max_index = mW * mH -1;
    
    if( 0 ){
        for( int y=0; y<mH; y++){
            for( int x=0; x<mW; x++){
                int index = x + y*mW;
                lvs[index*2] = vs[index];
                
                if( hole_factors[index] < 0 ){
                    // hole
                    ofFloatColor lc =  ofFloatColor(1) - cols[index];
                    lc.a *= 0.97;
                    lcols[index*2] = lc;
                    lcols[index*2+1] = lc;
                }else{
                    // Off
                    lcols[index*2].set(0, 0);
                    lcols[index*2+1].set(0, 0);
                }
            }
        }
    }

    int nh = hole_index.size();
    if( nh < 100) return;
    lines.clear();
    lines.setUsage( GL_DYNAMIC_DRAW );
    lines.setMode( OF_PRIMITIVE_LINES);
    
    int nl = 1000;
    for (int i=0; i<nl; i++) {
    
        float n1r = gn.getNoiseuf( i, 0);
        float n1g = gn.getNoiseuf( i, 1);
        float n1b = gn.getNoiseuf( i ,2);
        
        int id1 = hole_index[n1r * (nh-1) ];
        int id2 = hole_index[n1g * (nh-1) ];
        
        ofVec3f & v1 = vs[id1];
        ofVec3f & v2 = vs[id2];
        float dist = v1.distance(v2);
        if( 30 < dist && dist < 3000 ){
            lines.addVertex( v1 );
            lines.addVertex( v2 );
            lines.addColor( cols[id2] );
            lines.addColor( cols[id1] );
        }
    }
}
#endif

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
                
#ifdef USE_LINE_MESH
                if( bDrawLineMesh ){
                    glPointSize( 1 );
                    glLineWidth( 1 );

                    if( lines.getNumVertices() > 10 ){
                        //lines.setMode( OF_PRIMITIVE_LINES);
                        lines.draw();
                        //lines.setMode( OF_PRIMITIVE_POINTS);
                        //lines.draw();
                    }
                }
#endif
                if( bDrawMesh ){
                    glPointSize( 2);
                    glLineWidth( 1 );
                    if( mesh.getNumIndices() != 0){
                        mesh.setMode( OF_PRIMITIVE_LINES );
                        mesh.drawWireframe();
                    }
                    
                    mesh.setMode( OF_PRIMITIVE_POINTS );
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
        
        gn.draw( ofGetWidth()-200,  100, 0.1);
        gn2.draw( ofGetWidth()-200, 250, 0.1);
        gn3.draw( ofGetWidth()-200, 400, 0.1);

        ofPushMatrix();{
            ofSetColor( 255 );
            exporter.draw(0, 0);
        }ofPopMatrix();
    }
    
    stringstream ss;
    ss << "frame      : " << frame << "\n";
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
    ss << "export enable   : " << exporter.isExporting() << "\n";
    ss << "export frame    : " << exporter.getFrameNum() << "\n";

    ss << "\n";
    ss << "Mesh Resolution : " << res << "\n";
    ss << "Make Hole       ; " << bMakeHole << "\n";

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
				cam.setPosition(0, -60, camDist);
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
