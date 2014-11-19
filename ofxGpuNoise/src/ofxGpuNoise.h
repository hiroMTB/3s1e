#pragma once

#include "ofMain.h"
#include "ofxFastFboReader.h"
class ofxGpuNoise {
    
public:
    
    enum ShaderType{
        SHADER_TYPE_Perlin          = 0,
        SHADER_TYPE_SimplexPerlin   = 1,
        SHADER_TYPE_Cellular        = 2,
        SHADER_TYPE_Value           = 3,
    };
    
    enum ShaderDerivType{
        SHADER_DERIV_TYPE_NO        = 0,
        SHADER_DERIV_TYPE_YES       = 1
    };
    
    ofxGpuNoise();
    ~ofxGpuNoise();
    
    void            compileShader( string frag_name, ShaderType t, ShaderDerivType dt );
    void            create( int aValidWidth, int aValidHeight );
    
    void            setup( string shader_file_path="../../../../../addons/ofxGpuNoise/libs/shader/");
    void            update();
    void            draw( int x, int y, float scale );
	void			draw_samplingPointTexture( int x, int y, float scale );
    
    void            reset();
    void            clear();
    
    ofPixels *      getNoiseData();
    ShaderType      getShaderType() const;
    ShaderDerivType getShaderDerivType() const;
	ofVec2f			getSamplingPoints( int index );
	
	int             getWidth() const;
    int             getHeight() const;
    float           getFreq() const;
    
    void            setFrame( float aFrame );
    void            setOctaves( int aOctaves );
    void            setFreq( float aFreq );
    void            setShaderType( ShaderType aShaderType );
    void            setShaderDerivType( ShaderDerivType aShaderDerivType );
    void            setSamplingPointsScale( float aSamplingPointsScale );
    void            setSamplingPointsOffset( float aSamplingPointsOffset );
    void            setSamplingPoints( const vector<ofVec2f>& aSamplingPoints );
    void            setSamplingPoint( const ofVec2f & point, int position );
    void            setSendSamplingPoints( bool aSendSamplingPoints );
    void            toggleUseFboReader();

private:
    
    static bool     mShaderCompiled;
    static const int mShaderTypeNum = 4;
    static const int mShaderDerivTypeNum = 2;
    
    static ofShader mShaders[ mShaderTypeNum ][ mShaderDerivTypeNum ];
    
    bool            mUseFboReader;
    bool            mPixelAllocated;
    bool            mDownloadNoiseData;
    bool            mSendSamplingPoints;
    
    int             mWidth;
    int             mHeight;
    int             channel;
    int             mOctaves;
    
    float           mFrame;
    float           mFreq;
    float           mSamplingPointsScale;
    float           mSamplingPointsOffset;
	
	string			mShader_file_path;
    ofFbo           mFbo;
    ShaderType      mShaderType;
    ShaderDerivType mShaderDerivType;
    vector<ofVec2f> mSamplingPoints;
    ofFloatPixels   mSamplingPointsPix;
    ofTexture       mSamplingPointsTexture;
    ofxFastFboReader mFboReader;
    ofPixels        mNoiseData;
    
    ofPlanePrimitive plane;
};