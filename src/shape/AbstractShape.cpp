#include "AbstractShape.h"

void AbstractShape::show() {
	showing = true;
}

void AbstractShape::hide() {
	showing = false;
}

void AbstractShape::setLineWidth(float width) {
	lineWidth = width;
}

void AbstractShape::loadFonts(const string& normalPath, const string& boldPath,
							  int normalSize, int boldSize) {
	fontNormal.load(normalPath, normalSize);
	fontBold.load(boldPath, boldSize);
}
