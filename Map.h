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

		// Gets the value at the given "real" coordinates that are passed
		byte get(double, double);
		
		// Filters noise and finds endpoints
		void filter();

		// Called after filter, this removes any stray error values below some threshold
		void cleanUp();

		// Prints the whole map to serial
		void printMap();

		// Gets the value at the give (x, y, nibble)
		byte getValue(byte, byte, byte);

	private:
		// Sums horizontal values adjacent to the given cell
		int sumHorizontal(byte, byte, byte);

		// Sums vertical values adjacent to the given cell
		int sumVertical(byte, byte, byte);

		// Sets the value of the given cell and adjacent horizontal cells to smooth noise
		void formatHorizontal(byte, byte, byte, boolean);

		// Sets the value of the given cell and adjacent vertical cells to smooth noise
		void formatVertical(byte, byte, byte, boolean);

		// Finds all horizontal runs and formats them
		void findAndFormatHorizontalRuns();

		// Finds all vertical runs and formats them
		void findAndFormatVerticalRuns();

		// Checks if the cell at the given (x, y, nibble) is the start of a horizontal run
		boolean isStartHorizontal(byte, byte, byte);
		
		// Checks if the cell at the given (x, y, nibble) is the start of a vertical run
		boolean isStartVertical(byte, byte, byte);

		// Sets the value a tthe given (x, y, nibble)
		void setValue(byte, byte, byte, byte);

		byte width, height;
		byte cellSize;
		byte map[MAP_WIDTH][MAP_HEIGHT];

};

#endif