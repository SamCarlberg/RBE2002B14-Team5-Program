#include <Map.h>
#include <Arduino.h>
#include <Constants.h>

#define REAL_OBSTACLE_THRESHOLD 7 // half a nibble

Map::Map(): width(FIELD_WIDTH), height(FIELD_HEIGHT) {
	cellSize = 3;
}

boolean Map::set(double x, double y, boolean hasObstacle) {
	// Serial.print("Setting "); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print("\n");
	if(x < (-width / 2) || x > (width / 2) ||
	   y < (-height / 2) ||  y > (height / 2)) {
		Serial.println("Location out of bounds!");
		return false;
	}
	x = constrain(x, -width / 2, width / 2 - 1); // make sure we don't go out of bounds
	y = constrain(y, -height / 2, height / 2 - 1);

	byte cellX = (byte) (x / (cellSize * 8 / BITS_PER_CELL) + MAP_WIDTH / 2);
	byte nibbleNum = cellX % (8 / BITS_PER_CELL);
	cellX /= (8 / BITS_PER_CELL);

	byte cellY = (byte) (y / (cellSize) + MAP_HEIGHT / 2);

	byte hits = (map[cellX][cellY] >> (nibbleNum * 4)) & 15; // grab hits stored in nibble
	if(hits < 15) {
		hits++;
	}
	map[cellX][cellY] &= 15 << (!nibbleNum * 4);  // clears the value in the nibble
	map[cellX][cellY] |= hits << (nibbleNum * 4); // sets the nibble value to 'hits'


	Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(") to cell: ");
	Serial.print("["); Serial.print(cellX); Serial.print('-'); Serial.print(nibbleNum); Serial.print(", "); Serial.print(cellY); Serial.print("]");
	Serial.print(" Hits = "); Serial.print(hits); Serial.print('\n');

	return true;
}

void Map::printMap() {
	Serial.print("Grid:\n");
	for (int j = MAP_HEIGHT - 1; j >= 0; --j) {
		Serial.print('|'); // left hand border
		for (int i = 0; i < MAP_WIDTH; ++i) {
			for(int b = 0; b < BITS_PER_CELL; b++) {
				if(i == MAP_WIDTH / 2 && j == MAP_HEIGHT / 2 && b == BITS_PER_CELL / 2) { // center
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



void Map::filter() {

	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {

			// Loop over nibbles
			for(int nibble = 0; nibble < 2; nibble++) {

				if((map[x][y] << (nibble * 4)) & 15 >= REAL_OBSTACLE_THRESHOLD) {

					// Check for vertical runs
					int max = y;
					for(int i = y + 1; i < MAP_HEIGHT; i++) {
						if(sumHorizontal(x, i, nibble) >= REAL_OBSTACLE_THRESHOLD) {
							max = i;
						} else if(y < MAP_HEIGHT - 1 && sumHorizontal(x, i + 1, nibble) >= REAL_OBSTACLE_THRESHOLD) {
							// If this nibble is bad but the next one is good
							i++;
							max = i;
						} else {
							// Gap is too large, reset max and quit
							max = i - 1;
							break;
						}
					}
					for(int j = y; j <= max && max != y; j++) {
						formatHorizontal(x, j, nibble, j == y || j == max);
					}

					// Check for horizontal runs
					max = x;
					for(int cellX = x * 2 + nibble; cellX < MAP_WIDTH * 2; cellX++) {
						if(sumVertical(cellX / 2, y, cellX % 2) >= REAL_OBSTACLE_THRESHOLD) {
							max = cellX;
						}
						else if(cellX < MAP_WIDTH * 2 - 1 && sumVertical((cellX + 1) / 2, y, (cellX + 1) % 2)) {
							cellX++;
							max = cellX;
						} else {
							max = cellX - 1;
							break;
						}
					}

					for(int k = x * 2 + nibble; k <= max && max != x * 2 + nibble; k++) {
						formatVertical(k / 2, y, k % 2, k == x * 2 + nibble || k == max);
					}
				}
			}
		}
	}

}

int Map::sumHorizontal(byte x, byte y, byte nibble) {
	int sum = 0;
	sum += map[x][y] & 15; // adds this byte's first nibble
	sum += map[x][y] >> 4; // adds this byte's second nibble

	if(nibble == 0 && x > 0) {
		sum += map[x - 1][y] >> 4; // adds previous byte's second nibble
	} else if(nibble == 1 && x < MAP_WIDTH - 1) {
		sum += map[x + 1][y] & 15; // adds next byte's first nibble
	}

	return sum;
}

int Map::sumVertical(byte x, byte y, byte nibble) {
	int sum = 0;
	byte shiftFactor = nibble * 4;
	sum += (map[x][y] >> shiftFactor) & 15;
	if(y > 0) {
		sum += (map[x][y - 1] >> shiftFactor) & 15;
	}
	if(y < MAP_HEIGHT - 1) {
		sum += (map[x][y + 1] >> shiftFactor) & 15;
	}

	return sum;
}

void Map::formatHorizontal(byte x, byte y, byte nibble, boolean isEndPoint) {
	if(nibble == 0) {
		map[x][y] |= isEndPoint ? 15 : 14; // makes current nibble 14 or 15
		map[x][y] &= 15; //resets the next nibble to 0
		if(x > 0) {
			map[x - 1][y] &= 15; // resets previous nibble to 0
		}
	}
	if(nibble == 1) {
		map[x][y] |= isEndPoint ? (15 << 4) : (14 << 4); // make current nibble 14 or 15
		map[x][y] &= (15 << 4); // resets the next nibble to 0
		if(x < MAP_WIDTH - 1) {
			map[x + 1][y] &= (15 << 4); // resets previous nibble to 0
		}
	}
}

void Map::formatVertical(byte x, byte y, byte nibble, boolean isEndPoint) {
	byte shiftFactor = nibble * 4;
	map[x][y] |= isEndPoint ? (15 << shiftFactor) : (14 << shiftFactor); // makes current nibble 14 or 15
	if(y > 0) {
		map[x][y - 1] &= ~(15 << shiftFactor); // resets previous nibble to 0
	}
	if(y < MAP_HEIGHT - 1) {
		map[x][y + 1] &= ~(15 << shiftFactor);
	}

}
