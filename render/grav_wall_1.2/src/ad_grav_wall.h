//
//  ad_grav_wall.h
//  grav_wall
//
//  Created by hiroshi matoba on 11/21/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
using namespace tbb;

struct gvWall{
private:
    gvWall();
    
public:
    gvWall( ofVec2f _p1, ofVec2f _p2):p1(_p1), p2(_p2), inited(true){};
    
    bool bGrav;
    bool inited;
    ofVec2f p1;
    ofVec2f p2;
    ofVec2f norm;
};

class ad_grav_wall{
    
public:
    
    ad_grav_wall();
    void setup( ofImage * img=NULL, ofCamera * cam = NULL );

    void create_line( ofVec2f p1, ofVec2f p2, float density=0.1 );
    void create_waterline( ofVec2f p1, ofVec2f p2 );
    void create_wall( ofVec2f p1, ofVec2f p2, float thicknes=1 );
    void update();
    void update_lines();
    void update_points();
    void update_attrs();
    void draw_lines();
    void draw_points();
    void draw_guide();
    void setGrav( float g);
    void toggleCollision( float rate );
    void reset_attr();
    void change_attr();
    void change_size( float size );
    
    static int particle_col_group;
    static int wall_col_group;
    static int particle_col_setting;
    static int wall_col_setting;
    static int pid;
    static bool bDrawNearLine;
    static bool bDrawAttrLine;
    static int gravType;
    
    bool bInitPhysics;
	bool bReleased;
    int near_line_num;
    float frame;
    float impulse;
    
    ofImage * colref;
    ofVboMesh points;
    ofVboMesh lines;
    ofVboMesh attrLines;
    ofxBulletWorldRigid world;
    
    vector<btSphereShape*> sphereShapes;
    vector<ofxBulletBaseShape*>	shapes;
    vector<ofxBulletBox*> walls;
    vector<ofVec3f> pivots;
    vector<ofVec3f> collision;
    vector<gvWall*> gvws;
    vector<gvWall*> waterlines;
    
    //  prll
    struct NearestPoints {
        const ofVec3f *input;
        ofVec3f *output;
        ofFloatColor * in_colors;
        ofFloatColor * out_colors;
        
        int total_size;
        int num_line;
        int num_dupl;
        unsigned char * noise;
        
        void operator()( const blocked_range<int>& range ) const {
            
            multimap<float, ofVec3f> near_p;
            pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
            
            for( int i=range.begin(); i!=range.end(); ++i ){
                
                float nR = (float)noise[i*3 + 0]/255.0;
                float nG = (float)noise[i*3 + 1]/255.0;
                float nB = (float)noise[i*3 + 2]/255.0;
                
                const ofVec3f &pos1 = input[i];
                
                near_p.clear();
                for( int line=0; line<num_line; line++ ){
                    near_p.insert( pair1 );
                }
                
                //int try_num = 6000;
                int try_num = total_size;
                
                for( int j=0; j<try_num; j++ ){
                    
                    int id2 = j;
                    if( i==id2 ) continue;
                    
                    const ofVec3f &pos2 = input[id2];
                    
                    float dist = pos1.distance( pos2 );
                    float limit = 600;
                    if( 10<dist && dist<limit ){
                        multimap<float, ofVec3f>::iterator itr = near_p.end();
                        
                        itr--;
                        if( dist < itr->first ){
                            std::pair<float, ofVec3f> pair2( dist, pos2 );
                            near_p.insert( pair2 );
                            
                            multimap<float, ofVec3f>::iterator end = near_p.end();
                            near_p.erase( --end );
                        }
                    }
                }
                
                multimap<float, ofVec3f>::iterator itr = near_p.begin();
                for(int j=0; itr!=near_p.end(); itr++, j++ ){
                    ofVec3f &p2 = itr->second;
                    if( p2.x != -12345){
                        int outid = i*num_line*num_dupl*2 + j*num_dupl*2;

                        ofFloatColor c = in_colors[i];
//                        c = ofFloatColor(0.8) - c;
                        c.a = 0.2;

                        output[outid] = pos1;
                        output[outid+1] = p2;
                        out_colors[outid] = c;
                        out_colors[outid+1] = c;

                        c.r += nR*0.1 + ofRandomf()*0.01;
                        c.g += nG*0.1 + ofRandomf()*0.01;
                        c.b += nB*0.1  + ofRandomf()*0.01;
                        c.a = 0.05;
                        
                        for( int k=0; k<num_dupl-1; k++ ){
                            float rate = 1 + k;
                            ofVec3f r1( ofRandomf(), ofRandomf() );
                            ofVec3f r2( ofRandomf(), ofRandomf() );
                            
                            int did = outid + 2 + k*2;
                            output[ did ] = pos1 + r1*rate;
                            output[ did + 1] = p2 + r2*rate;
                            out_colors[ did ] = c;      //ofFloatColor(1,0,0,1);
                            out_colors[ did + 1] = c;   //ofFloatColor(1,0,0,1);
                        }
                    }
                }
            }
        }
    };
    
    void calcNearestPoints( const ofVec3f * input, ofVec3f * output, ofFloatColor * in_colors, ofFloatColor * out_colors, size_t n, int num_line, int num_dupl, unsigned char * noise ){
        NearestPoints np;
        np.input = input;
        np.output = output;
        np.in_colors = in_colors;
        np.out_colors = out_colors;
        np.total_size = n;
        np.num_line = num_line;
        np.num_dupl = num_dupl;
        np.noise = noise;
        parallel_for( blocked_range<int>(0,n), np );
    }
};