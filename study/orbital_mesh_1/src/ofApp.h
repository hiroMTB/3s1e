#pragma once

#include "ofMain.h"
#include "ofxExportImageSequence.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTransitions.h"

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
using namespace tbb;

class ofApp : public ofBaseApp{

public:
    void setup();
        void setup_export_layer( int w, int h, int num );

    void update();
    
    void draw();
        void draw_all();
        void draw_layer_0();
        void draw_preview();
        void draw_info();
    void load_model( string path );
    
    void keyPressed(int key);


    bool bStart;
    bool bDrawInfo;
    int layer_num;
    vector<ofxExportImageSequence> exps;

    ofxAssimpModelLoader model;
    ofVboMesh mesh;
    ofVboMesh points;
    
    ofEasyCam cam;
    
    ofPoint win;
    
    ofVec3f aabb_max, aabb_min, aabb_center;
    float scale;
    
    void draw_near_line();
    ofVboMesh lines;


    struct NearestPoints {
        const ofVec3f *input;
        ofVec3f *output;
        ofFloatColor * in_colors;
        ofFloatColor * out_colors;
        
        int total_size;
        int num_line;
        int num_dupl;
        
        /*
         *      input = all point
         *      output = all point * 10
         */
        void operator()( const blocked_range<int>& range ) const {
            
            
            multimap<float, ofVec3f> near_p;
            pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
            
            for( int i=range.begin(); i!=range.end(); ++i ){
                
                const ofVec3f &pos1 = input[i];
                
                near_p.clear();
                for( int line=0; line<num_line; line++ ){
                    near_p.insert( pair1 );
                }
                
                //int try_num = 6000;
                int try_num = total_size;
                
                for( int j=0; j<try_num; j++ ){
                    
                    int id2 = j;
                    //int id2 = ofRandomuf()*total_size;
                    //int id2 = i + j - try_num/2;
                    //id2 = abs(id2)%total_size;
                    if( i==id2 ) continue;
                    
                    const ofVec3f &pos2 = input[id2];
                    
                    float sat_dist = 20;
                    int sat_num = 0;
                    
                    float dist = pos1.distance( pos2 );
                    float limit = 600;
                    if( 100<dist && dist<limit ){
                        multimap<float, ofVec3f>::iterator itr = near_p.end();
                        
                        itr--;
                        if( dist < itr->first ){
                            std::pair<float, ofVec3f> pair2( dist, pos2 );
                            near_p.insert( pair2 );
                            
                            multimap<float, ofVec3f>::iterator end = near_p.end();
                            near_p.erase( --end );
                        }
                        
                        if( dist < sat_dist ){
                            if( ++sat_num >= num_line*2 ){
                                break;
                            }
                        }
                    }
                }
                
                ofxTransitions tr;
                
                multimap<float, ofVec3f>::iterator itr = near_p.begin();
                ofFloatColor c = ofFloatColor(0.8, 0.8) - in_colors[i];
                float alpha = c.a;
                for(int j=0; itr!=near_p.end(); itr++, j++ ){
                    
                    ofVec3f &p2 = itr->second;
                    c.a = 0.2;
                    if( p2.x != -12345){
                        int outid = i*num_line*num_dupl*2 + j*num_dupl*2;
                        
                        ofVec3f dir = p2 - pos1;
                        
                        for( int k=0; k<num_dupl/2; k++ ){

                            for( int l=0; i<2; i++){
                                int did = outid + k*2 + l*2;
                                float x = (float)k/num_dupl;
                                float y = tr.easeInOutBounce(k, 0, 1.0, num_dupl);
                                ofVec3f trd( dir.x*x, dir.y*y);
                                
                                ofVec3f p = pos1 + trd;
                                output[ did ] = p;
                                out_colors[ did ] = c;
                            }
                        }
                    }
                }
            }
        }
    };
    
    void calcNearestPoints( const ofVec3f * input, ofVec3f * output, ofFloatColor * in_colors, ofFloatColor * out_colors, size_t n, int num_line, int num_dupl  ){
        NearestPoints np;
        np.input = input;
        np.output = output;
        np.in_colors = in_colors;
        np.out_colors = out_colors;
        np.total_size = n;
        np.num_line = num_line;
        np.num_dupl = num_dupl;
        parallel_for( blocked_range<int>(0,n), np );
    }

};
