#include <Map.h>
#include <Arduino.h>
#include <Constants.h>

Map::Map(): width(FIELD_WIDTH), height(FIELD_HEIGHT) {
	cellSize = 3;
}

boolean Map::set(double x, double y, boolean hasObstacle) {
	Serial.print("Setting "); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print("\n");
	if(x < (-width / 2) || x > (width / 2) ||
	   y < (-height / 2) ||  y > (height / 2)) {
		Serial.println("Location out of bounds!");
		return false;
	}
	byte cellX = (byte) (x / (cellSize * 8)+ MAP_WIDTH / 2);
	byte bitIndex = cellX % 8;
	cellX /= 8;
	byte cellY = (byte) (y / (cellSize * 8) + MAP_HEIGHT / 2);
	// Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(") to cell: ");
	// Serial.print("["); Serial.print(cellX); Serial.print(", "); Serial.print(cellY); Serial.print("]\n");
	bitWrite(map[cellX][cellY], bitIndex, hasObstacle);
	return true;
}

void Map::printMap() {
	Serial.print("Grid:\n");
	for (int j = MAP_HEIGHT - 1; j >= 0; --j) {
		Serial.print('|'); // left hand border
		for (int i = MAP_WIDTH - 1; i >= 0; --i) {
			for(int b = 0; b < 8; b++) {
				if(i == MAP_WIDTH / 2 && j == MAP_HEIGHT / 2 && b == 5) { // center
					Serial.print('O');
				} else if(bitRead(map[i][j], b)) {
					Serial.print(1); // if the value is > 0, print it
				} else {
					Serial.print(' '); // otherwise print an empty space
				}
				Serial.print(' ');
			}
		}
		Serial.print("|\n");
	}
	Serial.print('\n');
}

boolean Map::get(double x, double y) {
	if(x < (-width / 2) || x > (width / 2) ||
	   y < (-height / 2) ||  y > (height / 2)) {
		Serial.println("Location out of bounds!");
		return true;
	}
	byte cellX = (byte) (x / (cellSize * 8)+ MAP_WIDTH / 2);
	byte bitIndex = cellX % 8;
	cellX /= 8;
	byte cellY = (byte) (y / (cellSize * 8) + MAP_HEIGHT / 2);
	return bitRead(cellX, cellY) == 1;	
}