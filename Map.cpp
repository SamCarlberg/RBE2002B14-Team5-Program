#include <Map.h>
#include <Arduino.h>
#include <Constants.h>

Map::Map(byte w, byte h): width(w), height(h) {
	map = new boolean*[height];
	for (int i = 0; i < height; i++) { // populate the 2D array
		map[i] = new byte[width];
	}
}

void Map::set(double x, double y, boolean isOpen) {
	byte cellX = (byte) (x / cellSize);
	byte cellY = (byte) (y / cellSize);
	map[cellX][cellY] = isOpen;
}

boolean Map::isOpen(double x, double y) {
	return map[(byte)(x / cellSize)][(byte)(y / cellSize)];
}