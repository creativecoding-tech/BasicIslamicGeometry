#pragma once

#include "ofMain.h"
#include <string>

/**
 * DotInfo - Structure untuk informasi dot/point dalam geometry
 * Digunakan untuk mouse hover detection dan interactive line creation
 */
struct DotInfo {
	glm::vec2 position;
	std::string shapeType;
};
