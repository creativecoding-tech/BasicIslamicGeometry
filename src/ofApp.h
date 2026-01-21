#pragma once

#include "ofMain.h"
#include "shape/CircleShape.h"
#include "shape/CartesianAxes.h"
#include "shape/CrossLine.h"
#include "shape/ParallelogramLine.h"
#include "shape/RectangleLine.h"
#include "shape/OctagramLine.h"
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
		std::unique_ptr<ParallelogramLine> parallelogramEtoB;
		std::unique_ptr<ParallelogramLine> parallelogramBtoD;
		std::unique_ptr<ParallelogramLine> parallelogramDtoC;
		//rectangle attribute
		std::unique_ptr<RectangleLine> rectangleLineFtoG;
		std::unique_ptr<RectangleLine> rectangleLineGtoI;
		std::unique_ptr<RectangleLine> rectangleLineItoH;
		std::unique_ptr<RectangleLine> rectangleLineHtoF;
		//octagram attribute
		std::unique_ptr<OctagramLine> octagramLine0;
		std::unique_ptr<OctagramLine> octagramLine1;
		std::unique_ptr<OctagramLine> octagramLine2;
		std::unique_ptr<OctagramLine> octagramLine3;
		std::unique_ptr<OctagramLine> octagramLine4;

		bool cursorVisible = false;

		// Line width control
		float currentLineWidth = 4.0f;  // Current line width

		// Sequential drawing mode
		bool sequentialMode = false;
		int currentShapeIndex = 0;  
		bool sequentialCompleted = false;  // Flag untuk track apakah sequential sudah selesai
		bool labelsVisible = true;      // Flag untuk track label visibility
		bool dotsVisible = true;        // Flag untuk track dot visibility

		void setup();
		void setupCircles();
		void setupCartesianAxes();
		void setupCrossLines();
		void setupParallelograms();
		void setupRectangleLine();
		void setupOctagramLine();
		void update();
		void draw();

		// Sequential drawing methods
		void startSequentialDrawing();
		void updateSequentialDrawing();

		// Shape control methods
		void toggleLabels();           // Toggle label visibility
		void toggleDots();             // Toggle dot visibility
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
