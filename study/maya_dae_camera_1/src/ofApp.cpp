#include "ofApp.h"
#include "ofxAssimpUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape( 1920, 1080 );
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(50, 0);

    ofDisableArbTex(); // we need GL_TEXTURE_2D for our models coords.
	
    model.loadModel("B4.dae", true);
    const aiScene * scene = model.getAssimpScene();
    int n = scene->mNumCameras;
    aiCamera ** cams = model.getAssimpScene()->mCameras;

    int anum = scene->mNumAnimations;
    aiAnimation ** anims = scene->mAnimations;
    for (int i=0; i<anum; i++) {
        aiAnimation * anim = anims[i];
        aiString name = anim->mName;
        int cnum = anim->mNumChannels;
        for (int j=0; j<cnum; j++) {
            aiNodeAnim * ch = anim->mChannels[j];
            
            aiString nname = ch->mNodeName;
            aiVectorKey &v = ch->mPositionKeys[0];
            aiQuaternion&q = ch->mRotationKeys[0].mValue;

            ofVec3f pos = aiVecToOfVec( v.mValue );
            ofQuaternion quat( q.x, q.y, q.z, q.w );
            ofVec3f rot = quat.getEuler();
            
            stringstream ss;
            ss << nname.data << "\n";
            ss << pos << "\n";
            ss << quat << "\n";
            ss << rot << "\n";
            cout << ss.str() << endl;
			
			ofMatrix4x4 mat;
			mat.translate( pos );
			mat.rotate( quat );
			cam.setTransformMatrix( mat );
        }

        
        stringstream ss;
        ss << name.data << "\n";
        
        cout << ss.str() << endl;
    }
    
    for (int i=0; i<n; i++) {
        aiCamera * acam = cams[i];
        stringstream ss;
        
        aiString name = acam->mName;
        ofVec3f pos = aiVecToOfVec(acam->mPosition);
        ofVec3f up = aiVecToOfVec(acam->mUp);
        
        ss << name.data << "\n";
        ss << pos << "\n";
        ss << up << "\n";
        cout << ss.str() << endl;
        
        cam.setPosition( pos );
    }
    
    model.setPosition(ofGetWidth() * 0.5, (float)ofGetHeight() * 0.75 , 0);
    model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
    model.playAllAnimations();
}

//--------------------------------------------------------------
void ofApp::update(){
    model.update();
    mesh = model.getCurrentAnimatedMesh(0);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground( 255 );
    ofSetColor(5);

    cam.begin();
    ofPushMatrix();
    model.drawWireframe();
    ofPopMatrix();
    cam.end();

//    ofPushMatrix();
//    ofTranslate(model.getPosition().x-300, model.getPosition().y, 0);
//    ofRotate(-mouseX, 0, 1, 0);
//    ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
//    
//    ofxAssimpMeshHelper & meshHelper = model.getMeshHelper(0);
//    
//    ofMultMatrix(model.getModelMatrix());
//    ofMultMatrix(meshHelper.matrix);
//    
//    ofMaterial & material = meshHelper.material;
//    if(meshHelper.hasTexture()){
//        meshHelper.getTextureRef().bind();
//    }
//    material.begin();
//    mesh.drawWireframe();
//    material.end();
//    if(meshHelper.hasTexture()){
//        meshHelper.getTextureRef().unbind();
//    }
//    ofPopMatrix();

    
    ofSetColor(255, 255, 255 );
    ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate(), 2), 10, 15);
    ofDrawBitmapString("keys 1-5 load models, spacebar to trigger animation", 10, 30);
    ofDrawBitmapString("drag to control animation with mouseY", 10, 45);
    ofDrawBitmapString("num animations for this model: " + ofToString(model.getAnimationCount()), 10, 60);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    ofPoint modelPosition(ofGetWidth() * 0.5, (float)ofGetHeight() * 0.75);
    switch (key) {
        default:
            break;
    }
}
