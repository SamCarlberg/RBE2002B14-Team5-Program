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
		void set(double x, double y, boolean);

		// Returns true if no obstacle has been sensed at the given location
		boolean isOpen(double x, double y);

		Point toCell(double x, double y);

		void printMap() {
			Serial.print("Grid:\n");
			for (int i = MAP_WIDTH - 1; i >= 0; --i) {
				for (int j = MAP_HEIGHT - 1; j >= 0; --j) {
					if(i == MAP_WIDTH / 2 && j == MAP_WIDTH / 2) {
						Serial.print('O');
					} else if(map[i][j]) {
						Serial.print(map[i][j]);
					} else {
						Serial.print(' ');
					}
					// Serial.print(map[i][j]);
					// Serial.print('\t');
					Serial.print(' ');
				}
				Serial.print('\n');
			}
			Serial.print('\n');
		}

	private:
		byte width, height;
		byte cellSize;
		byte map[MAP_WIDTH][MAP_HEIGHT];

};

#endif