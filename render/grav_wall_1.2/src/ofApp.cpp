
#include "ofApp.h"
#include "ad_util.h"

bool ofApp::bRender = true;
float ofApp::density = 0;

ofApp * ofApp::app = NULL;

void ofApp::setup(){

	ofSeedRandom(1231);
	sel_layer = 0;
    mode = 0;
    bDraw_info = true;
    bStart = true;
    ofSetCircleResolution( 4 );
    ofSetVerticalSync( true );
    
    // ABC / KLM
    //grav_wall.impulses[0] = -5;
	//grav_wall.impulses[1] = 2;
	//density = 1.3;

    // DEF / HIJ
    //grav_wall.impulses[0] = -1.2;
	//grav_wall.impulses[1] = 4.2;
	//density = 1.1;

    // G
	grav_wall.impulses[0] = -6.5;
	grav_wall.impulses[1] = 0.1;
	density = 3.1;
    
    load_noise();
    img.loadImage( ad_util::data_path +  "img/rk/Mx80_2_org.jpg");
    grav_wall.setup( &img );
    
    load_svg("");
    setup_window( win.x, win.y);
    
}

void ofApp::update(){
    
    gpu_noise.addFrame( 0.03 );
    gpu_noise.update();
    noise = gpu_noise.getNoiseData();
    grav_wall.update();
    

    /*
     * 
     *  TIME LINE
     *
     */
    frame = exps[0].isExporting() ? exps[0].getFrameNum() : ofGetFrameNum();

	
	int mix_start_f	= 250;
	int mix_end_f	= 2250;
	int mix_duration_f = mix_end_f - mix_start_f;
	int mix_frame = frame - mix_start_f;
	mix_frame = MAX(0, mix_frame);
	float mix_rate = mix_frame / (float)mix_duration_f;

	grav_wall.impulse_rate[0] = 1.0 - mix_rate;
	grav_wall.impulse_rate[1] = mix_rate;
	
    int modep = mode;
    if( frame < 50 ){
        mode = 0;
    }else if( frame < 500 ){
        mode = 1;
    }else if( frame < 1500 ){
        mode = 2;
	}else{
		mode = 3;
	}
		
    bool changed = mode != modep;
    if( changed ){
        if( mode == 1 ){
            
            // Collision ON
            grav_wall.toggleCollision( 0.99 );
            
        }else if( mode == 2){
            grav_wall.toggleCollision( 0.5 );   // small collision number make visual lika a wraping
//			grav_wall.toggleCollision( 0.8 );
		}else if( mode == 3){
			grav_wall.change_size(0.01);
		}
    }
    
    if( mode >= 2 ){
        grav_wall.change_attr();
	}
	
	if( mode == 3){
		float duration = 1000;
		float inc = 4.0;
		float step = inc / duration;
		grav_wall.impulses[1] += step;
	}
	
}

void ofApp::draw(){
    ofEnableAlphaBlending();
    ofDisableAntiAliasing();
    ofDisableSmoothing();

    exps[0].begin();{
        ofBackground( 255 );
        grav_wall.draw_lines();
		//grav_wall.draw_guide();
		mask.draw(0, 0);
    }exps[0].end();

    exps[1].begin();{
        ofBackground( 255 );
        grav_wall.draw_points();
        mask.draw(0, 0);
    }exps[1].end();
    
    ofPushMatrix();{
        ofBackground(255);
        ofSetColor(255);
        exps[sel_layer].draw(0, 0);

        //exps[1].draw(0, 0);

        ofSetColor(155);
        draw_info();
        gpu_noise.draw(300, 10, 0.2);
    }ofPopMatrix();
}
