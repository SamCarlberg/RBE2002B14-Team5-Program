#include <Map.h>
#include <Arduino.h>
#include <Constants.h>

Map::Map(): width(FIELD_WIDTH), height(FIELD_HEIGHT) {
	cellSize = FIELD_WIDTH / MAP_WIDTH;
}

boolean Map::set(double x, double y, byte probability) {
	if(x < (-width / 2) || x > (width / 2) ||
	   y < (-height / 2) ||  y > (height / 2)) {
		Serial.println("Location out of bounds!");
		return false;
	}
	byte cellX = (byte) ((x + FIELD_WIDTH / 2) / cellSize);
	byte cellY = (byte) ((y + FIELD_HEIGHT / 2) / cellSize);
	// Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(") to cell: ");
	// Serial.print("["); Serial.print(cellX); Serial.print(", "); Serial.print(cellY); Serial.print("]\n");
	map[cellX][cellY] += probability;
	map[cellX][cellY] = constrain(map[cellX][cellY], 0, 9);
	return true;
}

boolean Map::isOpen(double x, double y) {
	return map[(byte)(x / cellSize)][(byte)(y / cellSize)];
}

Point Map::toCell(double x, double y) {
	Point p((byte) (x + FIELD_WIDTH / 2) / cellSize, (byte) ((y + FIELD_HEIGHT / 2) / cellSize));
}

void Map::printMap() {
	Serial.print("Grid:\n");
	for (int i = MAP_WIDTH - 1; i >= 0; --i) {
		Serial.print('|'); // left hand border
		for (int j = MAP_HEIGHT - 1; j >= 0; --j) {
		if(i == MAP_WIDTH / 2 && j == MAP_WIDTH / 2) { // center
				Serial.print('O');
			} else if(map[i][j]) {
				Serial.print(map[i][j]); // if the value is > 0, print it
			// } else if(i == j) {
			// 	Serial.print('X');
			} else {
				Serial.print(' '); // otherwise print an empty space
			}
			Serial.print(' ');
		}
		Serial.print("|\n"); // right hand border
	}
	Serial.print('\n');
}

