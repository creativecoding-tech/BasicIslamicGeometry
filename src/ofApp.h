#pragma once

#include "ofMain.h"
#include "shape/AbstractShape.h"
#include "shape/CustomLine.h"
#include "shape/PolygonShape.h"
#include "shape/DotInfo.h"
#include "operation/FileManager.h"
#include "operation/gui/AbstractGuiComponent.h"
#include "operation/gui/MenuBar.h"
#include "operation/gui/SacredGeometry.h"
#include "operation/gui/Playground.h"
#include "operation/gui/SuccessPopup.h"
#include "operation/gui/ErrorPopup.h"
#include "template/SacredGeometryTemplate.h"
#include "template/TemplateRegistry.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_win32.h"
#include <set>
using glm::vec2;

class ofApp : public ofBaseApp{

	public:
		// UPDATE STATE - Strategy Pattern untuk Update Logic
		enum class UpdateState {
			NORMAL,              // Normal update mode
			SEQUENTIAL_DRAWING,  // Template sequential drawing mode
			DELAYED_LOAD,        // Delayed load waiting for timer
			STAGGERED_LOAD       // Staggered parallel load mode
		};
		UpdateState currentState = UpdateState::NORMAL;

		// TEMPLATE SYSTEM - Template sekarang self-contained!
		// Template owns shapes sendiri, handle draw/update sendiri
		SacredGeometryTemplate* currentTemplate = nullptr;  // Template yang sedang aktif
		float previousRadius = 0;  // Untuk tracking perubahan radius (scaling customLines & polygons)

		bool cursorVisible = false;
		bool imguiVisible = false;  // ImGui menu visibility

		ofTrueTypeFont fontNormal;  // Font untuk custom line labels

		// DotInfo sekarang didefinisikan di src/shape/DotInfo.h (common struct)

		enum DrawState {
			IDLE,
			DRAGGING
		};

		DrawState drawState = IDLE;
		vec2 startDotPos = vec2(0, 0);
		vec2 mousePos = vec2(9999, 9999);
		vector<vec2> currentPolylinePoints;  // Capture points saat drag untuk polyline
		vector<CustomLine> customLines;  // CustomLine dari FileManager

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
		float mouseLineWidth = 3.f;    // Line width khusus untuk mouse drag lines

		// CustomLine color control
		ofColor customLineColor = ofColor(0, 0, 255);  // Default biru

		float threshold = 10.0f; //dalam radius saat mouse hover pada dot
		bool isCtrlPressed = false;

		// Staggered parallel load mode (CTRL+O)
		enum LoadStage {
			LOAD_TEMPLATE,      // Stage 1: Draw template shapes paralel
			LOAD_CUSTOMLINES,   // Stage 2: Draw customLines paralel
			LOAD_POLYGONS,      // Stage 3: Draw polygons paralel
			LOAD_DONE           // Stage 4: Selesai
		};
		LoadStage loadStage = LOAD_DONE;  // Current load stage
		bool isStaggeredLoad = false;     // Flag untuk staggered parallel mode
		bool isSequentialShapeLoad = false;  // Flag untuk sequential load per shape (CTRL+SHIFT+O)
		int currentTemplateIndex = 0;     // Index template shape yang sedang di-animate (untuk sequential)

		// Cached dots untuk performance
		std::vector<DotInfo> cachedDots;
		bool dotsCacheDirty = true;  // Flag untuk rebuild cache

		// File Manager untuk save/load custom lines
		FileManager fileManager;
		std::string lastSavedPath;  // Path file terakhir yang di-save/open (untuk Save As dan Load)

		// Play button delay state
		bool isWaitingForLoad = false;
		float loadDelayTimer = 0.0f;
		int pendingLoadMode = -1;  // 0 = Parallel, 1 = Sequential

		// GUI Components
		std::vector<std::unique_ptr<AbstractGuiComponent>> guiComponents;
		std::unique_ptr<SuccessPopup> successPopup;  // Success popup dialog
		std::unique_ptr<ErrorPopup> errorPopup;  // Error popup dialog

		void setup();
		void setupTemplateSystem();  // Register semua templates ke registry
		void switchTemplate(const std::string& templateName);  // Switch ke template tertentu
		void update();
		void draw();
		void drawCustomLinesAndUI();  // Draw custom lines, curve label, preview, dan dot highlights

		void setupImGui();
		void drawImGui();
		void exitImGui();

		// UPDATE STRATEGIES - Strategy Pattern untuk Update Logic
		void updateNormal();           // Normal update mode
		void updateSequentialDrawing(); // Template sequential drawing mode
		void updateDelayedLoad();       // Delayed load waiting for timer
		void updateStaggeredLoad();     // Staggered parallel load mode

		// Staggered load helpers
		void updateStaggeredTemplate();    // LOAD_TEMPLATE stage
		void updateStaggeredCustomLines(); // LOAD_CUSTOMLINES stage
		void updateStaggeredPolygons();    // LOAD_POLYGONS stage

		// Normal update helpers
		void updateTemplateShapes();  // Update template di normal mode
		void updateScaling();         // Handle radius scaling
		void updateCustomLines();     // Update custom lines
		void updatePolygons();        // Update polygons

		// Sequential drawing methods
		void startSequentialDrawing();
		// NOTE: updateSequentialDrawing() removed - template handles it autonomously

		// Shape control methods
		void toggleLabels();           // Toggle label visibility
		void toggleDots();             // Toggle dot visibility
		void setCartesianAxesVisibility(bool show);  // Show/hide CartesianAxes
		void hideAllShapes();           // Hide all shapes
		void showAllShapes();           // Show all shapes
		void updateLineWidth();
		void updateCustomLineColor(ofColor color);  // Update warna semua customLines
		void syncColorPickerFromLoadedLines();  // Sync ColorPicker dari customLines yang diload
		void cleanCanvas();             // Clear all polygons, custom lines, and hide template shapes
		void scaleCustomLinesAndPolygons(float oldRadius, float newRadius);  // Scale customLines & polygons saat radius berubah
		bool isCanvasEmpty();  // Cek apakah canvas benar-bener kosong (tidak ada template showing, customLines, atau polygons)

		// File operations
		void saveWorkspace();          // Save workspace to file
		void saveWorkspaceAs();        // Save workspace to custom location
		void openWorkspace();          // Open file dialog dengan validasi .nay
		void loadWorkspace();          // Load workspace from file
		void loadWorkspaceSeq();          // Load workspace seq from file

		// Interactive Line Creation helpers
		const std::vector<DotInfo>& getAllDots();  // Return cached dots by reference
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
		void exit();

};
