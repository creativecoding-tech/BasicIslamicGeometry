#pragma once

#include "ofMain.h"
#include "shape/CircleShape.h"
#include "shape/CartesianAxes.h"
#include "shape/CrossLine.h"
#include "shape/ParallelogramLine.h"
#include "shape/RectangleLine.h"
#include "shape/OctagramLine.h"
#include "shape/CustomLine.h"
#include "shape/PolygonShape.h"
#include "operation/FileManager.h"
#include <set>
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
		vec2 mousePos = vec2(9999, 9999);
		vector<vec2> currentPolylinePoints;  // Capture points saat drag untuk polyline
		vector<CustomLine> customLines;  // CustomLine dari FileManager

		// Line selection (SATU SYSTEM UNTUK SEMUA!)
		std::set<int> selectedLineIndices;  // Bisa select 1 atau banyak garis
		int lastSelectedLineIndex = -1;  // Untuk track line terakhir di-klik

		// Invisible polygon system
		vector<PolygonShape> polygonShapes;
		int selectedPolygonIndex = -1;

		// Preset warna untuk polygon
		vector<ofColor> polygonPresetColors = {
			ofColor(255, 0, 0, 255),      // 1: Merah (OPAQUE!)
			ofColor(0, 255, 0, 255),      // 2: Hijau (OPAQUE!)
			ofColor(0, 0, 255, 255),      // 3: Biru (OPAQUE!)
			ofColor(255, 255, 0, 255),    // 4: Kuning (OPAQUE!)
			ofColor(255, 0, 255, 255),    // 5: Magenta (OPAQUE!)
			ofColor(0, 255, 255, 255),    // 6: Cyan (OPAQUE!)
			ofColor(255, 128, 0, 255),    // 7: Orange (OPAQUE!)
			ofColor(128, 0, 255, 255),    // 8: Ungu (OPAQUE!)
			ofColor(128, 128, 128, 255)   // 9: Abu-abu (OPAQUE!)
		};

		// Line width control
		float currentLineWidth = 4.0f;  // Current line width untuk shapes
		float mouseLineWidth = 3.f;    // Line width khusus untuk mouse drag lines

		bool sequentialMode = false; // Sequential drawing mode
		int currentShapeIndex = 0;
		bool sequentialCompleted = false;  // Flag untuk track apakah sequential sudah selesai
		bool labelsVisible = true;      // Flag untuk track label visibility
		bool dotsVisible = true;        // Flag untuk track dot visibility
		float threshold = 10.0f; //dalam radius saat mouse hover pada dot
		bool isCtrlPressed = false;

		// Cached dots untuk performance
		vector<DotInfo> cachedDots;
		bool dotsCacheDirty = true;  // Flag untuk rebuild cache

		// File Manager untuk save/load custom lines
		FileManager fileManager;

		void setup();
		void setupCircles();
		void setupCartesianAxes();
		void setupCrossLines();
		void setupParallelograms();
		void setupRectangleLine();
		void setupOctagramLine();
		void update();
		void draw();
		void drawCustomLinesAndUI();  // Draw custom lines, curve label, preview, dan dot highlights

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
		const vector<DotInfo>& getAllDots();  // Return cached dots by reference
		void updateDotsCache();  // Rebuild dots cache saat visibility berubah
		bool isMouseOverDot(vec2 mousePos, vec2 dotPos);
		bool isMouseOverLine(vec2 mousePos, vec2 lineStart, vec2 lineEnd, float lineWidth);
		float distanceToLine(vec2 point, vec2 lineStart, vec2 lineEnd, float curve = 0.0f);
		int getLineIndexAtPosition(vec2 pos);  // Get index line di posisi mouse (-1 jika tidak ada)
		bool lineExists(vec2 from, vec2 to);
		void undoLastLine();  // Undo last custom line (CTRL+Z)
		void createInvisiblePolygonFromSelected();  // Create invisible polygon from selected lines (CTRL+G)

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseScrolled(int x, int y, float scrollX, float scrollY);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

};
