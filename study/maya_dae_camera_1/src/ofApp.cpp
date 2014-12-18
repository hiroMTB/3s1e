#include "ofApp.h"
#include "ofxAssimpUtils.h"
#include "ad_util.h"

void ofApp::setup(){
    ofSetWindowShape( 1400, 1050 );
    ofBackground(255);
    model_pos.set( 2571.943, 1080.105, -2712.017 );


    
    dae.loadModel("B_cam_simple.dae", true);
    const aiScene * scene = dae.getAssimpScene();
    int n = scene->mNumCameras;

    aiCamera ** cams = dae.getAssimpScene()->mCameras;

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
            
            stringstream ss("Dae Animation\n");
            ss << "name  : " << nname.data << "\n";
            ss << "pos   : " << pos << "\n";
            ss << "rot   : " << rot << "\n";
            cout << ss.str() << endl;
        }
        
        //stringstream ss;
        //ss << name.data << "\n";
        //cout << ss.str() << endl;
    }
    
    for (int i=0; i<n; i++) {
        aiCamera * acam = cams[i];
        
        aiString name = acam->mName;
        ofVec3f pos = aiVecToOfVec(acam->mPosition);
        ofVec3f up = aiVecToOfVec(acam->mUp);

        stringstream ss("Dae Camera\n");
        ss << "name  : " << name.data << "\n";
        ss << "pos   : " << pos << "\n";
        ss << "up    : " << up << "\n";
        cout << ss.str() << endl;
    }
    
    mymesh.setUsage( GL_DYNAMIC_DRAW );
    mymesh.setMode( OF_PRIMITIVE_TRIANGLES );
    
    int mnum = scene->mNumMeshes;
    for (int i=0; i<mnum; i++) {
        aiMesh * mesh = scene->mMeshes[i];
        aiString name = mesh->mName;
        
        stringstream ss("Dae Mesh\n");
        ss << name.data << "\n";
        
        int vnum = mesh->mNumVertices;
        for (int j=0; j<vnum; j++) {
            ofVec3f v = aiVecToOfVec( mesh->mVertices[j] );
            ss << "v[" << j << "] = "<< v << "\n";
            
            //mesh->mVertices[j].Set(-v.x, -v.y, -v.z );
            //scene->mMeshes[i] = mesh;
            
            mymesh.addVertex( v + model_pos );
        }
        cout << ss.str() << endl;
        
    }
    
    
    //dae.setPosition(0,0,0);
    
    dae.setScale(1, 1, 1);
}


void ofApp::update(){
    dae.update();
    
    if(bAnimateMouse) {
        dae.setPositionForAllAnimations(animationPosition);
    }

    mesh = dae.getCurrentAnimatedMesh(0);
}


void ofApp::draw(){
    ofBackground( 255 );
    ofSetColor(5);

    cam.begin(); {
    
        {
            ad_util::draw_axis();
            ad_util::draw_plane( 3000 );

            ofFill();
            ofSetColor(255, 0, 0);
            ofDrawSphere( target, 10);
            ofSetColor(0, 0, 255);
            ofDrawSphere( campos, 10);
        }
        
        ofSetColor(0, 255, 0);
        dae.draw( OF_MESH_WIREFRAME );

        ofSetColor(0, 0, 255);
        mymesh.drawWireframe();
    } cam.end();


    string s;
    s += ad_util::getFrameInfoString();
    s += ad_util::getCameraInfoString( cam );
    ofDrawBitmapString( s, 20, 50 );
    
}


void ofApp::keyPressed(int key){
   
    campos.set(5362.976, 300, -1560.683);
    target.set(2554.265, 805, -2461.052);
    
    // cam.setTarget( target );
    cam.setPosition( campos );
    cam.setOrientation( ofVec3f(7.007, 45.416, 0) );
    cam.lookAt( target );

    float angle_of_view = 37.13;
    cam.setFov( angle_of_view - 8.8 );
    cam.setNearClip( 10 );
    cam.setFarClip( 100000000 );

}



