#include "ofApp.h"
#include "shape/AbstractShape.h"
#include "shape/DotShape.h"
#include "template/templates/BasicZelligeTemplate.h"
#include "operation/gui/SacredGeometry.h"
#include "operation/gui/UserCustom.h"
#include "operation/FileOperationManager.h"
#include "utils/GeometryUtils.h"
#include <vector>


//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(false);
  ofSetFrameRate(60);
  ofSetEscapeQuitsApp(false);
  ofSetBackgroundAuto(false);
  ofEnableAntiAliasing();
  ofEnableSmoothing();
  ofDisableArbTex();  // Required for FBO texture sampling in FillAnimation

  // Load font untuk custom line labels
  fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);

  // Setup template system (REGISTER SEMUA TEMPLATES)
  setupTemplateSystem();

  // Set default template (Basic Zellige) TANPA otomatis setupShapes()
  // User harus klik radio button "Parallel" atau "Sequential" untuk draw
  TemplateRegistry &registry = TemplateRegistry::getInstance();
  currentTemplate = registry.getTemplate("Basic Zellige");

  // Initial dots cache build
  dotsCacheDirty = true;

  // Initialize ColorManager (after ofApp is fully constructed)
  colorManager = std::make_unique<ColorManager>(this);

  // Initialize DuplicateManager (after ofApp is fully constructed)
  duplicateManager = std::make_unique<DuplicateManager>(this);

  // Initialize InputManager (after ofApp is fully constructed)
  inputManager = std::make_unique<InputManager>(this);

  // Initialize FileOperationManager (after ofApp is fully constructed)
  fileOperationManager = std::make_unique<FileOperationManager>(this);

  //define ImGUI
  setupImGui();
}

//--------------------------------------------------------------
void ofApp::setupTemplateSystem() {
  // Get template registry instance
  TemplateRegistry &registry = TemplateRegistry::getInstance();

  // Register semua available templates
  registry.registerTemplate(std::make_unique<BasicZelligeTemplate>());

  // Nanti kalau ada template lain, tambahkan di sini:
  // registry.registerTemplate(std::make_unique<FlowerOfLifeTemplate>());
  // registry.registerTemplate(std::make_unique<StarTemplate>());
}

//--------------------------------------------------------------
void ofApp::switchTemplate(const std::string &templateName) {
  // Get template dari registry
  TemplateRegistry &registry = TemplateRegistry::getInstance();
  SacredGeometryTemplate* oldTemplate = currentTemplate;
  currentTemplate = registry.getTemplate(templateName);

  // Null check - template tidak ditemukan
  if (!currentTemplate) {
    // Template tidak ditemukan, jangan lanjut
    return;
  }

  // Keep previous radius setting if available
  if (oldTemplate) {
      currentTemplate->radius = oldTemplate->radius;  // Copy radius dari template lama
  }

  // Setup shapes SAH radius di-set (REQUIRED!)
  currentTemplate->setupShapes();

  // Reset dots cache agar di-rebuild
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::update() {
  // STRATEGY PATTERN: Delegate update ke method yang sesuai berdasarkan state
  switch (currentState) {
    case UpdateState::NORMAL:
      updateNormal();
      break;

    case UpdateState::SEQUENTIAL_DRAWING:
      updateSequentialDrawing();
      break;

    case UpdateState::DELAYED_LOAD:
      updateDelayedLoad();
      break;

    case UpdateState::STAGGERED_LOAD:
      updateStaggeredLoad();
      break;
  }
}

void ofApp::updateNormal() {
  // Template sequential drawing
  if (currentTemplate && currentTemplate->sequentialMode) {
    float deltaTime = ofGetLastFrameTime();
    bool complete = currentTemplate->updateSequentialDrawing(deltaTime);
    if (complete) {
      dotsCacheDirty = true;
    }
  } else {
    // Hanya update semua template shapes jika TIDAK sequential mode
    updateTemplateShapes();
  }

  // Rebuild dots cache
  dotsCacheDirty = true;

  // Handle scaling
  updateScaling();

  // Update custom lines & polygons
  updateCustomLines();
  updatePolygons();

  // Update user-created dots
  for (auto& dot : userDots) {
    if (dot) {
      dot->update();
    }
  }
}

//--------------------------------------------------------------
void ofApp::updateSequentialDrawing() {
  // Template fully autonomous - template handles sequential drawing
  if (currentTemplate && currentTemplate->sequentialMode) {
    float deltaTime = ofGetLastFrameTime();
    bool complete = currentTemplate->updateSequentialDrawing(deltaTime);
    if (complete) {
      dotsCacheDirty = true;
      currentState = UpdateState::NORMAL;  // Kembali ke normal setelah selesai
    }
  }
}

//--------------------------------------------------------------
void ofApp::updateDelayedLoad() {
  // Check play button delay timer
  if (!isWaitingForLoad) {
    currentState = UpdateState::NORMAL;
    return;
  }

  // Accumulate deltaTime untuk smooth delay
  float deltaTime = ofGetLastFrameTime();
  loadDelayAccumulator += deltaTime;

  if (loadDelayAccumulator >= loadDelayDuration) {
    // Timer selesai, panggil load method
    if (pendingLoadMode == 0) {
      fileOperationManager->loadWorkspace();
    } else if (pendingLoadMode == 1) {
      fileOperationManager->loadWorkspaceSeq();
    }

    // Reset flag dan accumulator
    isWaitingForLoad = false;
    pendingLoadMode = -1;
    loadDelayAccumulator = 0.0f;
    currentState = UpdateState::STAGGERED_LOAD;  // Lanjut ke staggered load
  }
}

//--------------------------------------------------------------
void ofApp::updateStaggeredLoad() {
  switch (loadStage) {
    case LOAD_TEMPLATE:
      updateStaggeredTemplate();
      break;

    case LOAD_CUSTOMLINES:
      updateStaggeredCustomLines();
      break;

    case LOAD_POLYGONS:
      updateStaggeredPolygons();
      break;

    case LOAD_DONE:
      // Selesai, kembali ke normal
      isStaggeredLoad = false;
      isSequentialShapeLoad = false;
      currentState = UpdateState::NORMAL;
      break;
  }
}

//--------------------------------------------------------------
void ofApp::updateStaggeredTemplate() {
  if (!currentTemplate) {
    loadStage = LOAD_CUSTOMLINES;
    return;
  }

  if (isSequentialShapeLoad) {
    // SEQUENTIAL MODE: Update satu per satu
    const auto& shapes = currentTemplate->getShapes();
    if (currentTemplateIndex < shapes.size()) {
      shapes[currentTemplateIndex]->update();

      if (shapes[currentTemplateIndex]->isComplete()) {
        currentTemplateIndex++;
      }
    } else {
      loadStage = LOAD_CUSTOMLINES;
      currentTemplateIndex = 0;
    }
  } else {
    // PARALLEL MODE: Update semua bareng
    currentTemplate->update();

    // Cek apakah semua complete
    bool allComplete = true;
    const auto& shapes = currentTemplate->getShapes();
    for (auto& shape : shapes) {
      if (!shape->isComplete()) {
        allComplete = false;
        break;
      }
    }

    if (allComplete) {
      loadStage = LOAD_CUSTOMLINES;
      fileManager.setLoadParallelMode(true);
    }
  }
}

//--------------------------------------------------------------
void ofApp::updateStaggeredCustomLines() {
  // Sequential load: updateSequentialLoad akan menambah customLines bertahap
  size_t previousSize = customLines.size();
  fileManager.updateSequentialLoad(customLines, polygonShapes);

  // Sync ColorPicker saat customLines pertama kali muncul (sequential load)
  if (previousSize == 0 && !customLines.empty()) {
    syncColorPickerFromLoadedLines();
  }

  // Update progress semua customLines yang sudah ada
  float deltaTime = ofGetLastFrameTime();
  for (auto& line : customLines) {
    line.updateProgress(deltaTime);
  }

  // Cek apakah semua customLines sudah complete DAN semua sudah di-load
  bool allComplete = true;
  bool allLoaded = (fileManager.getCurrentLoadIndex() >= fileManager.getTotalLoadedLines());

  for (const auto& line : customLines) {
    if (line.getProgress() < 1.0f) {
      allComplete = false;
      break;
    }
  }

  // Pindah ke stage POLYGONS jika semua complete DAN semua sudah di-load
  if (allComplete && allLoaded) {
    loadStage = LOAD_POLYGONS;
  }
}

//--------------------------------------------------------------
void ofApp::updateStaggeredPolygons() {
  // Sequential load: updateSequentialLoad akan menambah polygons bertahap
  size_t previousSize = polygonShapes.size();
  fileManager.updateSequentialLoad(customLines, polygonShapes);

  // Sync ColorPicker saat polygons pertama kali muncul (sequential load)
  if (previousSize == 0 && !polygonShapes.empty()) {
    syncColorPickerFromLoadedPolygons();
  }

  // Update animation polygons yang BELUM complete saja
  float deltaTime = ofGetLastFrameTime();
  for (auto& polygon : polygonShapes) {
    if (!polygon.isAnimationComplete()) {
      polygon.update(deltaTime);
    }
  }

  // Cek apakah semua polygons sudah complete DAN sequential load sudah selesai
  bool allComplete = true;
  bool allLoaded = !fileManager.isLoadSequentialMode();

  for (const auto& polygon : polygonShapes) {
    if (!polygon.isAnimationComplete()) {
      allComplete = false;
      break;
    }
  }

  // Selesai staggered load jika semua complete DAN semua sudah di-load
  if (allComplete && allLoaded) {
    loadStage = LOAD_DONE;
    fileManager.cancelSequentialLoad();
  }
}

//--------------------------------------------------------------
// NORMAL UPDATE HELPERS
//--------------------------------------------------------------

void ofApp::updateTemplateShapes() {
  if (currentTemplate) {
    float deltaTime = ofGetLastFrameTime();
    currentTemplate->update(deltaTime);
  }
}

//--------------------------------------------------------------
void ofApp::updateScaling() {
  if (!currentTemplate || currentTemplate->radius == previousRadius) {
    return;
  }

  // Update radius di setiap shape individual
  const auto& shapes = currentTemplate->getShapes();
  for (auto& shape : shapes) {
    shape->setRadius(currentTemplate->radius);
  }

  // Scale customLines & polygons
  scaleCustomLinesAndPolygons(previousRadius, currentTemplate->radius);
  previousRadius = currentTemplate->radius;
}

//--------------------------------------------------------------
void ofApp::updateCustomLines() {
  fileManager.updateSequentialLoad(customLines, polygonShapes);
  float deltaTime = ofGetLastFrameTime();
  for (auto& line : customLines) {
    line.updateProgress(deltaTime);
  }
}

//--------------------------------------------------------------
void ofApp::updatePolygons() {
  float deltaTime = ofGetLastFrameTime();
  // Update polygons yang BELUM complete (bebas apa pun modenya)
  for (auto& polygon : polygonShapes) {
    if (!polygon.isAnimationComplete()) {
      polygon.update(deltaTime);
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  // Trail effect untuk geometry
  ofSetColor(255, 25);
  ofFill();
  ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

  // Push matrix untuk geometry drawing (centered)
  ofPushMatrix();

  // Apply canvas transform dalam urutan yang benar:
  // 1. Translate ke center screen
  ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

  // 2. Apply rotation (dari center)
  ofRotateDeg(canvasRotation);

  // 3. Apply zoom/scale (dari center)
  ofScale(canvasZoom, canvasZoom);

  // 4. Apply pan/translation (geser posisi canvas)
  ofTranslate(canvasTranslation.x, canvasTranslation.y);

  // Draw template - template handle drawing sendiri!
  if (currentTemplate) {
    currentTemplate->draw();
  }

  
  drawUserDots();

  // Draw custom lines dan UI elements
  drawCustomLinesAndUI();

  // Reset transform
  ofPopMatrix();

  //ImGUI (always render if context menu, popups, or visible)
  if (imguiVisible || contextMenu->isVisible() || successPopup->isVisible() || errorPopup->isVisible() || confirmationPopup->isVisible()) {
    drawImGui();
  }
}

//--------------------------------------------------------------
void ofApp::drawCustomLinesAndUI() {
  // Update selection state untuk semua lines
  for (int i = 0; i < customLines.size(); i++) {
    bool isSelected = selectionManager.isLineSelected(i);
    customLines[i].setSelected(isSelected);
    customLines[i].draw();
  }

  // Draw invisible polygons
  // TAPI jangan draw kalau sedang staggered load dan belum di stage POLYGONS
  bool shouldDrawPolygons = true;
  if (isStaggeredLoad && loadStage != LOAD_POLYGONS && loadStage != LOAD_DONE) {
    shouldDrawPolygons = false;  // Jangan draw polygons
  }

  if (shouldDrawPolygons) {
    for (int i = 0; i < polygonShapes.size(); i++) {
      polygonShapes[i].setSelected(selectionManager.isPolygonSelected(i));
      polygonShapes[i].draw();
    }
  }


  // Draw preview polyline (sedang drag)
  if (drawState == DRAGGING && currentPolylinePoints.size() > 1) {
    ofPushStyle();
    ofSetColor(colorManager->getCustomLineColor());
    ofSetLineWidth(mouseLineWidth); // Pakai mouseLineWidth untuk preview

    // Gambar preview polyline
    ofPolyline previewPolyline;
    for (auto &point : currentPolylinePoints) {
      previewPolyline.addVertex(point.x, point.y);
    }
    previewPolyline.draw();

    ofPopStyle();
  }

  // Draw dot highlights (hover state) - HANYA ketika TIDAK sedang drag DAN dots
  // visible
  if (drawState != DRAGGING && (!currentTemplate || currentTemplate->dotsVisible)) {
    vector<DotInfo> dots = getAllDots();
    for (auto &dot : dots) {
      if (isMouseOverDot(mousePos, dot.position)) {
        // Highlight dot
        ofPushStyle();
        ofSetColor(255, 0, 0, 200);            // Red highlight
        ofDrawCircle(dot.position, threshold); // Larger circle
        ofPopStyle();
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	inputManager->handleKeyPressed(key);
}

//--------------------------------------------------------------
// Interactive Line Creation Helpers
void ofApp::updateDotsCache() {
  cachedDots.clear();

  // Iterate semua template shapes dan collect dots secara polymorphic
  // Setiap shape bertanggung jawab untuk menambahkan dots-nya sendiri
  if (currentTemplate) {
    const auto& shapes = currentTemplate->getShapes();
    for (auto &shape : shapes) {
      if (shape) {
        shape->addDotsToCache(cachedDots);
      }
    }
  }

  // Tambahkan user-created dots ke cache untuk hover detection
  for (auto& dot : userDots) {
    if (dot) {
      dot->addDotsToCache(cachedDots);
    }
  }

  dotsCacheDirty = false;
}

//--------------------------------------------------------------
const vector<DotInfo> &ofApp::getAllDots() {
  // Lazy update cache kalau dirty
  if (dotsCacheDirty) {
    updateDotsCache();
  }

  return cachedDots; // Return reference, tidak copy
}

//--------------------------------------------------------------
bool ofApp::isMouseOverDot(vec2 mousePos, vec2 dotPos) {
	// Delegate to GeometryUtils
	return GeometryUtils::isMouseOverDot(mousePos, dotPos, threshold);
}

bool ofApp::lineExists(vec2 from, vec2 to) {
	// Delegate to GeometryUtils
	return GeometryUtils::lineExists(from, to, customLines);
}

//--------------------------------------------------------------
bool ofApp::isMouseOverLine(vec2 mousePos, vec2 lineStart, vec2 lineEnd,
                            float lineWidth) {
	// Delegate to GeometryUtils
	return GeometryUtils::isMouseOverLine(mousePos, lineStart, lineEnd, lineWidth);
}

//--------------------------------------------------------------
float ofApp::distanceToLine(vec2 point, vec2 lineStart, vec2 lineEnd,
                            float curve) {
	// Delegate to GeometryUtils
	return GeometryUtils::distanceToLine(point, lineStart, lineEnd, curve);
}

//--------------------------------------------------------------
int ofApp::getLineIndexAtPosition(vec2 pos) {
	// Delegate to GeometryUtils
	return GeometryUtils::getLineIndexAtPosition(pos, customLines, mouseLineWidth);
}

//--------------------------------------------------------------
void ofApp::pushUndoAction(UndoAction action) {
	// Saat action baru, clear redo stack (redo tidak valid lagi)
	clearRedoStack();

	// Add ke stack, jika sudah full, hapus yang paling lama
	if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
		undoStack.erase(undoStack.begin());  // Hapus yang paling lama (front)
	}
	undoStack.push_back(action);
}

//--------------------------------------------------------------
void ofApp::clearUndoStack() {
	undoStack.clear();
}

//--------------------------------------------------------------
void ofApp::clearRedoStack() {
	redoStack.clear();
}

//--------------------------------------------------------------
void ofApp::undo() {
	if (undoStack.empty()) {
		return;  // Tidak ada apa-apa untuk di-undo
	}

	// Ambil action terakhir
	UndoAction action = undoStack.back();
	undoStack.pop_back();

	// Siapkan redo action (perlu modifikasi untuk CHANGE actions)
	UndoAction redoAction = action;

	// Handle berdasarkan tipe action
	switch (action.type) {
		case CREATE_LINE:
		{
			// Undo create line = hapus line terakhir
			if (!customLines.empty()) {
				// SIMPAN line yang akan dihapus ke redoAction SEBELUM pop_back
				redoAction.deletedLine = customLines.back();
				redoAction.deletedLineIndex = static_cast<int>(customLines.size()) - 1;

				customLines.pop_back();

				// Clear selection jika index yang dipilih tidak valid lagi
				if (selectionManager.hasSelectedLine()) {
					// Cek apakah ada selected index yang out of bounds
					bool hasInvalidIndex = false;
					for (int index : selectionManager.getSelectedLineIndices()) {
						if (index >= customLines.size()) {
							hasInvalidIndex = true;
							break;
						}
					}
					if (hasInvalidIndex) {
						selectionManager.clearLineSelection();
					}
				}
			}
			// Push ke redo stack
			redoStack.push_back(redoAction);
			break;
		}

		case CREATE_POLYGON:
		{
			// Undo create polygon = hapus polygon terakhir
			if (!polygonShapes.empty()) {
				// SIMPAN polygon yang akan dihapus ke redoAction SEBELUM pop_back
				redoAction.deletedPolygon = polygonShapes.back();
				redoAction.deletedPolygonIndex = static_cast<int>(polygonShapes.size()) - 1;

				polygonShapes.pop_back();
				selectionManager.clearPolygonSelection();
			}
			// Push ke redo stack
			redoStack.push_back(redoAction);
			break;
		}

		case CREATE_DOT:
		{
			// Undo create dot = hapus dot terakhir
			if (!userDots.empty()) {
				// SIMPAN position dan radius dot yang akan dihapus ke redoAction SEBELUM pop_back
				redoAction.deletedDotPos = userDots.back()->getPosition();
				redoAction.deletedDotRadius = userDots.back()->getRadius();

				// Hapus dot terakhir
				userDots.pop_back();
			}
			// Push ke redo stack
			redoStack.push_back(redoAction);
			break;
		}

		case CHANGE_COLOR_LINE:
		{
			// Capture current colors dulu (sebelum restore) untuk redo
			std::vector<ofColor> currentColors;
			for (size_t i = 0; i < action.colorIndices.size(); i++) {
				int idx = static_cast<int>(action.colorIndices[i]);
				if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
					currentColors.push_back(customLines[idx].getColor());
				}
			}

			// Restore old colors
			for (size_t i = 0; i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
				int idx = static_cast<int>(action.colorIndices[i]);
				if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
					customLines[idx].setColor(action.oldColors[i]);
				}
			}

			// Setup redo action: redo akan apply new color (current colors)
			redoAction.oldColors = currentColors;
			redoStack.push_back(redoAction);
			break;
		}

		case CHANGE_COLOR_POLYGON:
		{
			// Capture current color dulu (sebelum restore) untuk redo
			ofColor currentColor;
			if (!action.colorIndices.empty() && action.colorIndices[0] >= 0 && action.colorIndices[0] < static_cast<int>(polygonShapes.size())) {
				currentColor = polygonShapes[action.colorIndices[0]].getColor();
			}

			// Restore old color
			if (!action.colorIndices.empty() && !action.oldColors.empty()) {
				int idx = action.colorIndices[0];
				if (idx >= 0 && idx < static_cast<int>(polygonShapes.size())) {
					polygonShapes[idx].setColor(action.oldColors[0]);
				}
			}

			// Setup redo action: redo akan apply new color (current color)
			if (!redoAction.oldColors.empty()) {
				redoAction.oldColors[0] = currentColor;
			}
			redoStack.push_back(redoAction);
			break;
		}

		case CHANGE_COLOR_DOT:
		{
			// Capture current colors dulu (sebelum restore) untuk redo
			std::vector<ofColor> currentColors;
			for (size_t i = 0; i < action.colorIndices.size(); i++) {
				int idx = static_cast<int>(action.colorIndices[i]);
				if (idx >= 0 && idx < static_cast<int>(userDots.size()) && userDots[idx]) {
					currentColors.push_back(userDots[idx]->getColor());
				}
			}

			// Restore old colors
			for (size_t i = 0; i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
				int idx = static_cast<int>(action.colorIndices[i]);
				if (idx >= 0 && idx < static_cast<int>(userDots.size()) && userDots[idx]) {
					userDots[idx]->setColor(action.oldColors[i]);
				}
			}

			// Setup redo action: redo akan apply new color (current colors)
			redoAction.oldColors = currentColors;
			redoStack.push_back(redoAction);
			break;
		}

		case DELETE_LINE:
			// Restore line yang dihapus
			if (action.deletedLineIndex >= 0 && action.deletedLineIndex <= static_cast<int>(customLines.size())) {
				customLines.insert(customLines.begin() + action.deletedLineIndex, action.deletedLine);
			}
			// Push ke redo stack
			redoStack.push_back(redoAction);
			break;

		case DELETE_POLYGON:
			// Restore polygon yang dihapus
			if (action.deletedPolygonIndex >= 0 && action.deletedPolygonIndex <= static_cast<int>(polygonShapes.size())) {
				polygonShapes.insert(polygonShapes.begin() + action.deletedPolygonIndex, action.deletedPolygon);
			}
			// Push ke redo stack
			redoStack.push_back(redoAction);
			break;

		case DELETE_DOT:
		{
			// Restore userDot yang dihapus
			if (action.deletedDotIndex >= 0 && action.deletedDotIndex <= static_cast<int>(userDots.size())) {
				auto dotShape = std::make_unique<DotShape>(action.deletedDotPos, "Dot", action.deletedDotRadius);
				dotShape->progress = 1.0f;
				dotShape->setLowerBound(action.deletedDotLowerBound);
				userDots.insert(userDots.begin() + action.deletedDotIndex, std::move(dotShape));
			}
			// Push ke redo stack
			redoStack.push_back(redoAction);
			break;
		}

		case CHANGE_CURVE:
		{
			// Capture current curves dulu (sebelum restore) untuk redo
			std::vector<float> currentCurves;
			for (size_t i = 0; i < action.curveLineIndices.size(); i++) {
				int idx = static_cast<int>(action.curveLineIndices[i]);
				if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
					currentCurves.push_back(customLines[idx].getCurve());
				}
			}

			// Restore old curves
			for (size_t i = 0; i < action.curveLineIndices.size() && i < action.oldCurves.size(); i++) {
				int idx = static_cast<int>(action.curveLineIndices[i]);
				if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
					customLines[idx].setCurve(action.oldCurves[i]);
				}
			}

			// Setup redo action: redo akan apply new curve (current curves)
			redoAction.oldCurves = currentCurves;
			redoStack.push_back(redoAction);
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::redo() {
	if (redoStack.empty()) {
		return;  // Tidak ada apa-apa untuk di-redo
	}

	// Ambil redo action terakhir
	UndoAction action = redoStack.back();
	redoStack.pop_back();

	// Handle berdasarkan tipe action
	switch (action.type) {
		case CREATE_LINE:
			// Redo create line = create line lagi
			customLines.push_back(action.deletedLine);
			// Push langsung ke undo stack (TANPA clearRedoStack)
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case CREATE_POLYGON:
			// Redo create polygon = create polygon lagi
			polygonShapes.push_back(action.deletedPolygon);
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case CREATE_DOT:
			// Redo create dot = buat dot baru dari position dan radius yang tersimpan
			{
				auto dotShape = std::make_unique<DotShape>(action.deletedDotPos, "Dot", action.deletedDotRadius);
				dotShape->progress = 1.0f;
				userDots.push_back(std::move(dotShape));
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case CHANGE_COLOR_LINE:
		{
			// Capture current colors dulu untuk membuat undo action baru
			UndoAction newUndoAction;
			newUndoAction.type = CHANGE_COLOR_LINE;
			newUndoAction.colorIndices = action.colorIndices;
			newUndoAction.newColor = action.newColor;

			// Apply redo colors (apply newColor) dan simpan current colors
			for (size_t i = 0; i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
				int idx = action.colorIndices[i];
				if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
					newUndoAction.oldColors.push_back(customLines[idx].getColor());
					customLines[idx].setColor(action.newColor);  // Apply NEW color (redo)
				}
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(newUndoAction);
			break;
		}

		case CHANGE_COLOR_POLYGON:
		{
			// Capture current color
			UndoAction newUndoActionPoly;
			newUndoActionPoly.type = CHANGE_COLOR_POLYGON;
			newUndoActionPoly.newColor = action.newColor;

			if (!action.colorIndices.empty()) {
				int idx = action.colorIndices[0];
				if (idx >= 0 && idx < static_cast<int>(polygonShapes.size())) {
					newUndoActionPoly.colorIndices = action.colorIndices;
					newUndoActionPoly.oldColors.push_back(polygonShapes[idx].getColor());
					polygonShapes[idx].setColor(action.newColor);  // Apply NEW color (redo)
				}
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(newUndoActionPoly);
			break;
		}

		case CHANGE_COLOR_DOT:
		{
			// Capture current colors dulu untuk membuat undo action baru
			UndoAction newUndoAction;
			newUndoAction.type = CHANGE_COLOR_DOT;
			newUndoAction.colorIndices = action.colorIndices;
			newUndoAction.newColor = action.newColor;

			// Apply redo colors (apply newColor) dan simpan current colors
			for (size_t i = 0; i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
				int idx = action.colorIndices[i];
				if (idx >= 0 && idx < static_cast<int>(userDots.size()) && userDots[idx]) {
					newUndoAction.oldColors.push_back(userDots[idx]->getColor());
					userDots[idx]->setColor(action.newColor);  // Apply NEW color (redo)
				}
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(newUndoAction);
			break;
		}

		case DELETE_LINE:
			// Redo = delete line lagi
			if (action.deletedLineIndex >= 0 && action.deletedLineIndex < static_cast<int>(customLines.size())) {
				customLines.erase(customLines.begin() + action.deletedLineIndex);
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case DELETE_POLYGON:
			// Redo = delete polygon lagi
			if (action.deletedPolygonIndex >= 0 && action.deletedPolygonIndex < static_cast<int>(polygonShapes.size())) {
				polygonShapes.erase(polygonShapes.begin() + action.deletedPolygonIndex);
			}
			selectionManager.clearPolygonSelection();
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case DELETE_DOT:
			// Redo = delete userDot lagi
			if (action.deletedDotIndex >= 0 && action.deletedDotIndex < static_cast<int>(userDots.size())) {
				userDots.erase(userDots.begin() + action.deletedDotIndex);
			}
			selectionManager.clearUserDotSelection();
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case CHANGE_CURVE:
		{
			// Capture current curves dan apply redo curves
			UndoAction newUndoActionCurve;
			newUndoActionCurve.type = CHANGE_CURVE;
			newUndoActionCurve.curveLineIndices = action.curveLineIndices;
			newUndoActionCurve.newCurve = action.newCurve;

			for (size_t i = 0; i < action.curveLineIndices.size() && i < action.oldCurves.size(); i++) {
				int idx = action.curveLineIndices[i];
				if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
					newUndoActionCurve.oldCurves.push_back(customLines[idx].getCurve());
					customLines[idx].setCurve(action.oldCurves[i]);
				}
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(newUndoActionCurve);
			break;
		}
	}
}


//--------------------------------------------------------------
void ofApp::createInvisiblePolygonFromSelected() {
  if (!selectionManager.hasSelectedLine())
    return; // Tidak ada yang selected

  // 1. Collect all selected lines dengan data endpoint-nya
  struct LineData {
    int index;
    vec2 start;  // Start point (points[0])
    vec2 end;    // End point (points[1])
    vector<vec2> sampledPoints;  // Full sampled points untuk curve
  };
  vector<LineData> lines;

  // Copy indices ke local vector untuk menghindari iterator invalidation
  std::vector<int> selectedLineIndices(selectionManager.getSelectedLineIndices().begin(),
                                       selectionManager.getSelectedLineIndices().end());
  for (int lineIndex : selectedLineIndices) {
    if (lineIndex >= 0 && lineIndex < customLines.size()) {
      const CustomLine& line = customLines[lineIndex];
      const vector<vec2>& points = line.getPoints();

      if (points.size() >= 2) {
        LineData lineData;
        lineData.index = lineIndex;
        lineData.start = points[0];
        lineData.end = points[1];
        lineData.sampledPoints = line.getSampledPoints();
        lines.push_back(lineData);
      }
    }
  }

  if (lines.size() < 1) {
    return;  // Tidak ada valid lines
  }

  // 2. Susun lines supaya connected secara berurutan
  // Dimulai dari line pertama sebagai anchor
  vector<LineData> orderedLines;
  orderedLines.push_back(lines[0]);
  lines.erase(lines.begin());

  // Loop sampa semua lines ter-order
  while (!lines.empty()) {
    vec2 currentEnd = orderedLines.back().end;
    bool found = false;

    // Cari line yang start-nya cocok dengan currentEnd
    for (size_t i = 0; i < lines.size(); i++) {
      if (glm::length(lines[i].start - currentEnd) < 1.0f) {
        // Found! Start point cocok dengan end point sebelumnya
        orderedLines.push_back(lines[i]);
        lines.erase(lines.begin() + i);
        found = true;
        break;
      } else if (glm::length(lines[i].end - currentEnd) < 1.0f) {
        // Found! End point cocok (butuh reverse)
        LineData reversed = lines[i];
        std::reverse(reversed.sampledPoints.begin(), reversed.sampledPoints.end());
        std::swap(reversed.start, reversed.end);
        orderedLines.push_back(reversed);
        lines.erase(lines.begin() + i);
        found = true;
        break;
      }
    }

    // Kalau tidak ketemu yang cocok, coba cari dari awal (untuk handle kasus khusus)
    if (!found) {
      // Coba connect dari start point line pertama
      vec2 currentStart = orderedLines.front().start;
      for (size_t i = 0; i < lines.size(); i++) {
        if (glm::length(lines[i].end - currentStart) < 1.0f) {
          // Found! End point cocok dengan start point pertama (insert di depan)
          LineData reversed = lines[i];
          std::reverse(reversed.sampledPoints.begin(), reversed.sampledPoints.end());
          std::swap(reversed.start, reversed.end);
          orderedLines.insert(orderedLines.begin(), reversed);
          lines.erase(lines.begin() + i);
          found = true;
          break;
        } else if (glm::length(lines[i].start - currentStart) < 1.0f) {
          // Found! Start point cocok dengan start point pertama (insert di depan, no reverse)
          orderedLines.insert(orderedLines.begin(), lines[i]);
          lines.erase(lines.begin() + i);
          found = true;
          break;
        }
      }
    }

    // Kalau masih tidak ketemu, break untuk avoid infinite loop
    if (!found) {
      ofLog(OF_LOG_WARNING) << "createInvisiblePolygon - Cannot connect all lines, path is broken";
      break;
    }
  }

  // 3. Extract semua titik dari ordered lines
  vector<vec2> allPoints;
  for (const LineData& lineData : orderedLines) {
    for (const vec2& p : lineData.sampledPoints) {
      allPoints.push_back(p);
    }
  }

  if (allPoints.size() < 3) {
    // Kurang dari 3 titik, tidak bisa buat polygon
    return;
  }

  int polygonIndex =
      static_cast<int>(polygonShapes.size());
  PolygonShape newPolygon(allPoints, colorManager->getPolygonColor(),
                          polygonIndex);
  polygonShapes.push_back(newPolygon);

  // Push undo action
  UndoAction undoAction;
  undoAction.type = CREATE_POLYGON;
  undoAction.isCreate = true;
  pushUndoAction(undoAction);

  // 4. Clear selection
  selectionManager.clearLineSelection();
}

//--------------------------------------------------------------
// File operations sekarang ditangani oleh FileManager class

//--------------------------------------------------------------
// Mouse Event Handlers
void ofApp::mousePressed(int x, int y, int button) {
	inputManager->handleMousePressed(x, y, button);
}

void ofApp::mouseDragged(int x, int y, int button) {
	inputManager->handleMouseDragged(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button) {
	inputManager->handleMouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	inputManager->handleMouseScrolled(x, y, scrollX, scrollY);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	inputManager->handleMouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	inputManager->handleKeyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}
void ofApp::mouseExited(int x, int y) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::gotMessage(ofMessage msg) {}
void ofApp::dragEvent(ofDragInfo dragInfo) {}

//--------------------------------------------------------------
void ofApp::startSequentialDrawing() {
  if (!currentTemplate) return;

  // Delegate ke template
  currentTemplate->startSequentialDrawing();

  // NOTE: JANGAN ubah currentState di sini!
  // sequentialMode adalah flag TEMPLATE level, bukan application state
  // Sequential drawing di-handle langsung di updateNormal() line 97-102
  // Jadi biarkan currentState tetap NORMAL
}

//--------------------------------------------------------------
// NOTE: updateSequentialDrawing() sudah dihapus!
// Logic sekarang ada di SacredGeometryTemplate::updateSequentialDrawing()
// Template handle sequential drawing secara autonomous

//--------------------------------------------------------------
void ofApp::toggleLabels() {
  if (!currentTemplate) return;

  // NOTE: ImGui Checkbox SUDAH mengubah nilai labelsVisible
  // Jadi JANGAN toggle lagi, langsung apply saja
  currentTemplate->toggleLabels();
}

//--------------------------------------------------------------
void ofApp::toggleDots() {
  if (!currentTemplate) return;

  // NOTE: ImGui Checkbox SUDAH mengubah nilai dotsVisible
  // Jadi JANGAN toggle lagi, langsung apply saja
  currentTemplate->toggleDots();
}

//--------------------------------------------------------------
// (hideAllShapes, showAllShapes, dan setCartesianAxesVisibility sudah dihapus - tidak diperlukan lagi dengan Draw Only concept)
//--------------------------------------------------------------
void ofApp::updateLineWidth() {
    if (!currentTemplate) return;

    // Delegate ke template - pakai nilai yang sudah ada di template
    currentTemplate->updateLineWidth(currentTemplate->lineWidth);
}

//--------------------------------------------------------------
void ofApp::updateCustomLineColor(ofColor color) {
	// Delegate ke ColorManager
	colorManager->updateCustomLineColor(color);
}

//--------------------------------------------------------------
void ofApp::resetAllCustomLineColor() {
	// Delegate ke ColorManager
	colorManager->resetAllCustomLineColor();
}

//--------------------------------------------------------------
void ofApp::resetSelectedCustomLineColor() {
	// Delegate ke ColorManager
	colorManager->resetSelectedCustomLineColor();
}

//--------------------------------------------------------------
void ofApp::updatePolygonColor(ofColor color) {
	// Delegate ke ColorManager
	colorManager->updatePolygonColor(color);
}

//--------------------------------------------------------------
void ofApp::resetAllPolygonColor() {
	// Delegate ke ColorManager
	colorManager->resetAllPolygonColor();
}

//--------------------------------------------------------------
void ofApp::resetSelectedPolygonColor() {
	// Delegate ke ColorManager
	colorManager->resetSelectedPolygonColor();
}

//--------------------------------------------------------------
void ofApp::updatePolygonAlpha(uint8_t alpha) {
	// Delegate ke ColorManager
	colorManager->updatePolygonAlpha(alpha);
}

//--------------------------------------------------------------
void ofApp::updateUserDotRadius(float radius) {
	// Update variabel global
	userDotRadius = radius;

	// Jika ada userDot yang terseleksi, update hanya yang terseleksi
	if (selectionManager.hasSelectedUserDot()) {
		const std::set<int>& indices = selectionManager.getSelectedUserDotIndices();
		for (int index : indices) {
			if (index >= 0 && index < userDots.size()) {
				if (userDots[index]) {
					userDots[index]->setRadius(radius);
				}
			}
		}
	} else {
		// Jika tidak ada yang terseleksi, update semua userDots
		for (auto& dot : userDots) {
			if (dot) {
				dot->setRadius(radius);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::updateUserDotColor(ofColor color) {
	// Delegate ke ColorManager
	colorManager->updateUserDotColor(color);
}

//--------------------------------------------------------------
void ofApp::copyDotColor() {
	// Delegate ke ColorManager
	colorManager->copyDotColor();
}

//--------------------------------------------------------------
void ofApp::copyPolygonColor() {
	// Delegate ke ColorManager
	colorManager->copyPolygonColor();
}

//--------------------------------------------------------------
void ofApp::copyLineColor() {
	// Delegate ke ColorManager
	colorManager->copyLineColor();
}

//--------------------------------------------------------------
void ofApp::pasteColorToDot() {
	// Delegate ke ColorManager
	colorManager->pasteColorToDot();
}

//--------------------------------------------------------------
void ofApp::pasteColorToPolygon() {
	// Delegate ke ColorManager
	colorManager->pasteColorToPolygon();
}

//--------------------------------------------------------------
void ofApp::pasteColorToLine() {
	// Delegate ke ColorManager
	colorManager->pasteColorToLine();
}

//--------------------------------------------------------------
void ofApp::syncColorPickerFromLoadedLines() {
	// Ambil warna dari customLine pertama yang diload (jika ada)
	if (!customLines.empty()) {
		ofColor loadedColor = customLines[0].getColor();

		// Update global color (ColorManager will sync internally when needed)
		// colorManager->customLineColor = loadedColor;

		// Update UserCustom color picker UI
		for (auto& gui : guiComponents) {
			// Cari UserCustom component
			UserCustom* userCustom = dynamic_cast<UserCustom*>(gui.get());
			if (userCustom) {
				userCustom->updateColorFromApp();
				break; // Found, no need to continue
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::syncColorPickerFromLoadedPolygons() {
	// Ambil warna dari polygon pertama yang diload (jika ada)
	if (!polygonShapes.empty()) {
		ofColor loadedColor = polygonShapes[0].getColor();

		// Update global color (ColorManager will sync internally when needed)
		// colorManager->polygonColor = loadedColor;

		// Update UserCustom color picker UI
		for (auto& gui : guiComponents) {
			// Cari UserCustom component
			UserCustom* userCustom = dynamic_cast<UserCustom*>(gui.get());
			if (userCustom) {
				userCustom->updatePolygonColorFromApp();
				break; // Found, no need to continue
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::syncUserDotFromLoaded() {
	// Ambil radius dan warna dari userDot pertama yang diload (jika ada)
	if (!userDots.empty() && userDots[0]) {
		float loadedRadius = userDots[0]->getRadius();
		ofColor loadedColor = userDots[0]->getColor();

		// Update global radius dan color
		userDotRadius = loadedRadius;
		// colorManager->userDotColor = loadedColor;  // ColorManager will sync internally

		// Update UserCustom color picker UI
		for (auto& gui : guiComponents) {
			// Cari UserCustom component
			UserCustom* userCustom = dynamic_cast<UserCustom*>(gui.get());
			if (userCustom) {
				userCustom->updateUserDotColorFromApp();
				break; // Found, no need to continue
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::syncColorFromSelectedObjects() {
	// Delegate ke ColorManager
	colorManager->syncColorPickersFromSelection();

	// Update SEMUA color picker UI
	for (auto& gui : guiComponents) {
		UserCustom* userCustom = dynamic_cast<UserCustom*>(gui.get());
		if (userCustom) {
			userCustom->updateUserDotColorFromApp();  // Update userDot color picker
			userCustom->updateColorFromApp();         // Update customLine color picker
			userCustom->updatePolygonColorFromApp();  // Update polygon color picker
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::clearCustomLinesAndPolygons() {
	// Jangan hapus jika sedang sequential load
	if (fileManager.isLoadSequentialMode()) {
		return; // Aborted, sedang loading
	}

	// Jangan hapus jika sedang parallel load dan masih ada polygon animasi
	if (fileManager.isLoadParallelMode()) {
		// Cek apakah masih ada polygon yang sedang animasi
		for (const auto &polygon : polygonShapes) {
			if (polygon.hasAnimation()) {
				return; // Masih ada polygon yang animasi, tunggu kelar
			}
		}
	}

	// Hapus semua polygons - delegate ke deleteAllPolygons()
	deleteAllPolygons();

	// Hapus semua custom lines - delegate ke deleteAllCustomLines()
	deleteAllCustomLines();
}

//--------------------------------------------------------------
void ofApp::deleteAllCustomLines() {
	// Jangan hapus jika sedang sequential load
	if (fileManager.isLoadSequentialMode()) {
		return;
	}

	// Hapus semua custom lines
	if (!fileManager.isLoadParallelMode()) {
		FileManager::clearCustomLines(customLines);
	}
	selectionManager.clearLineSelection();
}

//--------------------------------------------------------------
void ofApp::deleteAllPolygons() {
	// Jangan hapus jika sedang sequential load
	if (fileManager.isLoadSequentialMode()) {
		return;
	}

	// Hapus semua polygons
	if (!polygonShapes.empty()) {
		polygonShapes.clear();
		selectionManager.clearPolygonSelection();
	}
}

//--------------------------------------------------------------
void ofApp::deleteAllUserDots() {
	// Hapus semua userDots (duplicat dots)
	if (!userDots.empty()) {
		userDots.clear();
		selectionManager.clearUserDotSelection();
	}
}

//--------------------------------------------------------------
void ofApp::deleteSelectedUserDot() {
	// Cek apakah ada userDot yang terseleksi
	if (!selectionManager.hasSelectedUserDot()) {
		return;
	}

	// Hapus SEMUA userDots yang terselect (support multi-delete)
	// Sort descending agar aman untuk erase
	const std::set<int>& indices = selectionManager.getSelectedUserDotIndices();
	vector<int> toDelete(indices.begin(), indices.end());
	std::sort(toDelete.rbegin(), toDelete.rend()); // Descending

	// Push undo action untuk setiap dot yang dihapus (reverse order)
	for (int index : toDelete) {
		if (index >= 0 && index < userDots.size()) {
			UndoAction undoAction;
			undoAction.type = DELETE_DOT;
			undoAction.deletedDotPos = userDots[index]->getPosition();
			undoAction.deletedDotLowerBound = userDots[index]->getLowerBound();
			undoAction.deletedDotRadius = userDots[index]->getRadius();
			undoAction.deletedDotIndex = index;
			pushUndoAction(undoAction);
		}
	}

	// Hapus dots
	for (int index : toDelete) {
		if (index >= 0 && index < userDots.size()) {
			userDots.erase(userDots.begin() + index);
		}
	}
	selectionManager.clearUserDotSelection();
}

//--------------------------------------------------------------
void ofApp::deleteSelectedPolygons() {
	// Cek apakah ada polygon yang terseleksi
	if (!selectionManager.hasSelectedPolygon()) {
		return;
	}

	// Hapus SEMUA polygons yang terselect (support multi-delete)
	// Sort descending agar aman untuk erase
	vector<int> toDelete(selectionManager.getSelectedPolygonIndices().begin(),
						 selectionManager.getSelectedPolygonIndices().end());
	std::sort(toDelete.rbegin(), toDelete.rend()); // Descending

	// Push undo action untuk setiap polygon yang dihapus (reverse order)
	for (int index : toDelete) {
		if (index >= 0 && index < polygonShapes.size()) {
			UndoAction undoAction;
			undoAction.type = DELETE_POLYGON;
			undoAction.deletedPolygon = polygonShapes[index];
			undoAction.deletedPolygonIndex = index;
			pushUndoAction(undoAction);
		}
	}

	// Hapus polygons
	for (int index : toDelete) {
		if (index >= 0 && index < polygonShapes.size()) {
			polygonShapes.erase(polygonShapes.begin() + index);
		}
	}
	selectionManager.clearPolygonSelection();
}

//--------------------------------------------------------------
void ofApp::cleanCanvas() {
	// Tampilkan confirmation popup dulu
	// Callback akan memanggil cleanCanvasInternal() saat user klik Yes
	confirmationPopup->show(
		"Clean Canvas",
		"Are you sure you want to clean the canvas?\n\nEverything on the canvas will be deleted.",
		"Yes, Clean",
		"Cancel",
		[this]() {
			// User klik Yes, execute clean
			cleanCanvasInternal();
		}
	);
}

//--------------------------------------------------------------
void ofApp::cleanCanvasInternal() {
    // Skip kalau sedang load sequential
    if (fileManager.isLoadSequentialMode()) {
        return;
    }

    // Hapus semua polygons dan custom lines
    clearCustomLinesAndPolygons();

    // Hapus semua userDots
    userDots.clear();
    selectionManager.clearUserDotSelection();

    // Benar-benar HAPUS semua template shapes - delegate ke template
    if (currentTemplate) {
        currentTemplate->clearAllShapes();
    }
}

//--------------------------------------------------------------
void ofApp::resetTransform() {
	// Reset canvas transform ke default values
	canvasTranslation = vec2(0, 0);
	canvasRotation = 0.0f;
	canvasZoom = 1.0f;
}

//--------------------------------------------------------------
void ofApp::toggleSacredGeometryWindow() {
    if (!imguiVisible || !showSacredGeometry) {
        // Show SacredGeometry window
        imguiVisible = true;
        showSacredGeometry = true;
        // Jangan hide Playground, biarkan user punya banyak windows terbuka

        // Set windowOpen flag di SacredGeometry
        for (auto& gui : guiComponents) {
            SacredGeometry* sacredGeo = dynamic_cast<SacredGeometry*>(gui.get());
            if (sacredGeo) {
                sacredGeo->showWindow();
                break;
            }
        }
    } else {
        // SacredGeometry already visible, focus it
        for (auto& gui : guiComponents) {
            SacredGeometry* sacredGeo = dynamic_cast<SacredGeometry*>(gui.get());
            if (sacredGeo) {
                sacredGeo->focusWindow();
                break;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::togglePlaygroundWindow() {
    if (!imguiVisible || !showPlayground) {
        // Show Playground window
        imguiVisible = true;
        showPlayground = true;
        // Jangan hide SacredGeometry, biarkan user punya banyak windows terbuka

        // Set windowOpen flag di Playground
        for (auto& gui : guiComponents) {
            Playground* playground = dynamic_cast<Playground*>(gui.get());
            if (playground) {
                playground->showWindow();
                break;
            }
        }
    } else {
        // Playground already visible, focus it
        for (auto& gui : guiComponents) {
            Playground* playground = dynamic_cast<Playground*>(gui.get());
            if (playground) {
                playground->focusWindow();
                break;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::toggleUserCustomWindow() {
    if (!imguiVisible || !showUserCustom) {
        // Show UserCustom window
        imguiVisible = true;
        showUserCustom = true;
        // Jangan hide SacredGeometry dan Playground, biarkan user punya banyak windows terbuka

        // Set windowOpen flag di UserCustom
        for (auto& gui : guiComponents) {
            UserCustom* userCustom = dynamic_cast<UserCustom*>(gui.get());
            if (userCustom) {
                userCustom->showWindow();
                break;
            }
        }
    } else {
        // UserCustom already visible, focus it
        for (auto& gui : guiComponents) {
            UserCustom* userCustom = dynamic_cast<UserCustom*>(gui.get());
            if (userCustom) {
                userCustom->focusWindow();
                break;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::toggleSelectionInfoWindow() {
	if (!imguiVisible || !showSelectionInfo) {
		// Show SelectionInfo window
		imguiVisible = true;
		showSelectionInfo = true;
		selectionInfo->showWindow();
	} else {
		// SelectionInfo already visible, focus it
		selectionInfo->focusWindow();
	}
}

//--------------------------------------------------------------
void ofApp::duplicateDotAbove() {
	duplicateManager->duplicateDotAbove();
}

//--------------------------------------------------------------
void ofApp::duplicateDotBelow() {
	duplicateManager->duplicateDotBelow();
}

//--------------------------------------------------------------
void ofApp::duplicateDotLeft() {
	duplicateManager->duplicateDotLeft();
}

//--------------------------------------------------------------
void ofApp::duplicateDotRight() {
	duplicateManager->duplicateDotRight();
}

//--------------------------------------------------------------
void ofApp::duplicateLineR180() {
	duplicateManager->duplicateLineR180();
}

//--------------------------------------------------------------
void ofApp::drawUserDots() {

    // Hanya draw userDot jika showUserDot == true
    if (showUserDot) {
        for (auto& dot : userDots) {
            if (dot) {
                dot->draw();
            }
        }
    }

    // Label sudah dihapus, diganti dengan ImGui tooltip di ObjectTooltip
}

//--------------------------------------------------------------
void ofApp::scaleCustomLinesAndPolygons(float oldRadius, float newRadius) {
	// Hitung rasio scaling
	float scaleRatio = newRadius / oldRadius;

	// Hindari scaling jika ratio ~1 (untuk performance dan presisi)
	if (std::abs(scaleRatio - 1.0f) < 0.0001f) {
		return;
	}

	// Scale semua customLines
	for (auto& line : customLines) {
		vector<vec2> points = line.getPoints();
		for (auto& point : points) {
			point = point * scaleRatio;  // Scale setiap titik
		}
		line.setPoints(points);

		// Scale juga curve parameter agar kelengkungan proporsional
		float oldCurve = line.getCurve();
		float newCurve = oldCurve * scaleRatio;
		line.setCurve(newCurve);
	}

	// Scale semua polygons
	for (auto& polygon : polygonShapes) {
		vector<vec2> vertices = polygon.getVertices();
		for (auto& vertex : vertices) {
			vertex = vertex * scaleRatio;  // Scale setiap vertex
		}
		polygon.setVertices(vertices);
	}

	// Scale semua userDots (duplikat dots)
	for (auto& dot : userDots) {
		if (dot) {
			// Scale posisi userDot
			vec2 currentPos = dot->getPosition();
			vec2 newPos = currentPos * scaleRatio;
			dot->setPosition(newPos);

			// Scale juga lowerBound (posisi parent dot)
			vec2 currentLowerBound = dot->getLowerBound();
			vec2 newLowerBound = currentLowerBound * scaleRatio;
			dot->setLowerBound(newLowerBound);
		}
	}
}

//--------------------------------------------------------------
bool ofApp::isCanvasEmpty() {
	// Cek apakah ada template shapes
	if (currentTemplate && !currentTemplate->getShapes().empty()) {
		return false;  // Ada template shapes, canvas tidak kosong
	}

	// Cek customLines
	if (!customLines.empty()) {
		return false;  // Ada customLines, canvas tidak kosong
	}

	// Cek polygons
	if (!polygonShapes.empty()) {
		return false;  // Ada polygons, canvas tidak kosong
	}

	return true;  // Canvas benar-bener kosong
}

//--------------------------------------------------------------
vec2 ofApp::screenToWorld(vec2 screenPos) {
	// Delegate to GeometryUtils
	return GeometryUtils::screenToWorld(screenPos, canvasTranslation, canvasZoom, canvasRotation);
}

//--------------------------------------------------------------
void ofApp::exit() {
  // Cleanup jika ada

    //ImGUI
    exitImGui();
}

void ofApp::setupImGui() {
    IMGUI_CHECKVERSION(); //cek versi imgui.h and imgui.cpp harus sama
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init();
    HWND hwnd = ofGetWin32Window();
    ImGui_ImplWin32_Init(hwnd);

    // Initialize GUI Components
    guiComponents.push_back(std::make_unique<MenuBar>(this));
    guiComponents.push_back(std::make_unique<SacredGeometry>(this));
    guiComponents.push_back(std::make_unique<Playground>(this));
    guiComponents.push_back(std::make_unique<UserCustom>(this));

    // Initialize Context Menu (bukan bagian dari guiComponents karena draw-nya khusus)
    contextMenu = std::make_unique<ContextMenu>(this);

    // Initialize popup (not in guiComponents, drawn separately)
    successPopup = std::make_unique<SuccessPopup>(this);
    errorPopup = std::make_unique<ErrorPopup>(this);
    confirmationPopup = std::make_unique<ConfirmationPopup>(this);

    // Initialize Selection Info window
    selectionInfo = std::make_unique<SelectionInfo>(this);

    // Initialize Object Tooltip manager
    objectTooltip = std::make_unique<ObjectTooltip>(this);
}


//--------------------------------------------------------------
void ofApp::drawImGui() {
    // Skip ImGui render jika window minimized (DisplaySize = 0,0)
    int width = ofGetWidth();
    int height = ofGetHeight();

    if (width <= 0 || height <= 0) {
        return;  // Skip ImGui render jika window minimized
    }

    // Update display size BEFORE NewFrame! (WAJIB!)
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width, height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Draw GUI components
    // MenuBar (component 0) selalu draw jika imguiVisible
    if (imguiVisible && !guiComponents.empty()) {
        guiComponents[0]->draw();  // MenuBar
    }

    // SacredGeometry (component 1)
    if (imguiVisible && showSacredGeometry && guiComponents.size() > 1) {
        guiComponents[1]->draw();  // SacredGeometry
    }

    // Playground (component 2)
    if (imguiVisible && showPlayground && guiComponents.size() > 2) {
        guiComponents[2]->draw();  // Playground
    }

    // UserCustom (component 3)
    if (imguiVisible && showUserCustom && guiComponents.size() > 3) {
        guiComponents[3]->draw();  // UserCustom
    }

    // Draw popup dialogs
    successPopup->draw();
    errorPopup->draw();
    confirmationPopup->draw();

    // Draw Selection Info window
    if (imguiVisible && showSelectionInfo) {
        selectionInfo->draw();
    }

    // Draw tooltips (SELALU render, tidak tergantung imguiVisible)
    objectTooltip->draw();

    // Draw context menu (SELALU render terlepas dari imguiVisible)
    contextMenu->draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ofApp::exitImGui() {
    //Shutdown OpenGL3 Backend
    ImGui_ImplOpenGL3_Shutdown();

    //Shutdown Win32 Backend
    ImGui_ImplWin32_Shutdown();

    //Destroy ImGui Context
    ImGui::DestroyContext();
}

