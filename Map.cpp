#include <Map.h>
#include <Arduino.h>
#include <Constants.h>

const int REAL_OBSTACLE_THRESHOLD = 4;
const int START_THRESHOLD = 3;
const int RANDOM_NOISE_THRESHOLD = 3;

Map::Map() {
    width = FIELD_WIDTH;
    height = FIELD_HEIGHT;
    cellSize = 3;
}


boolean Map::set(double x, double y, boolean hasObstacle) {
    // Serial.println("Setting "); Serial.println(x); Serial.println(", "); Serial.println(y); Serial.println("\n");
    if (x < (-width / 2) || x > (width / 2)
            || y < (-height / 2) || y > (height / 2)) {
        // Serial.println("Location out of bounds!");
        return false;
    }
    x = constrain(x, -width / 2, width / 2 - 1); // make sure we don't go out of bounds
    y = constrain(y, -height / 2, height / 2 - 1);

    int cellX = (int) (x / (cellSize * 8 / BITS_PER_CELL) + MAP_WIDTH / 2);
    int nibbleNum = (int) (x / cellSize) % (8 / BITS_PER_CELL);

    int cellY = (int) (y / (cellSize) + MAP_HEIGHT / 2);

    int hits = (map[cellX][cellY] >> (nibbleNum * 4)) & 15; // grab hits stored in nibble
    if (hits < 14) {
        hits++;
    }
    map[cellX][cellY] &= 15 << ((1 - nibbleNum) * 4);  // clears the value in the nibble
    map[cellX][cellY] |= hits << (nibbleNum * 4); // sets the nibble value to 'hits'
    return true;
}



byte Map::get(double x, double y) {
    if (x < (-width / 2) || x > (width / 2)
            || y < (-height / 2) || y > (height / 2)) {
        // Serial.println("Location out of bounds!");
        return 255; // error
    }
    x = constrain(x, -width / 2, width / 2 - 1); // make sure we don't go out of bounds
    y = constrain(y, -height / 2, height / 2 - 1);

    int cellX = (int) (x / (cellSize * 8 / BITS_PER_CELL) + MAP_WIDTH / 2);
    int nibbleNum = (int) (x / cellSize) % (8 / BITS_PER_CELL);

    int cellY = (int) (y / (cellSize) + MAP_HEIGHT / 2);

    return getValue(x, y, nibbleNum);
}


void Map::printMap() {
    Serial.println("Grid:");
    for (int j = MAP_HEIGHT - 1; j >= 0; --j) {
        for (int i = 0; i < MAP_WIDTH; ++i) {
            for (int b = 0; b < 2; b++) {
                if (i == MAP_WIDTH / 2 && j == MAP_HEIGHT / 2 && b == 0) { // center
                    Serial.print('X');
                } else {
                    Serial.print(getValue(i, j, b));
                }
                Serial.print(',');
            }
        }
        Serial.print('\n');
    }
    Serial.print('\n');
}


void Map::cleanUp() {
	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			for(int nibble = 0; nibble < 2; nibble++) {
				// Sum adjacent cells in cardinal directions (each one includes current cell's value, so subtract it to onyl use 5 cells)
				if((sumVertical(x, y, nibble) + sumHorizontal(x, y, nibble) - getValue(x, y, nibble)) / 5.0 < RANDOM_NOISE_THRESHOLD) {
					setValue(x, y, nibble, 0);
				}
				// find endpoints separated by one cell and fill the gaps
				if(getValue(x, y, nibble) == 15) {
					if(getValue(x + 1, y, nibble) == 15) {
						setValue(x, y, nibble, 14);
						setValue(x, y, nibble + 1, 14);
						setValue(x + 1, y, nibble, 14);
					}
					if(getValue(x, y + 2, nibble) == 15) {
						setValue(x, y, nibble, 14);
						setValue(x, y + 1, nibble, 14);
						setValue(x, y + 2, nibble, 14);
					}
				}

			}
		}
	}
}

void Map::filter() {
    
    findAndFormatHorizontalRuns();
    // printMap();
    findAndFormatVerticalRuns();
    
}

void Map::findAndFormatVerticalRuns() {
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {

            // Loop over nibbles
            for (int nibble = 0; nibble < 2; nibble++) {

                if (isStartVertical(x, y, nibble)) {

                    // Check for vertical runs
                    int max = y;
                    for (int i = y + 1; i < MAP_HEIGHT; i++) {
                        if (sumHorizontal(x, i, nibble) >= REAL_OBSTACLE_THRESHOLD) {
                            max = i;
                        } else if (y < MAP_HEIGHT - 1 && sumHorizontal(x, i + 1, nibble) >= REAL_OBSTACLE_THRESHOLD) {
                            // If this nibble is bad but the next one is good
                            i++;
                            max = i;
                        } else {
                            // Gap is too large, reset max and quit
                            max = i - 1;
                            break;
                        }
                    }
                    for (int j = y; j <= max && max != y; j++) {
                        formatHorizontal(x, j, nibble, j == y || j == max);
                    }
                    y = max + 1;
                    if (y >= MAP_HEIGHT) {
                        break;
                    }
                }
            }
        }
    }        
}

void Map::findAndFormatHorizontalRuns() {        
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            
            // Loop over nibbles
            for (int nibble = 0; nibble < 2; nibble++) {
                if (isStartHorizontal(x, y, nibble)) {
                    // Check for horizontal runs
                    int max = x;
                    for (int cellX = x * 2 + nibble; cellX < MAP_WIDTH * 2; cellX++) {
                        if (sumVertical(cellX / 2, y, cellX % 2) >= REAL_OBSTACLE_THRESHOLD) {
                            max = cellX;
                        } else if (cellX < MAP_WIDTH * 2 - 1 &&
                        		   sumVertical((cellX + 1) / 2, y, (cellX + 1) % 2) >= REAL_OBSTACLE_THRESHOLD) {
                            cellX++;
                            max = cellX;
                        } else {
                            max = cellX - 1;
                            break;
                        }
                    }
                    for (int k = x * 2 + nibble; k <= max && max != x * 2 + nibble; k++) {
                        formatVertical(k / 2, y, k % 2, k == x * 2 + nibble || k == max);
                    }
                    
                    x = (max + 1) / 2;
                    if(x >= MAP_WIDTH) {
                        break;
                    }
                }
            }
        }
    }
}

int Map::sumHorizontal(byte x, byte y, byte nibble) {
    int sum = 0;
    sum += map[x][y] & 15; // adds this int's first nibble
    sum += map[x][y] >> 4; // adds this int's second nibble

    if (nibble == 0 && x > 0) {
        sum += map[x - 1][y] >> 4; // adds previous int's second nibble
    } else if (nibble == 1 && x < MAP_WIDTH - 1) {
        sum += map[x + 1][y] & 15; // adds next int's first nibble
    }

    return sum;
}

int Map::sumVertical(byte x, byte y, byte nibble) {
    int sum = 0;
    int shiftFactor = nibble * 4;
    sum += (map[x][y] >> shiftFactor) & 15;
    if (y > 0) {
        sum += (map[x][y - 1] >> shiftFactor) & 15;
    }
    if (y < MAP_HEIGHT - 1) {
        sum += (map[x][y + 1] >> shiftFactor) & 15;
    }

    return sum;
}

void Map::formatHorizontal(byte x, byte y, byte nibble, boolean isEndPoint) {
    if (nibble == 0) {
        setValue(x, y, nibble, isEndPoint ? 15 : 14);
        if(getValue(x, y, nibble + 1) <= 13) setValue(x, y, nibble + 1, 0); //resets the next nibble to 0
        if (x > 0) {
            if(getValue(x - 1, y, nibble + 1) <= 13) setValue(x - 1, y, nibble + 1, 0);
        }
    }
    if (nibble == 1) {
        setValue(x, y, nibble, isEndPoint ? 15 : 14);
        if(getValue(x, y, nibble - 1) <= 13) setValue(x, y, nibble - 1, 0);
        if (x < MAP_WIDTH - 1) {
            if(getValue(x + 1, y, nibble - 1) <= 13) setValue(x + 1, y, nibble - 1, 0);
        }
    }
}

void Map::formatVertical(byte x, byte y, byte nibble, boolean isEndPoint) {
    setValue(x, y, nibble, isEndPoint ? 15 : 14);
    if (y > 0) {
        setValue(x, y - 1, nibble, 0);
    }
    if (y < MAP_HEIGHT - 1) {
        setValue(x, y + 1, nibble, 0);
    }

}

boolean Map::isStartHorizontal(byte x, byte y, byte nibble) {
    int sum = getValue(x, y, nibble);
    boolean start = sum >= START_THRESHOLD;
    if (y < MAP_HEIGHT - 1) {
        sum += getValue(x, y + 1, nibble);
    }
    return start && (sum >= START_THRESHOLD);
}

boolean Map::isStartVertical(byte x, byte y, byte nibble) {
    int sum = getValue(x, y, nibble);
    boolean start = sum >= START_THRESHOLD;
    if (x * 2 + nibble < MAP_WIDTH * 2 - 1) {
        sum += getValue(x, y, nibble + 1);
    }
    return start && (sum >= START_THRESHOLD);
}

byte Map::getValue(byte x, byte y, byte nibble) {
    x += (int) (nibble / 2);
    nibble %= 2;
    return (byte) ((map[x][y] >> (nibble * 4)) & 15);
}

void Map::setValue(byte x, byte y, byte nibble, byte value) {
    map[x][y] &= (15 << ((1 - nibble) * 4));
    map[x][y] |= (value << (nibble * 4));
}
