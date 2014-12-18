//
//  ad_geom_util.cpp
//  dist_PL_test
//
//  Created by mtb on 09/11/14.
//
//

#include "ad_geom_util.h"

float ad_geom_util::dist_pl( ofVec3f p0, ofVec3f p1, ofVec3f p2){
	
	/*
	 *	http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
	 *
	 *	d = |(x0-x1) x (x0-x2)| / |x2-x1|
	 *
	 */
	
	return (p0-p1).cross((p0-p2)).length() / (p2-p1).length();
}

/*
 *		return vector from Point(p0) to line(p1-p2)
 */
ofVec3f ad_geom_util::vec_pl( ofVec3f p0, ofVec3f p1, ofVec3f p2){
	
	/*
	 *		http://geomalgorithms.com/a02-_lines.html#dist_Point_to_Line()
	 *		p1を起点に計算
	 */
	
	ofVec3f v = p2-p1;
	ofVec3f w = p0-p1;
	double b = v.dot(w) / v.dot(v);
	return -w + b*v;
}
