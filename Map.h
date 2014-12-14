/*

	This class represents the field 

 */
#ifndef Map_h
#define Map_h

#include <Arduino.h>
#include <Constants.h>
#include <Point.h>

class Map {

	public:
		// Constructor
		Map();

		// Sets the given location as having an obstacle
		boolean set(double x, double y, boolean);

		// Returns true if no obstacle has been sensed at the given location
		boolean isOpen(double x, double y);

		Point toCell(double x, double y);

		void printMap();

	private:
		byte width, height;
		byte cellSize;
		byte map[MAP_WIDTH][MAP_HEIGHT];

};

#endif