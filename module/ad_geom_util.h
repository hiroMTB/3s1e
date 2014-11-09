//
//  ad_geom_util.h
//  dist_PL_test
//
//  Created by mtb on 09/11/14.
//
//

#pragma once

#include "ofMain.h"

class ad_geom_util{
	
public:
	ad_geom_util();
	~ad_geom_util();
	
	
	static float dist_pl( ofVec3f point, ofVec3f line_st, ofVec3f line_end );
	
	static ofVec3f vec_pl( ofVec3f point, ofVec3f line_st, ofVec3f line_end );
	
};