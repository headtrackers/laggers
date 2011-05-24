/*
 * Path.h
 *
 *  Created on: May 24, 2011
 *      Author: halukcy
 */

#ifndef PATH_H_
#define PATH_H_

#include "SpaceObject.h"
#include <vector>

using std::vector;

class Path: public SpaceObject {
public:
	Path();
	virtual ~Path();

	void Render();
	void AddPoint(Point3 pt);
	void SetWidth(int w){width=w;}
private:
	vector<Point3> points;
	int width;
};

#endif /* PATH_H_ */
