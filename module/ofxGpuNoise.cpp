//
//  ofxGpuNoise.cpp
//  perlinGpu
//
//  Created by hiroshi matoba on 11/5/14.
//
//

#include "ofxGpuNoise.h"

ofShader ofxGpuNoise::mShaders[ ofxGpuNoise::mShaderTypeNum ][ ofxGpuNoise::mShaderDerivTypeNum ];
bool ofxGpuNoise::mShaderCompiled = false;

ofxGpuNoise::ofxGpuNoise()
:
mFrame( 0 ),
mFreq( 0.01 ),
mOctaves( 4 ),
mUseFboReader( true ),
mDownloadNoiseData( true ),
mPixelAllocated( false ),
mWidth( 0 ),
mHeight( 0 ),
mShaderType( ofxGpuNoise::SHADER_TYPE_Perlin ),
mShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_NO ),
mSendSamplingPoints( false ),
mSamplingPointsScale( 1 ),
mSamplingPointsOffset(0){

}

ofxGpuNoise::~ofxGpuNoise(){
    reset();
}

void ofxGpuNoise::setup( string aShader_file_path ){
    if( !mShaderCompiled ){
		mShader_file_path = aShader_file_path;
        compileShader( "Perlin3D.glsl", SHADER_TYPE_Perlin, SHADER_DERIV_TYPE_NO );
        compileShader( "Perlin3D_Deriv.glsl", SHADER_TYPE_Perlin, SHADER_DERIV_TYPE_YES );
        compileShader( "SimplexPerlin3D.glsl", SHADER_TYPE_SimplexPerlin, SHADER_DERIV_TYPE_NO );
        compileShader( "SimplexPerlin3D_Deriv.glsl", SHADER_TYPE_SimplexPerlin, SHADER_DERIV_TYPE_YES );
        compileShader( "Cellular3D.glsl", SHADER_TYPE_Cellular, SHADER_DERIV_TYPE_NO );
        compileShader( "Cellular3D_Deriv.glsl", SHADER_TYPE_Cellular, SHADER_DERIV_TYPE_YES );
        compileShader( "Value3D.glsl", SHADER_TYPE_Value, SHADER_DERIV_TYPE_NO );
        compileShader( "Value3D_Deriv.glsl", SHADER_TYPE_Value, SHADER_DERIV_TYPE_YES );
        mShaderCompiled = true;
    }
}

void ofxGpuNoise::compileShader( string frag_name , ShaderType t, ShaderDerivType dt ) {
    
    string vert_path( mShader_file_path + "passThru_vert.glsl" );
    string frag_path( mShader_file_path + "Wombat/" + frag_name );
    
    ofFile vert( vert_path);
    
    if( vert.isFile() ){
        cout << "compile shader[" << t <<  "][" << dt << "] = " << frag_name << endl;
        mShaders[t][dt].load( vert_path, frag_path );
    }
}

void ofxGpuNoise::create( int aValidWidth, int aValidHeight ) {
    
    reset();
    
    mWidth = aValidWidth;
    mHeight = aValidHeight;
    
    /*
     *      We allocate texture bit bigger than we need
     *      because better to set power of 2 size for width/height
     */
    //mWidth = pow( 2, ceil( log2( mValidWidth ) ) );
    //mHeight = pow( 2, ceil( log2( mValidHeight ) ) );
    
    cout << "Create Gpu Noise: size(" << mWidth<< ", " << mHeight << ")" << endl;
    
    /*
     *      fbo setup
     *      This is the background canvas for noise
     */
    ofFbo::Settings settings;
    settings.width = mWidth;
    settings.height = mHeight;
    settings.internalformat = GL_RGB;
    settings.useDepth = false;
    settings.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    settings.minFilter = GL_NEAREST;
    settings.maxFilter = GL_NEAREST;
    settings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    settings.wrapModeVertical = GL_CLAMP_TO_EDGE;
    mFbo.allocate( settings );
    
    mNoiseData.clear();
    mNoiseData.allocate( mWidth, mHeight, OF_PIXELS_RGB );
    
    /*
     *      Texture setup
     *      We set sampling point data to this texture and send to noise shader.
     */
    mSamplingPoints.resize( mWidth * mHeight );
    mSamplingPointsPix.allocate( mWidth, mHeight, OF_PIXELS_RGB );
    for(int i=0; i<mHeight; i++){
        for(int j=0; j<mWidth; j++){
            int index = i*mWidth + j;
            mSamplingPoints[index] = ofVec2f( (float)j/mWidth, (float)i/mHeight );
            mSamplingPointsPix.setColor( j, i, ofColor((float)j/mWidth, (float)i/mHeight, 0.0) );
        }
    }

    mSamplingPointsTexture.loadData( mSamplingPointsPix );
    mSamplingPointsTexture.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
    mSamplingPointsTexture.setTextureWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    
    plane.set(mWidth, mHeight, 10, 10);
    plane.mapTexCoordsFromTexture( mSamplingPointsTexture );
}

void ofxGpuNoise::update() {
    
    /*
     *      Update sampling point texture
     */
    if( mSendSamplingPoints ) {
        for(int i=0; i<mHeight; i++){
            for(int j=0; j<mWidth; j++){
                int index = i*mWidth + j;
                ofVec2f point = mSamplingPoints[index];
                mSamplingPointsPix.setColor( j, i, ofFloatColor(point.x, point.y, 0.0) );
            }
        }
        mSamplingPointsTexture.loadData( mSamplingPointsPix );
    }
    
    /*
     *      Draw noise data to Fbo
     */
    
    ofShader & shader = mShaders[mShaderType][mShaderDerivType];
    glPushMatrix();{
        mFbo.begin();{
            ofClear( 0,0,0,255 );

            shader.begin();{

                if( mSendSamplingPoints ){
                    shader.setUniformTexture("samplingPointsTexture", mSamplingPointsTexture, 1);
                    shader.setUniform1f( "samplingPointsOffset", (float)mSamplingPointsOffset );
                    shader.setUniform1f( "samplingPointsScale", (float)mSamplingPointsScale );
                }
                shader.setUniform1i( "sendSamplingPoints", (int) mSendSamplingPoints );
                shader.setUniform1i( "octaves", (int)mOctaves );
                shader.setUniform1f( "frame", (float)mFrame );
                shader.setUniform1f( "freq", (float)mFreq );
                ofTranslate( plane.getWidth()/2, plane.getHeight()/2 );
                plane.draw();
            }shader.end();
        }mFbo.end();
    }glPopMatrix();
    
    /*
     *      Read noise data from GPU and store
     *      Fbo -> FboReader(or Surface) -> mNoiseData
     */
    if( mDownloadNoiseData ) {
        if( mUseFboReader ){
            mFboReader.readToPixels( mFbo, mNoiseData );
        }else{
            mFbo.getTextureReference().readToPixels( mNoiseData );
        }
        mPixelAllocated = true;
    }
}

void ofxGpuNoise::draw( int x, int y, float scale ) {
    mFbo.draw( x, y, mWidth*scale, mHeight*scale );
}

void ofxGpuNoise::draw_samplingPointTexture( int x, int y, float scale ){
	if( mSendSamplingPoints ) {
		glColor3f( 1, 1, 1 );
		mSamplingPointsTexture.draw( x, y, mWidth*scale, mHeight*scale );
	}
}

void ofxGpuNoise::reset(){
    if( mNoiseData.isAllocated() && mPixelAllocated ){
        mNoiseData.clear();
        mPixelAllocated = false;
    }
}

void ofxGpuNoise::clear(){
    glPushMatrix();{
        ofViewport( 0, 0, mFbo.getWidth(), mFbo.getHeight() );
        mFbo.bind();{
            glClear( 0 );
        }mFbo.unbind();
    }glPopMatrix();
}

//  Getter
ofPixels * ofxGpuNoise::getNoiseData() {
    return &mNoiseData;
}

ofxGpuNoise::ShaderType ofxGpuNoise::getShaderType() const {
    return mShaderType;
}

ofxGpuNoise::ShaderDerivType ofxGpuNoise::getShaderDerivType() const {
    return mShaderDerivType;
}

int ofxGpuNoise::getWidth() const {
    return mWidth;
}

int ofxGpuNoise::getHeight() const {
    return mHeight;
}

float ofxGpuNoise::getFreq() const {
    return mFreq;
}

ofVec2f ofxGpuNoise::getSamplingPoints( int index ){
	if( index<mSamplingPoints.size() )
		return mSamplingPoints.at( index );
	else
		return ofVec2f( -123,-123 );
}

//  Setter
void ofxGpuNoise::setFrame( float aFrame ) {
    mFrame = aFrame;
}

void ofxGpuNoise::setOctaves( int aOctaves ) {
    mOctaves = aOctaves;
}

void ofxGpuNoise::setFreq( float aFreq ){
    mFreq = aFreq;
}

void ofxGpuNoise::setShaderType( ofxGpuNoise::ShaderType aShaderType ) {
    mShaderType = aShaderType;
}

void ofxGpuNoise::setShaderDerivType( ofxGpuNoise::ShaderDerivType aShaderDerivType ) {
    mShaderDerivType = aShaderDerivType;
}

void ofxGpuNoise::setSendSamplingPoints( bool aSendSamplingPoints ){
    mSendSamplingPoints = aSendSamplingPoints;
}

void ofxGpuNoise::setSamplingPointsScale( float aSamplingPointsScale ){
    mSamplingPointsScale  = aSamplingPointsScale;
}

void ofxGpuNoise::setSamplingPointsOffset( float aSamplingPointsOffset ){
    mSamplingPointsOffset  = aSamplingPointsOffset;
}

void ofxGpuNoise::setSamplingPoints( const vector<ofVec2f> & aSamplingPoints ) {
    mSamplingPoints = aSamplingPoints;
}

void ofxGpuNoise::setSamplingPoint( const ofVec2f & point, int position ) {
    mSamplingPoints[position] = point;
}

void ofxGpuNoise::toggleUseFboReader() {
    mUseFboReader = !mUseFboReader;
}