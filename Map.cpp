#include <Map.h>
#include <Arduino.h>
#include <Constants.h>

Map::Map(): width(FIELD_WIDTH), height(FIELD_HEIGHT) {
}

void Map::set(double x, double y, boolean isOpen) {
	if(x > width || y > height) {
		Serial.println("Location out of bounds!");
		return;
	}
	byte cellX = (byte) (x / cellSize);
	byte cellY = (byte) (y / cellSize);
	map[cellX][cellY] = isOpen;
}

boolean Map::isOpen(double x, double y) {
	return map[(byte)(x / cellSize)][(byte)(y / cellSize)];
}