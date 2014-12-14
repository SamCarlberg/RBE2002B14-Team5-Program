/*

	This class represents the field 

 */
#ifndef Map_h
#define Map_h

#include <Arduino.h>
#include <Constants.h>

class Map {

	public:
		// Constructor
		Map();

		// Sets the given location as having an obstacle
		void set(double x, double y, boolean);

		// Returns true if no obstacle has been sensed at the given location
		boolean isOpen(double x, double y);

	private:
		byte width, height;
		byte cellSize;
		byte map[MAP_WIDTH][MAP_HEIGHT];

};

#endif