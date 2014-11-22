//
//  ofxGpuNoiseShaders.h
//  ofxGpuNoiseExample
//
//  Created by hiroshi matoba on 11/22/14.
//
//

//  Wombat
//  An efficient texture-free GLSL procedural noise library
//  Source: https://github.com/BrianSharpe/Wombat
//  Derived from: https://github.com/BrianSharpe/GPU-Noise-Lib
//

#pragma once

#include <iostream>
#include <string>
using namespace std;

string getPassThruVert();
string getNoise3D_Main();
string getNoise3D_Deriv_Main();

string getPerlin3D();
string getPerlin3D_Deriv();
string getSimplexPerlin3D();
string getSimplexPerlin3D_Deriv();
string getCellular3D();
string getCellular3D_Deriv();
string getValue3D();
string getValue3D_Deriv();

