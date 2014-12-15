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

		// Is there an obstacle at (x,y)?
		boolean get(double x, double y);

		// Prints the whole map to serial
		void printMap();

	private:
		byte width, height;
		byte cellSize;
		byte map[MAP_WIDTH][MAP_HEIGHT];

};

#endif