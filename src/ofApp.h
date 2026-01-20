#pragma once

#include "ofMain.h"
#include "shape/CircleShape.h"
#include "shape/CartesianAxes.h"
#include "shape/CrossLine.h"
using glm::vec2;
class ofApp : public ofBaseApp{

	public:
		float radiusCircle = 240;
		std::unique_ptr<CircleShape> circleA;
		std::unique_ptr<CircleShape> circleB;
		std::unique_ptr<CircleShape> circleC;
		std::unique_ptr<CircleShape> circleD;
		std::unique_ptr<CircleShape> circleE;
		std::unique_ptr<CartesianAxes> cartesianAxes;
		std::unique_ptr<CrossLine> crossLineF;
		std::unique_ptr<CrossLine> crossLineG;
		std::unique_ptr<CrossLine> crossLineH;
		std::unique_ptr<CrossLine> crossLineI;
		bool cursorVisible = false;
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
