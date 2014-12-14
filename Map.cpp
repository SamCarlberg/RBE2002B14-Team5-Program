#include <Map.h>
#include <Arduino.h>
#include <Constants.h>

Map::Map(): width(FIELD_WIDTH), height(FIELD_HEIGHT) {
	cellSize = FIELD_WIDTH / MAP_WIDTH;
}

void Map::set(double x, double y, byte probability) {
	if(x < (-width / 2) || x > (width / 2) ||
	   y < (-height / 2) ||  y > (height / 2)) {
		Serial.println("Location out of bounds!");
		return;
	}
	byte cellX = (byte) ((x + FIELD_WIDTH / 2) / cellSize);
	byte cellY = (byte) ((y + FIELD_HEIGHT / 2) / cellSize);
	// Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(") to cell: ");
	// Serial.print("["); Serial.print(cellX); Serial.print(", "); Serial.print(cellY); Serial.print("]\n");
	map[cellX][cellY] += probability;
	map[cellX][cellY] = constrain(map[cellX][cellY], 0, 9);
}

boolean Map::isOpen(double x, double y) {
	return map[(byte)(x / cellSize)][(byte)(y / cellSize)];
}

Point Map::toCell(double x, double y) {
	Point p((byte) (x + FIELD_WIDTH / 2) / cellSize, (byte) ((y + FIELD_HEIGHT / 2) / cellSize));
}