//
//  ofxGpuNoise.cpp
//  perlinGpu
//
//  Created by hiroshi matoba on 11/5/14.
//
//

#include "ofxGpuNoise.h"
#include "ofxGpuNoiseShaders.h"

ofShader ofxGpuNoise::mShaders[ ofxGpuNoise::mShaderTypeNum ][ ofxGpuNoise::mShaderDerivTypeNum ];
bool ofxGpuNoise::mShaderCompiled = false;

ofxGpuNoise::ofxGpuNoise()
:
mFrame( 0 ),
mFreq( 0.05 ),
mOctaves( 4 ),
mUseFastFboReader( true ),
mDownloadNoiseData( true ),
mPixelAllocated( false ),
mWidth( 0 ),
mHeight( 0 ),
mShaderType( ofxGpuNoise::SHADER_TYPE_SimplexPerlin ),
mShaderDerivType( ofxGpuNoise::SHADER_DERIV_TYPE_YES ),
mSendSamplingPoints( false ),
mSamplingPointsScale( 1 ),
mSamplingPointsOffset(0){
    setup();
}

ofxGpuNoise::~ofxGpuNoise(){
    reset();
    for(int i=0; i<mShaderTypeNum; i++ ){
        for(int j=0; j<mShaderDerivTypeNum; j++ ){
            mShaders[i][j].unload();
        }
    }
}

void ofxGpuNoise::setup(){
    if( !mShaderCompiled ){
        compileShaderFromSrc( getPerlin3D(), SHADER_TYPE_Perlin, SHADER_DERIV_TYPE_NO );
        compileShaderFromSrc( getPerlin3D_Deriv(), SHADER_TYPE_Perlin, SHADER_DERIV_TYPE_YES );
        compileShaderFromSrc( getSimplexPerlin3D(), SHADER_TYPE_SimplexPerlin, SHADER_DERIV_TYPE_NO );
        compileShaderFromSrc( getSimplexPerlin3D_Deriv(), SHADER_TYPE_SimplexPerlin, SHADER_DERIV_TYPE_YES );
        compileShaderFromSrc( getCellular3D(), SHADER_TYPE_Cellular, SHADER_DERIV_TYPE_NO );
        compileShaderFromSrc( getCellular3D_Deriv(), SHADER_TYPE_Cellular, SHADER_DERIV_TYPE_YES );
        compileShaderFromSrc( getValue3D(), SHADER_TYPE_Value, SHADER_DERIV_TYPE_NO );
        compileShaderFromSrc( getValue3D_Deriv(), SHADER_TYPE_Value, SHADER_DERIV_TYPE_YES );
        mShaderCompiled = true;
    }
}

void ofxGpuNoise::compileShaderFromSrc( string frag_code , ShaderType t, ShaderDerivType dt ) {

    mShaders[t][dt].setupShaderFromSource( GL_VERTEX_SHADER, getPassThruVert() );
    mShaders[t][dt].setupShaderFromSource( GL_FRAGMENT_SHADER, frag_code );
    mShaders[t][dt].linkProgram();
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
    
    mNoiseDataPix.clear();
    mNoiseDataPix.allocate( mWidth, mHeight, OF_PIXELS_RGB );
    
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
        float * pix = mSamplingPointsPix.getPixels();

        for(int i=0; i<mHeight; i++){
            for(int j=0; j<mWidth; j++){
                int index = i*mWidth + j;
                ofVec2f point = mSamplingPoints[index];
                
                //  Don't use ofPixels::setPixel(x,y,ofColor) it's slow
                pix[index*3 + 0] = point.x;
                pix[index*3 + 1] = point.y;
                pix[index*3 + 2] = 0.0;
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
     */
    if( mDownloadNoiseData ) {
        //if( mUseFastFboReader ){
#ifdef USE_FAST_FBO_READER
            mFboReader.readToPixels( mFbo, mNoiseDataPix );
#else
            
        //}else{
            mFbo.getTextureReference().readToPixels( mNoiseDataPix );
        //}
#endif
        mPixelAllocated = true;
    }
}

void ofxGpuNoise::draw( int x, int y, float scale ) {
	glColor4f( 1,1,1,1 );
    mFbo.draw( x, y, mWidth*scale, mHeight*scale );
}

void ofxGpuNoise::draw_samplingPointTexture( int x, int y, float scale ){
	if( mSendSamplingPoints ) {
		glColor3f( 1, 1, 1 );
		mSamplingPointsTexture.draw( x, y, mWidth*scale, mHeight*scale );
	}
}

void ofxGpuNoise::reset(){
    if( mNoiseDataPix.isAllocated() && mPixelAllocated ){
        mNoiseDataPix.clear();
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
float ofxGpuNoise::getNoisef( int index, int ch ){
	index %= mWidth * mHeight;
	return (mNoiseDataPix.getPixels()[index*3 + ch]-128)*2.0/255.0;
}

float ofxGpuNoise::getNoiseuf( int index, int ch ){
	index %= mWidth * mHeight;
	return mNoiseDataPix.getPixels()[index*3 + ch]/255.0;
}

unsigned char ofxGpuNoise::getNoiseuc( int index, int ch ){
	index %= mWidth * mHeight;
	return mNoiseDataPix.getPixels()[index*3 + ch];
}

float ofxGpuNoise::getNoisef( int x, int y, int ch ){
    int index = x + y*mWidth;
    return getNoisef( index, ch );
}

float ofxGpuNoise::getNoiseuf( int x, int y, int ch ){
    int index = x + y*mWidth;
    return getNoiseuf(index, ch );
}

unsigned char ofxGpuNoise::getNoiseuc( int x, int y, int ch ){
    int index = x + y*mWidth;
    return getNoiseuc(index, ch);
}

unsigned char * ofxGpuNoise::getNoiseData() {
    return mNoiseDataPix.getPixels();
}

string ofxGpuNoise::getNoiseParamString(){
	stringstream ss;
	ss << "Gpu Noise" << "\n";
	ss << "ShaderType      : " << mShaderType		<< "\n";
	ss << "ShaderDerivType : " << mShaderDerivType	<< "\n";
	ss << "Freq            : " << mFreq				<< "\n";
	ss << "Octave          : " << mOctaves			<< "\n";
	ss << "frame           : " << mFrame            << "\n";
	return ss.str();
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

ofVec2f & ofxGpuNoise::getSamplingPoints( int index ){
	if( index<mSamplingPoints.size() )
		return mSamplingPoints.at( index );
//	else
//		return ofVec2f( -123,-123 );
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

void ofxGpuNoise::toggleUseFastFboReader() {
    mUseFastFboReader = !mUseFastFboReader;
}