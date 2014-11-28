//
//  ofxGpuNoiseShaders.cpp
//  ofxGpuNoiseExample
//
//  Created by hiroshi matoba on 11/22/14.
//
//

#include "ofxGpuNoiseShaders.h"

#define STR(A) #A

string getPassThruVert(){
    return STR(
                    \n#version 110\n
                     varying vec2 texCoordVarying;
                     void main(){
                         gl_TexCoord[0] = gl_MultiTexCoord0;
                         gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                     }
               );
}

string getNoise3D_Main(){
    return STR(
                    uniform sampler2DRect samplingPointsTexture;
                    uniform int sendSamplingPoints;
                    uniform int octaves;
                    uniform float frame;
                    uniform float freq;
                    uniform float samplingPointsScale;
                    uniform float samplingPointsOffset;

                    void main(){
                        float result = 0.0;
                        float amp = 0.5;
                        vec3 position;
                     
                        if( sendSamplingPoints == 1) {
                            // get sampling point from texture
                            position = texture2DRect( samplingPointsTexture, gl_FragCoord.st ).xyz;
                            position.xy =  position.xy / samplingPointsScale;
                            position.xy -= samplingPointsOffset;
                        }else{
                            position.xy = gl_FragCoord.st;
                        }
                    
                        position.xy *= freq;
                        position.z  = frame;
                         
                        for(int i=0; i<octaves; i++){
                            result += noise3D( position ) * amp;
                            position *= 2.0;
                            amp *= 0.5;
                        }
                         
                        result = (result*0.5) + 0.5;
                        gl_FragColor.rgb = vec3( result, result, result );
                        gl_FragColor.a = 1.0;
                    }
            );
}

string getNoise3D_Deriv_Main(){
    
    return STR(               
                     uniform sampler2DRect samplingPointsTexture;
                     uniform int sendSamplingPoints;
                     uniform int octaves;
                     uniform float frame;
                     uniform float freq;
                     uniform float samplingPointsScale;
                     uniform float samplingPointsOffset;
                     
                     void main(){
                         
                         vec4 result = vec4( 0.0, 0.0, 0.0, 0.0 );
                         float amp = 0.5;
                         vec3 position;
                         
                         if( sendSamplingPoints == 1) {
                             
                             // get sampling point from texture
                             position = texture2DRect( samplingPointsTexture, gl_FragCoord.st ).xyz;
                             position.xy =  position.xy / samplingPointsScale;
                             position.xy -= samplingPointsOffset;
                         }else{
                             position.xy = gl_FragCoord.st;
                         }
                         
                         position.z = frame;
                         position.xy *= freq;
                         
                         for( int i=0; i<octaves; i++ ){
                             result += noise3D_Deriv( position ) * amp;
                             position *= 2.0;
                             amp *= 0.5;
                         }
                         
                         gl_FragColor.rgba = result.xyzw*0.5 + 0.5;
                         //gl_FragColor.a = 1.0;
                     }
               );
}

string getPerlin3D(){
    string n = STR(
                        \n#version 120\n
                        float noise3D( vec3 P ){
                        // establish our grid cell and unit position
                        vec3 Pi = floor(P);
                        vec3 Pf = P - Pi;
                        vec3 Pf_min1 = Pf - 1.0;
                         
                        // clamp the domain
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
                         
                        // calculate the hash
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        Pt = Pt.xzxz * Pt.yyww;
                        const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
                        const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
                        vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
                        vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
                        vec4 hashx0 = fract( Pt * lowz_mod.xxxx );
                        vec4 hashx1 = fract( Pt * highz_mod.xxxx );
                        vec4 hashy0 = fract( Pt * lowz_mod.yyyy );
                        vec4 hashy1 = fract( Pt * highz_mod.yyyy );
                        vec4 hashz0 = fract( Pt * lowz_mod.zzzz );
                        vec4 hashz1 = fract( Pt * highz_mod.zzzz );
                         
                        // calculate the gradients
                        vec4 grad_x0 = hashx0 - 0.49999;
                        vec4 grad_y0 = hashy0 - 0.49999;
                        vec4 grad_z0 = hashz0 - 0.49999;
                        vec4 grad_x1 = hashx1 - 0.49999;
                        vec4 grad_y1 = hashy1 - 0.49999;
                        vec4 grad_z1 = hashz1 - 0.49999;
                        vec4 grad_results_0 = inversesqrt( grad_x0 * grad_x0 + grad_y0 * grad_y0 + grad_z0 * grad_z0 ) * ( vec2( Pf.x, Pf_min1.x ).xyxy * grad_x0 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y0 + Pf.zzzz * grad_z0 );
                        vec4 grad_results_1 = inversesqrt( grad_x1 * grad_x1 + grad_y1 * grad_y1 + grad_z1 * grad_z1 ) * ( vec2( Pf.x, Pf_min1.x ).xyxy * grad_x1 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y1 + Pf_min1.zzzz * grad_z1 );
                         
                        // Classic Perlin Interpolation
                        vec3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
                        vec4 res0 = mix( grad_results_0, grad_results_1, blend.z );
                        vec4 blend2 = vec4( blend.xy, vec2( 1.0 - blend.xy ) );
                        float final = dot( res0, blend2.zxzx * blend2.wwyy );
                        return ( final * 1.1547005383792515290182975610039 );  // scale things to a strict -1.0->1.0 range  *= 1.0/sqrt(0.75)
                    }
                );
    
    return n + getNoise3D_Main();
}

string getPerlin3D_Deriv(){
    string n= STR(
                  \n#version 120\n
                  vec4 noise3D_Deriv( vec3 P ){
                        // establish our grid cell and unit position
                        vec3 Pi = floor(P);
                        vec3 Pf = P - Pi;
                        vec3 Pf_min1 = Pf - 1.0;

                        // clamp the domain
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );

                        // calculate the hash
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        Pt = Pt.xzxz * Pt.yyww;
                        const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
                        const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
                        vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
                        vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
                        vec4 hashx0 = fract( Pt * lowz_mod.xxxx );
                        vec4 hashx1 = fract( Pt * highz_mod.xxxx );
                        vec4 hashy0 = fract( Pt * lowz_mod.yyyy );
                        vec4 hashy1 = fract( Pt * highz_mod.yyyy );
                        vec4 hashz0 = fract( Pt * lowz_mod.zzzz );
                        vec4 hashz1 = fract( Pt * highz_mod.zzzz );

                        //	calculate the gradients
                        vec4 grad_x0 = hashx0 - 0.49999;
                        vec4 grad_y0 = hashy0 - 0.49999;
                        vec4 grad_z0 = hashz0 - 0.49999;
                        vec4 grad_x1 = hashx1 - 0.49999;
                        vec4 grad_y1 = hashy1 - 0.49999;
                        vec4 grad_z1 = hashz1 - 0.49999;
                        vec4 norm_0 = inversesqrt( grad_x0 * grad_x0 + grad_y0 * grad_y0 + grad_z0 * grad_z0 );
                        vec4 norm_1 = inversesqrt( grad_x1 * grad_x1 + grad_y1 * grad_y1 + grad_z1 * grad_z1 );
                        grad_x0 *= norm_0;
                        grad_y0 *= norm_0;
                        grad_z0 *= norm_0;
                        grad_x1 *= norm_1;
                        grad_y1 *= norm_1;
                        grad_z1 *= norm_1;

                        //	calculate the dot products
                        vec4 dotval_0 = vec2( Pf.x, Pf_min1.x ).xyxy * grad_x0 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y0 + Pf.zzzz * grad_z0;
                        vec4 dotval_1 = vec2( Pf.x, Pf_min1.x ).xyxy * grad_x1 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y1 + Pf_min1.zzzz * grad_z1;

                        //	C2 Interpolation
                        vec3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
                        vec3 blendDeriv = Pf * Pf * (Pf * (Pf * 30.0 - 60.0) + 30.0);

                        //  the following is based off Milo Yips derivation, but modified for parallel execution
                        //  http://stackoverflow.com/a/14141774

                        //	Convert our data to a more parallel format
                        vec4 dotval0_grad0 = vec4( dotval_0.x, grad_x0.x, grad_y0.x, grad_z0.x );
                        vec4 dotval1_grad1 = vec4( dotval_0.y, grad_x0.y, grad_y0.y, grad_z0.y );
                        vec4 dotval2_grad2 = vec4( dotval_0.z, grad_x0.z, grad_y0.z, grad_z0.z );
                        vec4 dotval3_grad3 = vec4( dotval_0.w, grad_x0.w, grad_y0.w, grad_z0.w );
                        vec4 dotval4_grad4 = vec4( dotval_1.x, grad_x1.x, grad_y1.x, grad_z1.x );
                        vec4 dotval5_grad5 = vec4( dotval_1.y, grad_x1.y, grad_y1.y, grad_z1.y );
                        vec4 dotval6_grad6 = vec4( dotval_1.z, grad_x1.z, grad_y1.z, grad_z1.z );
                        vec4 dotval7_grad7 = vec4( dotval_1.w, grad_x1.w, grad_y1.w, grad_z1.w );

                        //	evaluate common constants
                        vec4 k0_gk0 = dotval1_grad1 - dotval0_grad0;
                        vec4 k1_gk1 = dotval2_grad2 - dotval0_grad0;
                        vec4 k2_gk2 = dotval4_grad4 - dotval0_grad0;
                        vec4 k3_gk3 = dotval3_grad3 - dotval2_grad2 - k0_gk0;
                        vec4 k4_gk4 = dotval5_grad5 - dotval4_grad4 - k0_gk0;
                        vec4 k5_gk5 = dotval6_grad6 - dotval4_grad4 - k1_gk1;
                        vec4 k6_gk6 = (dotval7_grad7 - dotval6_grad6) - (dotval5_grad5 - dotval4_grad4) - k3_gk3;

                        //	calculate final noise + deriv
                        float u = blend.x;
                        float v = blend.y;
                        float w = blend.z;
                        vec4 result = dotval0_grad0
                        + u * ( k0_gk0 + v * k3_gk3 )
                        + v * ( k1_gk1 + w * k5_gk5 )
                        + w * ( k2_gk2 + u * ( k4_gk4 + v * k6_gk6 ) );
                        result.y += dot( vec4( k0_gk0.x, k3_gk3.x * v, vec2( k4_gk4.x, k6_gk6.x * v ) * w ), vec4( blendDeriv.x ) );
                        result.z += dot( vec4( k1_gk1.x, k3_gk3.x * u, vec2( k5_gk5.x, k6_gk6.x * u ) * w ), vec4( blendDeriv.y ) );
                        result.w += dot( vec4( k2_gk2.x, k4_gk4.x * u, vec2( k5_gk5.x, k6_gk6.x * u ) * v ), vec4( blendDeriv.z ) );
                        return result * 1.1547005383792515290182975610039;  // scale things to a strict -1.0->1.0 range  *= 1.0/sqrt(0.75)
            }
                  );
    return n + getNoise3D_Deriv_Main();
}

string getSimplexPerlin3D(){
    string n = STR(
                   
                   \n#version 120\n
                   float noise3D( vec3 P ) {
                        //  simplex math constants
                        const float SKEWFACTOR = 1.0/3.0;
                        const float UNSKEWFACTOR = 1.0/6.0;
                        const float SIMPLEX_CORNER_POS = 0.5;
                        const float SIMPLEX_TETRAHADRON_HEIGHT = 0.70710678118654752440084436210485;    // sqrt( 0.5 )
                        
                        //  establish our grid cell.
                        P *= SIMPLEX_TETRAHADRON_HEIGHT;    // scale space so we can have an approx feature size of 1.0
                        vec3 Pi = floor( P + dot( P, vec3( SKEWFACTOR) ) );
                        
                        //  Find the vectors to the corners of our simplex tetrahedron
                        vec3 x0 = P - Pi + dot(Pi, vec3( UNSKEWFACTOR ) );
                        vec3 g = step(x0.yzx, x0.xyz);
                        vec3 l = 1.0 - g;
                        vec3 Pi_1 = min( g.xyz, l.zxy );
                        vec3 Pi_2 = max( g.xyz, l.zxy );
                        vec3 x1 = x0 - Pi_1 + UNSKEWFACTOR;
                        vec3 x2 = x0 - Pi_2 + SKEWFACTOR;
                        vec3 x3 = x0 - SIMPLEX_CORNER_POS;
                        
                        //  pack them into a parallel-friendly arrangement
                        vec4 v1234_x = vec4( x0.x, x1.x, x2.x, x3.x );
                        vec4 v1234_y = vec4( x0.y, x1.y, x2.y, x3.y );
                        vec4 v1234_z = vec4( x0.z, x1.z, x2.z, x3.z );
                        
                        // clamp the domain of our grid cell
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
                        
                        //	generate the random vectors
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        vec4 V1xy_V2xy = mix( Pt.xyxy, Pt.zwzw, vec4( Pi_1.xy, Pi_2.xy ) );
                        Pt = vec4( Pt.x, V1xy_V2xy.xz, Pt.z ) * vec4( Pt.y, V1xy_V2xy.yw, Pt.w );
                        const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
                        const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
                        vec3 lowz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi.zzz * ZINC.xyz ) );
                        vec3 highz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi_inc1.zzz * ZINC.xyz ) );
                        Pi_1 = ( Pi_1.z < 0.5 ) ? lowz_mods : highz_mods;
                        Pi_2 = ( Pi_2.z < 0.5 ) ? lowz_mods : highz_mods;
                        vec4 hash_0 = fract( Pt * vec4( lowz_mods.x, Pi_1.x, Pi_2.x, highz_mods.x ) ) - 0.49999;
                        vec4 hash_1 = fract( Pt * vec4( lowz_mods.y, Pi_1.y, Pi_2.y, highz_mods.y ) ) - 0.49999;
                        vec4 hash_2 = fract( Pt * vec4( lowz_mods.z, Pi_1.z, Pi_2.z, highz_mods.z ) ) - 0.49999;
                        
                        //	evaluate gradients
                        vec4 grad_results = inversesqrt( hash_0 * hash_0 + hash_1 * hash_1 + hash_2 * hash_2 ) * ( hash_0 * v1234_x + hash_1 * v1234_y + hash_2 * v1234_z );
                        
                        //	Normalization factor to scale the final result to a strict 1.0->-1.0 range
                        //	http://briansharpe.wordpress.com/2012/01/13/simplex-noise/#comment-36
                        const float FINAL_NORMALIZATION = 37.837227241611314102871574478976;
                        
                        //  evaulate the kernel weights ( use (0.5-x*x)^3 instead of (0.6-x*x)^4 to fix discontinuities )
                        vec4 kernel_weights = v1234_x * v1234_x + v1234_y * v1234_y + v1234_z * v1234_z;
                        kernel_weights = max(0.5 - kernel_weights, 0.0);
                        kernel_weights = kernel_weights*kernel_weights*kernel_weights;
                        
                        //	sum with the kernel and return
                        return dot( kernel_weights, grad_results ) * FINAL_NORMALIZATION;
                    }
                   );
    return n + getNoise3D_Main();
}

string getSimplexPerlin3D_Deriv(){
    string n = STR(
                   
                   \n#version 120\n
                   vec4 noise3D_Deriv(vec3 P){
                        //  simplex math constants
                        const float SKEWFACTOR = 1.0/3.0;
                        const float UNSKEWFACTOR = 1.0/6.0;
                        const float SIMPLEX_CORNER_POS = 0.5;
                        const float SIMPLEX_TETRAHADRON_HEIGHT = 0.70710678118654752440084436210485;    // sqrt( 0.5 )
                        
                        //  establish our grid cell.
                        P *= SIMPLEX_TETRAHADRON_HEIGHT;    // scale space so we can have an approx feature size of 1.0
                        vec3 Pi = floor( P + dot( P, vec3( SKEWFACTOR) ) );
                        
                        //  Find the vectors to the corners of our simplex tetrahedron
                        vec3 x0 = P - Pi + dot(Pi, vec3( UNSKEWFACTOR ) );
                        vec3 g = step(x0.yzx, x0.xyz);
                        vec3 l = 1.0 - g;
                        vec3 Pi_1 = min( g.xyz, l.zxy );
                        vec3 Pi_2 = max( g.xyz, l.zxy );
                        vec3 x1 = x0 - Pi_1 + UNSKEWFACTOR;
                        vec3 x2 = x0 - Pi_2 + SKEWFACTOR;
                        vec3 x3 = x0 - SIMPLEX_CORNER_POS;
                        
                        //  pack them into a parallel-friendly arrangement
                        vec4 v1234_x = vec4( x0.x, x1.x, x2.x, x3.x );
                        vec4 v1234_y = vec4( x0.y, x1.y, x2.y, x3.y );
                        vec4 v1234_z = vec4( x0.z, x1.z, x2.z, x3.z );
                        
                        // clamp the domain of our grid cell
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
                        
                        //	generate the random vectors
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        vec4 V1xy_V2xy = mix( Pt.xyxy, Pt.zwzw, vec4( Pi_1.xy, Pi_2.xy ) );
                        Pt = vec4( Pt.x, V1xy_V2xy.xz, Pt.z ) * vec4( Pt.y, V1xy_V2xy.yw, Pt.w );
                        const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
                        const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
                        vec3 lowz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi.zzz * ZINC.xyz ) );
                        vec3 highz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi_inc1.zzz * ZINC.xyz ) );
                        Pi_1 = ( Pi_1.z < 0.5 ) ? lowz_mods : highz_mods;
                        Pi_2 = ( Pi_2.z < 0.5 ) ? lowz_mods : highz_mods;
                        vec4 hash_0 = fract( Pt * vec4( lowz_mods.x, Pi_1.x, Pi_2.x, highz_mods.x ) ) - 0.49999;
                        vec4 hash_1 = fract( Pt * vec4( lowz_mods.y, Pi_1.y, Pi_2.y, highz_mods.y ) ) - 0.49999;
                        vec4 hash_2 = fract( Pt * vec4( lowz_mods.z, Pi_1.z, Pi_2.z, highz_mods.z ) ) - 0.49999;
                        
                        //	normalize random gradient vectors
                        vec4 norm = inversesqrt( hash_0 * hash_0 + hash_1 * hash_1 + hash_2 * hash_2 );
                        hash_0 *= norm;
                        hash_1 *= norm;
                        hash_2 *= norm;
                        
                        //	evaluate gradients
                        vec4 grad_results = hash_0 * v1234_x + hash_1 * v1234_y + hash_2 * v1234_z;
                        
                        //  evaulate the kernel weights ( use (0.5-x*x)^3 instead of (0.6-x*x)^4 to fix discontinuities )
                        vec4 m = v1234_x * v1234_x + v1234_y * v1234_y + v1234_z * v1234_z;
                        m = max(0.5 - m, 0.0);
                        vec4 m2 = m*m;
                        vec4 m3 = m*m2;
                        
                        //  calc the derivatives
                        vec4 temp = -6.0 * m2 * grad_results;
                        float xderiv = dot( temp, v1234_x ) + dot( m3, hash_0 );
                        float yderiv = dot( temp, v1234_y ) + dot( m3, hash_1 );
                        float zderiv = dot( temp, v1234_z ) + dot( m3, hash_2 );
                        
                        //	Normalization factor to scale the final result to a strict 1.0->-1.0 range
                        //	http://briansharpe.wordpress.com/2012/01/13/simplex-noise/#comment-36
                        const float FINAL_NORMALIZATION = 37.837227241611314102871574478976;
                        
                        //  sum and return all results as a vec3
                        return vec4( dot( m3, grad_results ), xderiv, yderiv, zderiv ) * FINAL_NORMALIZATION;
                    }
                   );
    return n + getNoise3D_Deriv_Main();
}

string getCellular3D(){
    string n = STR(
                   \n#version 120\n
                   float noise3D(vec3 P){
                        //	establish our grid cell and unit position
                        vec3 Pi = floor(P);
                        vec3 Pf = P - Pi;
                        
                        // clamp the domain
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
                        
                        // calculate the hash ( over -1.0->1.0 range )
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        Pt = Pt.xzxz * Pt.yyww;
                        const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
                        const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
                        vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
                        vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
                        vec4 hash_x0 = fract( Pt * lowz_mod.xxxx ) * 2.0 - 1.0;
                        vec4 hash_x1 = fract( Pt * highz_mod.xxxx ) * 2.0 - 1.0;
                        vec4 hash_y0 = fract( Pt * lowz_mod.yyyy ) * 2.0 - 1.0;
                        vec4 hash_y1 = fract( Pt * highz_mod.yyyy ) * 2.0 - 1.0;
                        vec4 hash_z0 = fract( Pt * lowz_mod.zzzz ) * 2.0 - 1.0;
                        vec4 hash_z1 = fract( Pt * highz_mod.zzzz ) * 2.0 - 1.0;
                        
                        //  generate the 8 point positions
                        const float JITTER_WINDOW = 0.166666666;	// 0.166666666 will guarentee no artifacts.
                        hash_x0 = ( ( hash_x0 * hash_x0 * hash_x0 ) - sign( hash_x0 ) ) * JITTER_WINDOW + vec4( 0.0, 1.0, 0.0, 1.0 );
                        hash_y0 = ( ( hash_y0 * hash_y0 * hash_y0 ) - sign( hash_y0 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 1.0, 1.0 );
                        hash_x1 = ( ( hash_x1 * hash_x1 * hash_x1 ) - sign( hash_x1 ) ) * JITTER_WINDOW + vec4( 0.0, 1.0, 0.0, 1.0 );
                        hash_y1 = ( ( hash_y1 * hash_y1 * hash_y1 ) - sign( hash_y1 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 1.0, 1.0 );
                        hash_z0 = ( ( hash_z0 * hash_z0 * hash_z0 ) - sign( hash_z0 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 0.0, 0.0 );
                        hash_z1 = ( ( hash_z1 * hash_z1 * hash_z1 ) - sign( hash_z1 ) ) * JITTER_WINDOW + vec4( 1.0, 1.0, 1.0, 1.0 );
                        
                        //	return the closest squared distance
                        vec4 dx1 = Pf.xxxx - hash_x0;
                        vec4 dy1 = Pf.yyyy - hash_y0;
                        vec4 dz1 = Pf.zzzz - hash_z0;
                        vec4 dx2 = Pf.xxxx - hash_x1;
                        vec4 dy2 = Pf.yyyy - hash_y1;
                        vec4 dz2 = Pf.zzzz - hash_z1;
                        vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
                        vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;
                        d1 = min(d1, d2);
                        d1.xy = min(d1.xy, d1.wz);
                        return min(d1.x, d1.y) * ( 9.0 / 12.0 ); // return a value scaled to 0.0->1.0
                    }
    
                   );
    return n + getNoise3D_Main();
}

string getCellular3D_Deriv(){
    string n = STR(
                   \n#version 120\n
                   vec4 noise3D_Deriv( vec3 P ){
                        //	establish our grid cell and unit position
                        vec3 Pi = floor(P);
                        vec3 Pf = P - Pi;
                        
                        // clamp the domain
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
                        
                        // calculate the hash ( over -1.0->1.0 range )
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        Pt = Pt.xzxz * Pt.yyww;
                        const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
                        const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
                        vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
                        vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
                        vec4 hash_x0 = fract( Pt * lowz_mod.xxxx ) * 2.0 - 1.0;
                        vec4 hash_x1 = fract( Pt * highz_mod.xxxx ) * 2.0 - 1.0;
                        vec4 hash_y0 = fract( Pt * lowz_mod.yyyy ) * 2.0 - 1.0;
                        vec4 hash_y1 = fract( Pt * highz_mod.yyyy ) * 2.0 - 1.0;
                        vec4 hash_z0 = fract( Pt * lowz_mod.zzzz ) * 2.0 - 1.0;
                        vec4 hash_z1 = fract( Pt * highz_mod.zzzz ) * 2.0 - 1.0;
                        
                        //  generate the 8 point positions
                        const float JITTER_WINDOW = 0.166666666;	// 0.166666666 will guarentee no artifacts.
                        hash_x0 = ( ( hash_x0 * hash_x0 * hash_x0 ) - sign( hash_x0 ) ) * JITTER_WINDOW + vec4( 0.0, 1.0, 0.0, 1.0 );
                        hash_y0 = ( ( hash_y0 * hash_y0 * hash_y0 ) - sign( hash_y0 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 1.0, 1.0 );
                        hash_x1 = ( ( hash_x1 * hash_x1 * hash_x1 ) - sign( hash_x1 ) ) * JITTER_WINDOW + vec4( 0.0, 1.0, 0.0, 1.0 );
                        hash_y1 = ( ( hash_y1 * hash_y1 * hash_y1 ) - sign( hash_y1 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 1.0, 1.0 );
                        hash_z0 = ( ( hash_z0 * hash_z0 * hash_z0 ) - sign( hash_z0 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 0.0, 0.0 );
                        hash_z1 = ( ( hash_z1 * hash_z1 * hash_z1 ) - sign( hash_z1 ) ) * JITTER_WINDOW + vec4( 1.0, 1.0, 1.0, 1.0 );
                        
                        //	return the closest squared distance + derivatives ( thanks to Jonathan Dupuy )
                        vec4 dx1 = Pf.xxxx - hash_x0;
                        vec4 dy1 = Pf.yyyy - hash_y0;
                        vec4 dz1 = Pf.zzzz - hash_z0;
                        vec4 dx2 = Pf.xxxx - hash_x1;
                        vec4 dy2 = Pf.yyyy - hash_y1;
                        vec4 dz2 = Pf.zzzz - hash_z1;
                        vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
                        vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;
                        vec4 r1 = d1.x < d1.y ? vec4( d1.x, dx1.x, dy1.x, dz1.x ) : vec4( d1.y, dx1.y, dy1.y, dz1.y );
                        vec4 r2 = d1.z < d1.w ? vec4( d1.z, dx1.z, dy1.z, dz1.z ) : vec4( d1.w, dx1.w, dy1.w, dz1.w );
                        vec4 r3 = d2.x < d2.y ? vec4( d2.x, dx2.x, dy2.x, dz2.x ) : vec4( d2.y, dx2.y, dy2.y, dz2.y );
                        vec4 r4 = d2.z < d2.w ? vec4( d2.z, dx2.z, dy2.z, dz2.z ) : vec4( d2.w, dx2.w, dy2.w, dz2.w );
                        vec4 t1 = r1.x < r2.x ? r1 : r2;
                        vec4 t2 = r3.x < r4.x ? r3 : r4;
                        return ( t1.x < t2.x ? t1 : t2 ) * vec4( 1.0, vec3( 2.0 ) ) * ( 9.0 / 12.0 ); // return a value scaled to 0.0->1.0
                    }
                   );
    return n + getNoise3D_Deriv_Main();
}

string getValue3D(){
    string n = STR(
                   \n#version 120\n
                   
                   float noise3D( vec3 P ){
                        // establish our grid cell and unit position
                        vec3 Pi = floor(P);
                        vec3 Pf = P - Pi;
                        vec3 Pf_min1 = Pf - 1.0;
                        
                        // clamp the domain
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
                        
                        // calculate the hash
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        Pt = Pt.xzxz * Pt.yyww;
                        vec2 hash_mod = vec2( 1.0 / ( 635.298681 + vec2( Pi.z, Pi_inc1.z ) * 48.500388 ) );
                        vec4 hash_lowz = fract( Pt * hash_mod.xxxx );
                        vec4 hash_highz = fract( Pt * hash_mod.yyyy );
                        
                        //	blend the results and return
                        vec3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
                        vec4 res0 = mix( hash_lowz, hash_highz, blend.z );
                        vec4 blend2 = vec4( blend.xy, vec2( 1.0 - blend.xy ) );
                        return dot( res0, blend2.zxzx * blend2.wwyy );
                    }
                   );
    
    return n + getNoise3D_Main();
}

string getValue3D_Deriv(){
    string n = STR(
                   
                   \n#version 120\n
                   vec4 noise3D_Deriv( vec3 P )
                    {
                        //  https://github.com/BrianSharpe/Wombat/blob/master/Value3D_Deriv.glsl
                        
                        // establish our grid cell and unit position
                        vec3 Pi = floor(P);
                        vec3 Pf = P - Pi;
                        vec3 Pf_min1 = Pf - 1.0;
                        
                        // clamp the domain
                        Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
                        vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );
                        
                        // calculate the hash
                        vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
                        Pt *= Pt;
                        Pt = Pt.xzxz * Pt.yyww;
                        vec2 hash_mod = vec2( 1.0 / ( 635.298681 + vec2( Pi.z, Pi_inc1.z ) * 48.500388 ) );
                        vec4 hash_lowz = fract( Pt * hash_mod.xxxx );
                        vec4 hash_highz = fract( Pt * hash_mod.yyyy );
                        
                        //	blend the results and return
                        vec3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
                        vec3 blendDeriv = Pf * Pf * (Pf * (Pf * 30.0 - 60.0) + 30.0);
                        vec4 res0 = mix( hash_lowz, hash_highz, blend.z );
                        vec4 res1 = mix( res0.xyxz, res0.zwyw, blend.yyxx );
                        vec4 res3 = mix( vec4( hash_lowz.xy, hash_highz.xy ), vec4( hash_lowz.zw, hash_highz.zw ), blend.y );
                        vec2 res4 = mix( res3.xz, res3.yw, blend.x );
                        return vec4( res1.x, 0.0, 0.0, 0.0 ) + ( vec4( res1.yyw, res4.y ) - vec4( res1.xxz, res4.x ) ) * vec4( blend.x, blendDeriv );
                    }
                   );
    return n + getNoise3D_Deriv_Main();
}

