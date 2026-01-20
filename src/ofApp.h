#pragma once

#include "ofMain.h"
#include "shape/CircleShape.h"
#include "shape/CartesianAxes.h"
#include "shape/CrossLine.h"
#include "shape/ParallelogramLine.h"
using glm::vec2;
class ofApp : public ofBaseApp{

	public:
		float radiusCircle = 240;
		//circle attribute
		std::unique_ptr<CircleShape> circleA;
		std::unique_ptr<CircleShape> circleB;
		std::unique_ptr<CircleShape> circleC;
		std::unique_ptr<CircleShape> circleD;
		std::unique_ptr<CircleShape> circleE;
		//cartesian attribute
		std::unique_ptr<CartesianAxes> cartesianAxes;
		//crossline attribute
		std::unique_ptr<CrossLine> crossLineF;
		std::unique_ptr<CrossLine> crossLineG;
		std::unique_ptr<CrossLine> crossLineH;
		std::unique_ptr<CrossLine> crossLineI;
		//parallelogram attribute
		std::unique_ptr<ParallelogramLine> parallelogramCtoE;

		bool cursorVisible = false;

		// Line width control
		float currentLineWidth = 4.0f;  // Current line width

		// Sequential drawing mode
		bool sequentialMode = false;
		int currentShapeIndex = 0;  // 0-9 untuk 10 shapes
		bool sequentialCompleted = false;  // Flag untuk track apakah sequential sudah selesai
		bool labelsVisible = true;      // Flag untuk track label visibility

		void setup();
		void update();
		void draw();

		// Sequential drawing methods
		void startSequentialDrawing();
		void updateSequentialDrawing();

		// Shape control methods
		void toggleLabels();           // Toggle label visibility
		void hideAllShapes();           // Hide all shapes
		void showAllShapes();           // Show all shapes
		void decreaseLineWidth();       // Kurangi line width
		void increaseLineWidth();       // Tambah line width

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
