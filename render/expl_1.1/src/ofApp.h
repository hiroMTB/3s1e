#pragma once

#include "ofMain.h"
#include "ofxExportImageSequence.h"
#include "ofxSvg.h"

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
using namespace tbb;


class bLine{
    
public:
    ofVec2f st;
    ofVec2f end;
    
    void draw(){
        ofLine( st, end );
    }
};

class ofApp : public ofBaseApp{

public:
    void setup();
        void setup_export_layer( int w, int h, int num );
        void setup_scene();

    void update();
    
    void draw();
        void draw_all();
        void draw_layer_0();
        void draw_preview();
        void draw_info();

    void keyPressed(int key);


    bool bStart;
    bool bDebugDraw;
    bool bDrawInfo;
    int layer_num;
    vector<ofxExportImageSequence> exps;
    
    int frame;
    
    ofVboMesh n_points;
    ofVboMesh points;
    ofVboMesh lines;
    ofVboMesh prep_lines;
    ofVboMesh prep_lines_b;

    ofVboMesh branchs;
    ofVboMesh flowers;
    ofVboMesh circles;
    
    ofxSVG svg;
    ofxSVG svg_r;
    ofPoint win;
    ofPoint center;
    ofPoint st;
    ofPoint end;
    
    float gAngle;

    ofMesh circle;

    vector<bLine> bLines;
    
    list<int> branchIds;
    
    ofImage img;
    ofImage img2;
    
    
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
            
            for( int i=range.begin(); i!=range.end(); ++i ){
                
                const ofVec3f &pos1 = input[i];
                
                multimap<float, ofVec3f> near_p;
                pair<float, ofVec3f> pair1( 999999999999, ofVec3f(-12345,0,0) );
                for( int line=0; line<num_line; line++ ){
                    near_p.insert( pair1 );
                }
                
                //int try_num = 500;
                int try_num = total_size;

                for( int j=0; j<try_num; j++ ){
                    
                    int id2 = j;
                    //int id2 = ofRandomuf()*total_size;
                    //int id2 = i + j - try_num/2;
                    //id2 = abs(id2)%total_size;
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
                        ofFloatColor c = ofFloatColor(0.8, 0.8) - in_colors[i];
                        int outid = i*num_line*num_dupl*2 + j*num_dupl*2;

                        output[outid] = pos1;
                        output[outid+1] = p2;
                        out_colors[outid] = c;
                        out_colors[outid+1] = c;
                        c.a = 0.06;
                        
                        for( int k=0; k<num_dupl-1; k++ ){
                            float rate = 1.0 + k/2;
                            ofVec3f r1( ofRandomf(), ofRandomf() );
                            ofVec3f r2( ofRandomf(), ofRandomf() );

                            int did = outid + 2 + k*2;
                            output[ did ] = pos1 + r1*rate;
                            output[ did + 1] = p2 + r2*rate;
                            out_colors[ did ] = c; //ofFloatColor(0,0);
                            out_colors[ did + 1] = c; //ofFloatColor(0,0);
                        }
                    }
                }
            }
        }
    };

    void calcNearestPoints( const ofVec3f * input, ofVec3f * output, ofFloatColor * in_colors, ofFloatColor * out_colors, size_t n, int num_line, int num_dupl ){
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
    
    
    
    
    /*
    struct TbbTestCase {
        
        const ofVec3f *input;
        ofVec3f *output;
        
         // input = all point
         // output = all point * 10
        void operator()( const blocked_range<int>& range ) const {
            for( int i=range.begin(); i!=range.end(); ++i ){
                for( int j=0; j<10; j++ ){
                    output[i*10+j] = input[i] + ofVec3f(1+j,1+j,1+j);
                }
            }
        }
    };
    
    void calcTestCase( const ofVec3f * input, ofVec3f * output, size_t n ){
        TbbTestCase tc;
        tc.input = input;
        tc.output = output;
        parallel_for( blocked_range<int>(0,n), tc );
    }
    */
};



