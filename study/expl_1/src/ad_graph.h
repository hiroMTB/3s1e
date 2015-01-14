//
//  ad_graph.h
//  expl_1.1
//
//  Created by hiroshi matoba on 1/12/15.
//
//

#pragma once

#include "ofMain.h"

class ad_graph{
    
public:
    static int max_btype;
    static int max_ftype;
    
    static void add_branch( ofVboMesh &mesh, const ofVec2f &st, const ofVec2f &end, ofFloatColor c, int btype=-1, bool dot=false );
    static void add_flower( ofVboMesh &mesh, const ofVec2f &st, const ofVec2f &end, ofFloatColor c, int ftype=-1 );
};