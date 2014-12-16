/*
	Class representing a 2D point.
 */

#ifndef Point_h
#define Point_h

class Point {
	public:
		Point(): x(0), y(0){}
		Point(double _x, double _y): x(_x), y(_y) {}
		double x, y;
};

#endif