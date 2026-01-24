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
		std::unique_ptr<OctagramLine> octagramLine5;
		std::unique_ptr<OctagramLine> octagramLine6;
		std::unique_ptr<OctagramLine> octagramLine7;

		bool cursorVisible = false;

		ofTrueTypeFont fontNormal;  // Font untuk custom line labels

		// Interactive Line Creation
		struct CustomLine {
			vec2 fromPos;
			vec2 toPos;
			ofColor color;
			float lineWidth;
		};

		struct DotInfo {
			vec2 position;
			string shapeType;
		};

		enum DrawState {
			IDLE,
			DRAGGING
		};

		DrawState drawState = IDLE;
		vec2 startDotPos = vec2(0, 0);
		vec2 mousePos = vec2(0, 0);
		vector<CustomLine> customLines;

		// Line width control
		float currentLineWidth = 4.0f;  // Current line width

		
		bool sequentialMode = false; // Sequential drawing mode
		int currentShapeIndex = 0;  
		bool sequentialCompleted = false;  // Flag untuk track apakah sequential sudah selesai
		bool labelsVisible = true;      // Flag untuk track label visibility
		bool dotsVisible = true;        // Flag untuk track dot visibility
		float threshold = 10.0f; //radius atau besaran saat mouse hover pada dot
		bool isCtrlPressed = false;

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

		// Interactive Line Creation helpers
		vector<DotInfo> getAllDots();
		bool isMouseOverDot(vec2 mousePos, vec2 dotPos);
		bool lineExists(vec2 from, vec2 to);
		void drawDashedLine(vec2 start, vec2 end, int numDashes = 10);
		void undoLastLine();  // Undo last custom line (CTRL+Z)
		void saveCustomLines();  // Save custom lines to binary file (CTRL+S)
		void loadCustomLines();  // Load custom lines from binary file (CTRL+O)

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
