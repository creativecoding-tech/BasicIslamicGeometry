#include "ofApp.h"
#include "shape/AbstractShape.h"
#include "template/templates/BasicZelligeTemplate.h"
#include "operation/gui/SacredGeometry.h"


//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(false);
  ofSetFrameRate(60);
  ofSetEscapeQuitsApp(false);
  ofSetBackgroundAuto(false);
  ofEnableAntiAliasing();
  ofEnableSmoothing();

  // Load font untuk custom line labels
  fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);

  // Setup template system (REGISTER SEMUA TEMPLATES)
  setupTemplateSystem();

  // Load default template (Basic Zellige)
  switchTemplate("Basic Zellige");

  // Initial dots cache build
  dotsCacheDirty = true;

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

  // PENTING: Selalu setup shapes saat switch template!
  // Ini diperlukan untuk pertama kali atau saat template benar-benar berubah
  currentTemplate->setupShapes();  // Setup/populate shapes vector

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


//--------------------------------------------------------------
// UPDATE STRATEGIES - Strategy Pattern Implementation
//--------------------------------------------------------------

void ofApp::updateNormal() {
  // Template sequential drawing
  if (currentTemplate && currentTemplate->sequentialMode) {
    bool complete = currentTemplate->updateSequentialDrawing();
    if (complete) {
      dotsCacheDirty = true;
    }
  }

  // Update template shapes
  updateTemplateShapes();

  // Rebuild dots cache
  dotsCacheDirty = true;

  // Handle scaling
  updateScaling();

  // Update custom lines & polygons
  updateCustomLines();
  updatePolygons();
}

//--------------------------------------------------------------
void ofApp::updateSequentialDrawing() {
  // Template fully autonomous - template handles sequential drawing
  if (currentTemplate && currentTemplate->sequentialMode) {
    bool complete = currentTemplate->updateSequentialDrawing();
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

  float elapsed = ofGetElapsedTimef() - loadDelayTimer;
  if (elapsed >= 0.5f) {
    // Timer selesai, panggil load method
    if (pendingLoadMode == 0) {
      loadWorkspace();
    } else if (pendingLoadMode == 1) {
      loadWorkspaceSeq();
    }

    // Reset flag
    isWaitingForLoad = false;
    pendingLoadMode = -1;
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
  for (auto& line : customLines) {
    line.updateProgress();
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

  // Update animation semua polygons yang sudah ada
  for (auto& polygon : polygonShapes) {
    polygon.update();
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
    currentTemplate->update();
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
  for (auto& line : customLines) {
    line.updateProgress();
  }
}

//--------------------------------------------------------------
void ofApp::updatePolygons() {
  if (fileManager.isLoadParallelMode()) {
    for (auto& polygon : polygonShapes) {
      polygon.update();
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
  ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

  // Draw template - template handle drawing sendiri!
  if (currentTemplate) {
    currentTemplate->draw();
  }

  // Draw custom lines dan UI elements
  drawCustomLinesAndUI();

  // Reset transform
  ofPopMatrix();

  //ImGUI (only if visible)
  if (imguiVisible) {
    drawImGui();
  }
}

//--------------------------------------------------------------
void ofApp::drawCustomLinesAndUI() {
  // Update selection state untuk semua lines
  for (int i = 0; i < customLines.size(); i++) {
    bool isSelected = (selectedLineIndices.count(i) > 0);
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
      polygonShapes[i].setSelected(i == selectedPolygonIndex);
      polygonShapes[i].draw();
    }
  }

  // Draw curve value label untuk garis yang selected
  if (!selectedLineIndices.empty()) {
    // Tampilkan label untuk SEMUA garis yang selected
    for (int lineIndex : selectedLineIndices) {
      if (lineIndex >= 0 && lineIndex < customLines.size()) {
        const CustomLine &line = customLines[lineIndex];
        if (line.getPoints().size() >= 2) {
          vec2 midPoint = (line.getPoints()[0] + line.getPoints()[1]) / 2.0f;

          ofPushStyle();
          ofSetColor(0, 0, 0); // Hitam untuk label
          // Label "CustomLine" + index (di atas)
          fontNormal.drawString("CustomLine" + ofToString(lineIndex),
                                midPoint.x + 10, midPoint.y - 25);
          // Label "Curve: ..." (di bawah)
          fontNormal.drawString("Curve: " + ofToString(line.getCurve(), 1),
                                midPoint.x + 10, midPoint.y - 10);
          ofPopStyle();
        }
      }
    }
  }

  // Draw preview polyline (sedang drag)
  if (drawState == DRAGGING && currentPolylinePoints.size() > 1) {
    ofPushStyle();
    ofSetColor(customLineColor);
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
  if (key == OF_KEY_END)
    ofExit();

  // Sequential drawing dengan SHIFT+1
  if (key == '!' && ofGetKeyPressed(OF_KEY_SHIFT)) {
    startSequentialDrawing();
  }

  // Toggle label visibility dengan ` atau ~
  if (key == '`' || key == '~') {
    // Toggle label visibility - delegate ke template
    if (currentTemplate) {
      currentTemplate->labelsVisible = !currentTemplate->labelsVisible;
      currentTemplate->toggleLabels();
    }
  }

  // Toggle dot visibility dengan . atau >
  if (key == '.' || key == '>') {
    // Toggle dot visibility - delegate ke template
    if (currentTemplate) {
      currentTemplate->dotsVisible = !currentTemplate->dotsVisible;
      currentTemplate->toggleDots();
    }
  }

  if (key == OF_KEY_DEL) {
    bool isShiftPressed = ofGetKeyPressed(OF_KEY_SHIFT);

    if (isCtrlPressed && isShiftPressed) {
      // CTRL+SHIFT+DEL: Clean canvas (hapus semua polygon, custom lines, dan hide template shapes)
      cleanCanvas();
      return; // Jangan lanjut ke logic DEL biasa
    } else if (isCtrlPressed) {
      // CTRL+DEL: Hapus semua polygon dan semua custom lines
      clearCustomLinesAndPolygons();
      return; // Jangan lanjut ke hideAllShapes()
    } else {
      // DEL saja: Hide semua shapes (hanya jika TIDAK staggered load)
      if (!isStaggeredLoad) {
        hideAllShapes();
      }
    }
  }

  if (key == OF_KEY_BACKSPACE) {
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

    // Prioritas: Hapus polygon selected dulu
    if (selectedPolygonIndex != -1) {
      // Push undo action SEBELUM hapus
      UndoAction undoAction;
      undoAction.type = DELETE_POLYGON;
      undoAction.deletedPolygon = polygonShapes[selectedPolygonIndex];
      undoAction.deletedPolygonIndex = selectedPolygonIndex;
      pushUndoAction(undoAction);

      // Hapus polygon yang selected
      polygonShapes.erase(polygonShapes.begin() + selectedPolygonIndex);
      selectedPolygonIndex = -1;
    }
    // Kalau tidak ada polygon selected, cek customLine
    else if (!selectedLineIndices.empty()) {
      // Hapus SEMUA garis yang terselect (support multi-delete)
      // Sort descending agar aman untuk erase
      vector<int> toDelete(selectedLineIndices.begin(),
                           selectedLineIndices.end());
      std::sort(toDelete.rbegin(), toDelete.rend()); // Descending

      // Push undo action untuk setiap line yang dihapus (reverse order)
      for (int index : toDelete) {
        UndoAction undoAction;
        undoAction.type = DELETE_LINE;
        undoAction.deletedLine = customLines[index];
        undoAction.deletedLineIndex = index;
        pushUndoAction(undoAction);
      }

      // Hapus lines
      for (int index : toDelete) {
        customLines.erase(customLines.begin() + index);
      }
      selectedLineIndices.clear();
    }
    // Kalau tidak ada polygon dan customLine terselect, toggle CartesianAxes
    else {
      if (currentTemplate) {
        const auto& shapes = currentTemplate->getShapes();
        if (!shapes.empty()) {
          AbstractShape* cartesianAxes = shapes[0].get();
          setCartesianAxesVisibility(!cartesianAxes->showing);  // Toggle
        }
      }
    }
  }

  if (key == ')' && ofGetKeyPressed(OF_KEY_SHIFT)) {
    // Hanya boleh show semua jika TIDAK sedang sequential drawing
    if (!currentTemplate || !currentTemplate->sequentialMode) {
      showAllShapes();
    }
  }

  // FIX: Juga handle SHIFT+) yang terbaca sebagai '+' atau '='
  if ((key == '+' || key == '=') && ofGetKeyPressed(OF_KEY_SHIFT)) {
    // Hanya boleh show semua jika TIDAK sedang sequential drawing
    if (!currentTemplate || !currentTemplate->sequentialMode) {
      showAllShapes();
      return; // Return agar tidak trigger increaseLineWidth
    }
  }

  if (key == '-' || key == '_') {
       // Kurangi line width secara bertahap
      if (currentTemplate) {
          currentTemplate->lineWidth -= 0.5f;

          // Batasi minimum line width
          if (currentTemplate->lineWidth < 0.f) {
              currentTemplate->lineWidth = 0.f;
          }
          updateLineWidth();
      }
  }

  if (key == '+' || key == '=') {
      // Tambah line width secara bertahap
      if (currentTemplate) {
          currentTemplate->lineWidth += 0.5f;

          // Batasi maximum line width
          if (currentTemplate->lineWidth > 4.0f) {
              currentTemplate->lineWidth = 4.0f;
          }
          updateLineWidth();
      }
  }

  // Handle CTRL key ditekan
  if (key == 3682 || key == 3683) { // HANYA 3682/3683
    isCtrlPressed = true;
    return; // Jangan lanjut ke logic lain
  }

  // SHIFT+B = Select SEMUA customLines (bisa 'B' atau 'b' karena CAPSLOCK)
  if ((key == 'B' || key == 'b') && ofGetKeyPressed(OF_KEY_SHIFT)) {
    for (int i = 0; i < customLines.size(); i++) {
      selectedLineIndices.insert(i);
    }
    return;
  }

  // Handle tombol saat CTRL aktif - gunakan switch-case
  if (isCtrlPressed) {
    switch (key) {
    case 'z':
    case 'Z':
    case 26: // CTRL+Z (ASCII 26)
      // Cek apakah SHIFT juga ditekan (CTRL+SHIFT+Z untuk Redo)
      if (ofGetKeyPressed(OF_KEY_SHIFT)) {
        redo();
      } else {
        undo();
      }
      break;

    case 's':
    case 'S':
    case 19: // CTRL+S (ASCII 19)
      // Cek apakah SHIFT juga ditekan
      if (ofGetKeyPressed(OF_KEY_SHIFT)) {
          saveWorkspaceAs();
      } else {
          saveWorkspace();
      }
      break;

    case 'o':
    case 'O':
    case 15: // CTRL+O (ASCII 15)
        openWorkspace();
      break;

    case 'g':
    case 'G':
    case 7: // CTRL+G (ASCII 7)
      createInvisiblePolygonFromSelected();
      break;
    }
  }

  // G/g - Toggle ImGui visibility (HANYA tanpa CTRL)
  if ((key == 'g' || key == 'G') && !isCtrlPressed) {
    if (!imguiVisible) {
      // Show MenuBar only
      imguiVisible = true;
      showSacredGeometry = false;
      showPlayground = false;
    } else if (!showSacredGeometry && !showPlayground) {
      // Show all windows
      showSacredGeometry = true;
      showPlayground = true;
    } else {
      // Hide all
      imguiVisible = false;
      showSacredGeometry = false;
      showPlayground = false;
    }
  }

  // Keys 1-9 - Assign color to selected polygon
  if (key >= 49 && key <= 57) { // '1' to '9'
    int colorIndex = key - 49;  // 0 to 8
    if (selectedPolygonIndex >= 0 &&
        selectedPolygonIndex < polygonShapes.size()) {
      polygonShapes[selectedPolygonIndex].setColor(
          polygonPresetColors[colorIndex]);
    }
  }
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
      if (shape && shape->showing) {
        shape->addDotsToCache(cachedDots);
      }
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
  // Euclidean distance
  // ukuran jarak antara 2 titik
  float distance = glm::length(mousePos - dotPos);
  return distance <= this->threshold;
}

bool ofApp::lineExists(vec2 from, vec2 to) {
  // Cek apakah ada line dengan titik awal dan akhir yang sama
  for (auto &line : customLines) {
    const vector<vec2> &points = line.getPoints();
    if (points.size() >= 2) {
      vec2 lineStart = points.front();
      vec2 lineEnd = points.back();

      if ((glm::length(lineStart - from) < 1.0f &&
           glm::length(lineEnd - to) < 1.0f) ||
          (glm::length(lineStart - to) < 1.0f &&
           glm::length(lineEnd - from) < 1.0f)) {
        return true;
      }
    }
  }
  return false;
}

//--------------------------------------------------------------
bool ofApp::isMouseOverLine(vec2 mousePos, vec2 lineStart, vec2 lineEnd,
                            float lineWidth) {
  // Hitung jarak dari titik ke garis (point-to-line distance)
  // Formula: jarak = |(end - start) × (start - point)| / |end - start|
  vec2 lineVec = lineEnd - lineStart;
  vec2 pointVec = mousePos - lineStart;
  float lineLength = glm::length(lineVec);

  if (lineLength == 0)
    return false;

  // Project pointVec ke lineVec
  float projection = glm::dot(pointVec, lineVec) / lineLength;
  vec2 closestPoint = lineStart + (lineVec / lineLength) * projection;

  // Clamp projection ke line segment (biar tidak extend)
  if (projection < 0)
    closestPoint = lineStart;
  if (projection > 1)
    closestPoint = lineEnd;

  // Hitung jarak dari mousePos ke closestPoint di garis
  float distance = glm::length(mousePos - closestPoint);

  // Threshold = setengah line width + sedikit buffer
  return distance <= (lineWidth / 2.0f) + 2.0f;
}

//--------------------------------------------------------------
float ofApp::distanceToLine(vec2 point, vec2 lineStart, vec2 lineEnd,
                            float curve) {
  // Hitung jarak terpendek dari point ke garis/lengkungan

  // Jika curve != 0, hitung jarak ke bezier curve
  if (curve != 0.0f) {
    // Hitung control point
    vec2 midPoint = (lineStart + lineEnd) / 2.0f;
    vec2 dir = lineEnd - lineStart;
    vec2 perp = vec2(-dir.y, dir.x);
    float perpLen = glm::length(perp);
    if (perpLen > 0) {
      perp = perp / perpLen;
    }
    vec2 controlPoint = midPoint + perp * curve;

    // Sampling bezier curve untuk cari jarak minimum
    float minDistance = 999999.0f;
    int samples = 50;

    for (int i = 0; i <= samples; i++) {
      float t = (float)i / samples;
      // Quadratic bezier
      vec2 curvePoint = lineStart * (1 - t) * (1 - t) +
                        controlPoint * 2 * (1 - t) * t + lineEnd * t * t;

      float distance = glm::length(point - curvePoint);
      if (distance < minDistance) {
        minDistance = distance;
      }
    }

    return minDistance;
  } else {
    // Hitung jarak ke garis lurus (original code)
    vec2 lineVec = lineEnd - lineStart;
    vec2 pointVec = point - lineStart;
    float lineLengthSq = glm::dot(lineVec, lineVec);

    if (lineLengthSq == 0)
      return 0.0f;

    // Project pointVec ke lineVec (hasilnya parameter t dalam [0,1])
    float projection = glm::dot(pointVec, lineVec) / lineLengthSq;

    // Clamp projection ke line segment (biar tidak extend)
    projection = ofClamp(projection, 0.0f, 1.0f);

    vec2 closestPoint = lineStart + lineVec * projection;

    // Hitung jarak dari point ke closestPoint
    return glm::length(point - closestPoint);
  }
}

//--------------------------------------------------------------
int ofApp::getLineIndexAtPosition(vec2 pos) {
  // Cari garis dengan JARAK TERDEKAT
  int closestLineIndex = -1;
  float closestDistance = 999999.0f;

  for (int i = 0; i < customLines.size(); i++) {
    const CustomLine &line = customLines[i];
    const vector<vec2> &points = line.getPoints();
    if (points.size() >= 2) {
      vec2 start = points[0];
      vec2 end = points[1];

      // Skip invalid lines (start == end)
      if (glm::length(start - end) < 1.0f) {
        continue;
      }

      // Hitung jarak ke garis ini (dengan curve support)
      float distance = distanceToLine(pos, start, end, line.getCurve());

      // Update jika lebih dekat
      if (distance < closestDistance) {
        closestDistance = distance;
        closestLineIndex = i;
      }
    }
  }

  // Return line terdekat (jika dalam threshold)
  if (closestDistance <= (mouseLineWidth / 2.0f) + 2.0f) {
    return closestLineIndex;
  }

  return -1; // Tidak ada yang diklik
}

//--------------------------------------------------------------
void ofApp::pushUndoAction(UndoAction action) {
	// Saat action baru, clear redo stack (redo tidak valid lagi)
	clearRedoStack();

	// Add ke stack, jika sudah full, hapus yang paling lama
	if (undoStack.size() >= MAX_UNDO_STEPS) {
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
				if (!selectedLineIndices.empty()) {
					selectedLineIndices.clear();
					lastSelectedLineIndex = -1;
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
				selectedPolygonIndex = -1;
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
			if (undoStack.size() >= MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case CREATE_POLYGON:
			// Redo create polygon = create polygon lagi
			polygonShapes.push_back(action.deletedPolygon);
			// Push langsung ke undo stack
			if (undoStack.size() >= MAX_UNDO_STEPS) {
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

			// Apply redo colors dan simpan current colors
			for (size_t i = 0; i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
				int idx = action.colorIndices[i];
				if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
					newUndoAction.oldColors.push_back(customLines[idx].getColor());
					customLines[idx].setColor(action.oldColors[i]);
				}
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= MAX_UNDO_STEPS) {
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
					polygonShapes[idx].setColor(action.oldColors[0]);
				}
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(newUndoActionPoly);
			break;
		}

		case DELETE_LINE:
			// Redo = delete line lagi
			if (action.deletedLineIndex >= 0 && action.deletedLineIndex < static_cast<int>(customLines.size())) {
				customLines.erase(customLines.begin() + action.deletedLineIndex);
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(action);
			break;

		case DELETE_POLYGON:
			// Redo = delete polygon lagi
			if (action.deletedPolygonIndex >= 0 && action.deletedPolygonIndex < static_cast<int>(polygonShapes.size())) {
				polygonShapes.erase(polygonShapes.begin() + action.deletedPolygonIndex);
			}
			// Push langsung ke undo stack
			if (undoStack.size() >= MAX_UNDO_STEPS) {
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
			if (undoStack.size() >= MAX_UNDO_STEPS) {
				undoStack.erase(undoStack.begin());
			}
			undoStack.push_back(newUndoActionCurve);
			break;
		}
	}
}


//--------------------------------------------------------------
void ofApp::createInvisiblePolygonFromSelected() {
  if (selectedLineIndices.empty())
    return; // Tidak ada yang selected

  // 1. Extract semua titik dari selected lines (pakai sampled points untuk
  // curve)
  vector<vec2> allPoints;
  for (int lineIndex : selectedLineIndices) {
    if (lineIndex >= 0 && lineIndex < customLines.size()) {
      auto sampledPoints = customLines[lineIndex].getSampledPoints();
      if (!sampledPoints.empty()) {
        // Tambahkan semua sampled points ke allPoints
        for (auto &p : sampledPoints) {
          allPoints.push_back(p);
        }
      }
    }
  }

  if (allPoints.size() < 3) {
    // Kurang dari 3 titik, tidak bisa buat polygon
    return;
  }

  
  int polygonIndex =
      static_cast<int>(polygonShapes.size());
  PolygonShape newPolygon(allPoints, polygonColor,
                          polygonIndex);
  polygonShapes.push_back(newPolygon);

  // Push undo action
  UndoAction undoAction;
  undoAction.type = CREATE_POLYGON;
  undoAction.isCreate = true;
  pushUndoAction(undoAction);

  // 3. Clear selection
  selectedLineIndices.clear();
  lastSelectedLineIndex = -1;
}

//--------------------------------------------------------------
// File operations sekarang ditangani oleh FileManager class

//--------------------------------------------------------------
// Mouse Event Handlers
void ofApp::mousePressed(int x, int y, int button) {
  // Forward ke ImGui DULU (sebelum logic lain!)
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseButtonEvent(button, true);

  // Cek apakah ImGui mau capture mouse (termasuk drag window)
  if (io.WantCaptureMouse) {
    return; // ImGui handle, jangan process di OF
  }

  // Logic lama: Cursor toggle dengan right click (button 2)
  if (button == 2) {
    cursorVisible = !cursorVisible;
    if (cursorVisible)
      ofShowCursor();
    if (!cursorVisible)
      ofHideCursor();
    return; // Jangan lanjut ke interactive line creation untuk right click
  }

  // CTRL+Click untuk multi-select
  if (button == 0 && isCtrlPressed) {
    vec2 adjustedMousePos(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
    int clickedLineIndex = getLineIndexAtPosition(adjustedMousePos);

    if (clickedLineIndex >= 0) {
      // Toggle selection
      if (selectedLineIndices.count(clickedLineIndex)) {
        selectedLineIndices.erase(clickedLineIndex); // Deselect
      } else {
        selectedLineIndices.insert(clickedLineIndex); // Select
      }
      lastSelectedLineIndex = clickedLineIndex;
    }
    return; // Jangan lanjut ke logic drag
  }

  // Logic untuk interactive line creation & line selection
  if (button == 0) {
    // Adjust mouse position untuk center translation
    vec2 adjustedMousePos(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
    vector<DotInfo> dots = getAllDots();

    // Check jika klik di atas dot - HANYA bisa mulai dari dot
    bool clickedOnDot = false;
    for (auto &dot : dots) {
      if (isMouseOverDot(adjustedMousePos, dot.position)) {
        drawState = DRAGGING;
        currentPolylinePoints.clear();
        currentPolylinePoints.push_back(dot.position); // Start dari dot
        startDotPos = dot.position;
        mousePos = adjustedMousePos;
        clickedOnDot = true;

        // Deselect line yang mungkin terpilih
        selectedLineIndices.clear();

        return;
      }
    }

    // Kalau tidak klik di dot, cek apakah klik di garis untuk SELECT
    if (!clickedOnDot) {
      // CEK POLYGON DULU (sebelum cek garis)
      bool clickedOnPolygon = false;
      for (int i = 0; i < polygonShapes.size(); i++) {
        if (polygonShapes[i].containsPoint(adjustedMousePos)) {
          // Select polygon ini
          selectedPolygonIndex = i;
          selectedLineIndices.clear(); // Deselect semua lines
          clickedOnPolygon = true;
          break;
        }
      }

      // Kalau tidak klik di polygon, cek garis
      if (!clickedOnPolygon) {
        int lineIndex = getLineIndexAtPosition(adjustedMousePos);
        if (lineIndex >= 0) {
          // Select garis ini (single select: hapus yang lama, select yang baru)
          selectedLineIndices.clear();
          selectedLineIndices.insert(lineIndex);
          selectedPolygonIndex = -1; // Deselect polygon
        } else {
          // Klik di tempat kosong → deselect semua
          selectedLineIndices.clear();
          selectedPolygonIndex = -1; // Deselect polygon
        }
      }
    }
  }
}

void ofApp::mouseDragged(int x, int y, int button) {
  // Forward ke ImGui DULU (untuk drag window)
  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent(x, y);

  // Cek apakah ImGui mau capture mouse
  if (io.WantCaptureMouse) {
    return; // ImGui handle, jangan process di OF
  }

  if (drawState == DRAGGING) {
    // Adjust mouse position untuk center translation
    vec2 currentPos = vec2(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
    mousePos = currentPos;

    // Untuk garis lurus, kita hanya perlu 2 titik: start dan end
    // currentPolylinePoints[0] = start (diset di mousePressed)
    // currentPolylinePoints[1] = end (diupdate setiap drag)
    if (currentPolylinePoints.size() < 2) {
      currentPolylinePoints.push_back(currentPos); // Add end point
    } else {
      currentPolylinePoints[1] = currentPos; // Update end point
    }
  }
}

void ofApp::mouseReleased(int x, int y, int button) {
  // Forward ke ImGui
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseButtonEvent(button, false);

  // Biarkan ImGui handle semua

  if (drawState != DRAGGING) {
    return;
  }

  // Adjust mouse position untuk center translation
  vec2 releasePos(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
  vector<DotInfo> dots = getAllDots();

  // Check jika release di atas valid dot - HANYA bisa selesai di dot
  for (auto &dot : dots) {
    if (isMouseOverDot(releasePos, dot.position)) {
      // Cek apakah line sudah ada (duplicate check)
      if (!lineExists(startDotPos, dot.position)) {
        // Cek apakah start != end (bukan garis panjang 0)
        if (glm::length(startDotPos - dot.position) > 1.0f) {
          // Update end point ke posisi dot yang dilepas
          if (currentPolylinePoints.size() < 2) {
            currentPolylinePoints.push_back(dot.position);
          } else {
            currentPolylinePoints[1] = dot.position;
          }

          // Simpan polyline dengan 2 points (start dan end)
          CustomLine newLine(currentPolylinePoints, customLineColor,
                             mouseLineWidth);
          customLines.push_back(newLine);

          // Push undo action
          UndoAction undoAction;
          undoAction.type = CREATE_LINE;
          undoAction.isCreate = true;
          pushUndoAction(undoAction);
        }
      }
      break;
    }
  }

  // Clear dan reset state
  currentPolylinePoints.clear();
  drawState = IDLE;
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
  // Forward ke ImGui DULU
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseWheelEvent(scrollX, scrollY);

  // Cek apakah ImGui mau capture mouse
  if (io.WantCaptureMouse) {
    return; // ImGui handle, jangan process di OF
  }

  // Update curve untuk SEMUA garis yang selected
  if (!selectedLineIndices.empty()) {
    // Siapkan undo action
    UndoAction undoAction;
    undoAction.type = CHANGE_CURVE;
    undoAction.newCurve = 0.0f;  // Placeholder, tidak dipakai

    float curveSpeed = 5.0f; // Kecepatan perubahan curve
    for (int lineIndex : selectedLineIndices) {
      if (lineIndex >= 0 && lineIndex < customLines.size()) {
        // Simpan old curve SEBELUM mengubah
        undoAction.curveLineIndices.push_back(lineIndex);
        undoAction.oldCurves.push_back(customLines[lineIndex].getCurve());

        // Hitung new curve dan update
        float newCurve = customLines[lineIndex].getCurve() + scrollY * curveSpeed;
        undoAction.newCurve = newCurve;  // Semua lines dapat curve baru yang sama
        customLines[lineIndex].setCurve(newCurve);
      }
    }
    // Push undo action hanya jika ada yang diubah
    if (!undoAction.curveLineIndices.empty()) {
      pushUndoAction(undoAction);
    }
  }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
  // Forward ke ImGui
  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent(x, y);

  // Update mouse position untuk hover detection (adjust untuk center translation)
  mousePos = vec2(x - ofGetWidth() / 2, y - ofGetHeight() / 2);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
  if (key == OF_KEY_CONTROL)
    isCtrlPressed = false;
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
void ofApp::hideAllShapes() {
  if (!currentTemplate) return;

  // Delegate ke template
  currentTemplate->hideAllShapes();

  // Mark dots cache dirty karena visibility berubah
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::showAllShapes() {
  if (!currentTemplate) return;

  // Delegate ke template
  currentTemplate->showAllShapes();

  // Mark dots cache dirty karena visibility berubah
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::setCartesianAxesVisibility(bool show) {
  if (!currentTemplate) return;

  const auto& shapes = currentTemplate->getShapes();
  if (!shapes.empty()) {
    AbstractShape* cartesianAxes = shapes[0].get(); // CartesianAxes selalu index 0
    if (show) {
      cartesianAxes->show();
    } else {
      cartesianAxes->hide();
    }
  }

  // Mark dots cache dirty karena visibility berubah
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::updateLineWidth() {
    if (!currentTemplate) return;

    // Delegate ke template - pakai nilai yang sudah ada di template
    currentTemplate->updateLineWidth(currentTemplate->lineWidth);
}

//--------------------------------------------------------------
void ofApp::updateCustomLineColor(ofColor color) {
	// Update variabel global untuk customLine baru
	customLineColor = color;

	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_LINE;
	undoAction.newColor = color;

	// Jika ada customLine yang selected, hanya update yang selected saja
	if (!selectedLineIndices.empty()) {
		for (int lineIndex : selectedLineIndices) {
			if (lineIndex >= 0 && lineIndex < customLines.size()) {
				// Simpan old color SEBELUM mengubah
				undoAction.colorIndices.push_back(lineIndex);
				undoAction.oldColors.push_back(customLines[lineIndex].getColor());

				// Ubah warna
				customLines[lineIndex].setColor(color);
			}
		}
		// Push undo action hanya jika ada yang diubah
		if (!undoAction.colorIndices.empty()) {
			pushUndoAction(undoAction);
		}
	}
	// Jika tidak ada yang selected, update semua customLines
	else {
		for (size_t i = 0; i < customLines.size(); i++) {
			// Simpan old color SEBELUM mengubah
			undoAction.colorIndices.push_back(i);
			undoAction.oldColors.push_back(customLines[i].getColor());

			// Ubah warna
			customLines[i].setColor(color);
		}
		// Push undo action hanya jika ada yang diubah
		if (!undoAction.colorIndices.empty()) {
			pushUndoAction(undoAction);
		}
	}
}

//--------------------------------------------------------------
void ofApp::updatePolygonColor(ofColor color) {
	// Update variabel global untuk polygon baru
	polygonColor = color;

	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_POLYGON;
	undoAction.newColor = color;

	// Jika ada polygon yang selected, hanya update yang selected saja
	if (selectedPolygonIndex >= 0 && selectedPolygonIndex < polygonShapes.size()) {
		// Simpan old color SEBELUM mengubah
		undoAction.colorIndices.push_back(selectedPolygonIndex);
		undoAction.oldColors.push_back(polygonShapes[selectedPolygonIndex].getColor());

		// Ubah warna
		polygonShapes[selectedPolygonIndex].setColor(color);

		// Push undo action
		pushUndoAction(undoAction);
	}
	// Jika tidak ada yang selected, update semua polygons
	else {
		for (size_t i = 0; i < polygonShapes.size(); i++) {
			// Simpan old color SEBELUM mengubah
			undoAction.colorIndices.push_back(i);
			undoAction.oldColors.push_back(polygonShapes[i].getColor());

			// Ubah warna
			polygonShapes[i].setColor(color);
		}
		// Push undo action hanya jika ada yang diubah
		if (!undoAction.colorIndices.empty()) {
			pushUndoAction(undoAction);
		}
	}
}

//--------------------------------------------------------------
void ofApp::syncColorPickerFromLoadedLines() {
	// Ambil warna dari customLine pertama yang diload (jika ada)
	if (!customLines.empty()) {
		ofColor loadedColor = customLines[0].getColor();

		// Update global color
		customLineColor = loadedColor;

		// Update SacredGeometry color picker UI
		for (auto& gui : guiComponents) {
			// Cari SacredGeometry component
			SacredGeometry* sacredGeo = dynamic_cast<SacredGeometry*>(gui.get());
			if (sacredGeo) {
				sacredGeo->updateColorFromApp();
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

		// Update global color
		polygonColor = loadedColor;

		// Update SacredGeometry color picker UI
		for (auto& gui : guiComponents) {
			// Cari SacredGeometry component
			SacredGeometry* sacredGeo = dynamic_cast<SacredGeometry*>(gui.get());
			if (sacredGeo) {
				sacredGeo->updatePolygonColorFromApp();
				break; // Found, no need to continue
			}
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
	selectedLineIndices.clear();
	lastSelectedLineIndex = -1;
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
		selectedPolygonIndex = -1;
	}
}

//--------------------------------------------------------------
void ofApp::cleanCanvas() {
    // Skip kalau sedang load sequential
    if (fileManager.isLoadSequentialMode()) {
        return;
    }

    // Hapus semua polygons dan custom lines
    clearCustomLinesAndPolygons();

    // Hide semua template shapes - delegate ke template
    if (currentTemplate) {
        currentTemplate->hideAllShapes();
    }
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
	}

	// Scale semua polygons
	for (auto& polygon : polygonShapes) {
		vector<vec2> vertices = polygon.getVertices();
		for (auto& vertex : vertices) {
			vertex = vertex * scaleRatio;  // Scale setiap vertex
		}
		polygon.setVertices(vertices);
	}
}

//--------------------------------------------------------------
bool ofApp::isCanvasEmpty() {
	// Cek apakah ada template shapes yang showing
	if (currentTemplate) {
		const auto& shapes = currentTemplate->getShapes();
		for (const auto& shape : shapes) {
			if (shape && shape->showing) {
				return false;  // Ada template yang showing, canvas tidak kosong
			}
		}
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
void ofApp::saveWorkspace() {
  if (!currentTemplate) {
    return;
  }

  // Kalau belum pernah Save As, langsung buka dialog (Save As)
  if (lastSavedPath.empty()) {
    saveWorkspaceAs();
    return;
  }

  // Sudah ada Save As sebelumnya, save langsung ke file tersebut
  fileManager.saveAll(currentTemplate->getName(), currentTemplate->radius,
                      customLines, polygonShapes, currentTemplate->lineWidth,
                      currentTemplate->labelsVisible, currentTemplate->dotsVisible,
                      lastSavedPath);

  // Show MenuBar agar popup terlihat
  imguiVisible = true;
  showSacredGeometry = false;
  showPlayground = false;
  successPopup->show();
}

//--------------------------------------------------------------
void ofApp::saveWorkspaceAs() {
  if (!currentTemplate) {
    return;  // Tidak ada template aktif
  }

  // Buka save dialog untuk pilih lokasi dan nama file
  ofFileDialogResult saveDialog = ofSystemSaveDialog("workspace.nay", "Save Workspace As");

  // Cek apakah user memilih file (tidak cancel)
  if (saveDialog.getPath().empty()) {
    return;  // User cancel
  }

  // Ambil filepath dari dialog
  string filepath = saveDialog.getPath();

  // Tambah extension .nay kalau belum ada
  if (filepath.find(".nay") == string::npos) {
    filepath += ".nay";
  }

  // Simpan langsung ke filepath yang user pilih
  fileManager.saveAll(currentTemplate->getName(), currentTemplate->radius,
                      customLines, polygonShapes, currentTemplate->lineWidth,
                      currentTemplate->labelsVisible, currentTemplate->dotsVisible,
                      filepath);

  // Simpan path ini sebagai lastSavedPath (CTRL+S selanjutnya akan kesini)
  lastSavedPath = filepath;

  // Show MenuBar agar popup terlihat
  imguiVisible = true;
  showSacredGeometry = false;
  showPlayground = false;
  successPopup->show();
}

//--------------------------------------------------------------
void ofApp::openWorkspace() {
  // Buka open file dialog
  ofFileDialogResult openDialog = ofSystemLoadDialog("Open Workspace", false);

  // Cek apakah user memilih file (tidak cancel)
  if (openDialog.getPath().empty()) {
    return;  // User cancel
  }

  // Ambil filepath dari dialog
  string filepath = openDialog.getPath();

  // Cek apakah file extension .nay
  if (filepath.find(".nay") == string::npos) {
    // File bukan format .nay, tampilkan error popup
    // Show MenuBar agar popup terlihat
    imguiVisible = true;
    showSacredGeometry = false;
    showPlayground = false;
    errorPopup->show("Invalid File Format",
                     "Please select a .nay file format!",
                     "OK");
    return;
  }

  // Set lastSavedPath ke filepath yang di-open (untuk load dan save)
  lastSavedPath = filepath;

  // Show dan focus Playground window (file valid, akan diload)
  imguiVisible = true;
  showPlayground = true;

  // Set windowOpen flag dan focus ke Playground
  for (auto& gui : guiComponents) {
    Playground* playground = dynamic_cast<Playground*>(gui.get());
    if (playground) {
      playground->showWindow();
      playground->focusWindow();
      break;
    }
  }
}

//--------------------------------------------------------------
void ofApp::loadWorkspace() {
    // NOTE: Validasi canvas sudah dilakukan di UI level (Playground)
    // Jangan cek lagi di sini agar autoCleanCanvas bisa berfungsi

    // Cek apakah ada file yang di-open (lastSavedPath)
    if (lastSavedPath.empty()) return;

    // Load workspace dengan template name, radius, dan ALL settings
    string loadedTemplateName;
    float loadedRadius;
    float loadedLineWidth;
    bool loadedLabelsVisible;
    bool loadedDotsVisible;

    if (fileManager.loadAll(loadedTemplateName, loadedRadius, customLines,
        polygonShapes, loadedLineWidth,
        loadedLabelsVisible, loadedDotsVisible, lastSavedPath)) {

        // Switch ke template yang di-load DULU
        switchTemplate(loadedTemplateName);

        // Rebuild shapes dengan loaded radius yang benar!
        currentTemplate->setRadius(loadedRadius);

        // Update tracking untuk scaling
        previousRadius = loadedRadius;  // Reset tracking agar tidak scaling saat load

        // Sync Settings ke template
        currentTemplate->lineWidth = loadedLineWidth;
        currentTemplate->labelsVisible = loadedLabelsVisible;
        currentTemplate->dotsVisible = loadedDotsVisible;

        // Apply settings ke semua shapes yang baru di-load
        const auto& shapes = currentTemplate->getShapes();
        for (auto& shape : shapes) {
            shape->setLineWidth(currentTemplate->lineWidth);

            if (currentTemplate->labelsVisible)
                shape->showLabel();
            else
                shape->hideLabel();

            if (currentTemplate->dotsVisible)
                shape->showDot();
            else
                shape->hideDot();

            shape->setSequentialMode(false);  // PARALLEL mode (CTRL+O)
        }

        // Mulai staggered parallel load
        showAllShapes();  // Ini akan memulai animasi dari awal

        // Apply Cartesian, Circle, & CrossLine preferensi dari Playground SETELAH shapes dibuat
        if (currentTemplate) {
            const auto& shapes = currentTemplate->getShapes();
            if (!shapes.empty()) {
                // Index 0: CartesianAxes
                AbstractShape* cartesianAxes = shapes[0].get();
                if (currentTemplate->showCartesianOnPlay) {
                    cartesianAxes->show();
                } else {
                    cartesianAxes->hide();
                }

                // Circle A-E menggunakan circleIndices (fleksibel!)
                for (int circleIndex : currentTemplate->circleIndices) {
                    if (circleIndex < shapes.size()) {
                        if (currentTemplate->showCirclesOnPlay) {
                            shapes[circleIndex]->show();
                        } else {
                            shapes[circleIndex]->hide();
                        }
                    }
                }

                // CrossLine F-I menggunakan crossLineIndices (fleksibel!)
                for (int crossLineIndex : currentTemplate->crossLineIndices) {
                    if (crossLineIndex < shapes.size()) {
                        if (currentTemplate->showCrossLinesOnPlay) {
                            shapes[crossLineIndex]->show();
                        } else {
                            shapes[crossLineIndex]->hide();
                        }
                    }
                }

                // Parallelogram N-Q menggunakan parallelogramIndices (fleksibel!)
                for (int parallelogramIndex : currentTemplate->parallelogramIndices) {
                    if (parallelogramIndex < shapes.size()) {
                        if (currentTemplate->showParallelogramsOnPlay) {
                            shapes[parallelogramIndex]->show();
                        } else {
                            shapes[parallelogramIndex]->hide();
                        }
                    }
                }

                // RectangleLine RS, TU, VW, XY menggunakan rectangleLineIndices (fleksibel!)
                for (int rectangleLineIndex : currentTemplate->rectangleLineIndices) {
                    if (rectangleLineIndex < shapes.size()) {
                        if (currentTemplate->showRectangleLinesOnPlay) {
                            shapes[rectangleLineIndex]->show();
                        } else {
                            shapes[rectangleLineIndex]->hide();
                        }
                    }
                }

                // OctagramLine 0-7 menggunakan octagramLineIndices (fleksibel!)
                for (int octagramLineIndex : currentTemplate->octagramLineIndices) {
                    if (octagramLineIndex < shapes.size()) {
                        if (currentTemplate->showOctagramLinesOnPlay) {
                            shapes[octagramLineIndex]->show();
                        } else {
                            shapes[octagramLineIndex]->hide();
                        }
                    }
                }
            }
        }

        // Matikan parallel dulu supaya customLines tidak langsung di-animate
        fileManager.setLoadParallelMode(false);

        // Sync ColorPicker dengan warna customLines yang diload
        syncColorPickerFromLoadedLines();
        syncColorPickerFromLoadedPolygons();

        loadStage = LOAD_TEMPLATE;
        isStaggeredLoad = true;
        isSequentialShapeLoad = false;  // PARALLEL mode (CTRL+O)
        currentState = UpdateState::STAGGERED_LOAD;  // STRATEGY PATTERN: Set state ke STAGGERED_LOAD
    }
}

void ofApp::loadWorkspaceSeq() {
    // NOTE: Validasi canvas sudah dilakukan di UI level (Playground)
    // Jangan cek lagi di sini agar autoCleanCanvas bisa berfungsi

    // Cek apakah ada file yang di-open (lastSavedPath)
    if (lastSavedPath.empty()) return;

    // Clear customLines dan polygons yang sudah ada sebelumnya
    // HARUS DILAKUKAN SEBELUM loadAllSequential agar buffer diisi dengan benar!
    customLines.clear();
    selectedLineIndices.clear();
    lastSelectedLineIndex = -1;
    polygonShapes.clear();
    selectedPolygonIndex = -1;

    // Sequential load dengan animasi
    string loadedTemplateName;
    float loadedRadius;
    float loadedLineWidth;
    bool loadedLabelsVisible;
    bool loadedDotsVisible;

    fileManager.loadAllSequential(loadedTemplateName, loadedRadius,
        loadedLineWidth, loadedLabelsVisible, loadedDotsVisible,
        customLines, polygonShapes, lastSavedPath);

    // Switch ke template yang di-load DULU
    switchTemplate(loadedTemplateName);

    // Rebuild shapes dengan loaded radius yang benar!
    currentTemplate->setRadius(loadedRadius);

    // Update tracking untuk scaling
    previousRadius = loadedRadius;  // Reset tracking agar tidak scaling saat load

    // Sync Settings ke template
    currentTemplate->lineWidth = loadedLineWidth;
    currentTemplate->labelsVisible = loadedLabelsVisible;
    currentTemplate->dotsVisible = loadedDotsVisible;

    // Apply settings ke semua template shapes
    const auto& shapes = currentTemplate->getShapes();
    for (auto& shape : shapes) {
        shape->setLineWidth(currentTemplate->lineWidth);
        if (currentTemplate->labelsVisible)
            shape->showLabel();
        else
            shape->hideLabel();
        if (currentTemplate->dotsVisible)
            shape->showDot();
        else
            shape->hideDot();
    }

    showAllShapes();  // Reset animasi template

    // Apply Cartesian, Circle, & CrossLine preferensi dari Playground SETELAH showAllShapes
    if (currentTemplate) {
        const auto& shapes = currentTemplate->getShapes();
        if (!shapes.empty()) {
            // Index 0: CartesianAxes
            AbstractShape* cartesianAxes = shapes[0].get();
            if (currentTemplate->showCartesianOnPlay) {
                cartesianAxes->show();
            } else {
                cartesianAxes->hide();
            }

            // Circle A-E menggunakan circleIndices (fleksibel!)
            for (int circleIndex : currentTemplate->circleIndices) {
                if (circleIndex < shapes.size()) {
                    if (currentTemplate->showCirclesOnPlay) {
                        shapes[circleIndex]->show();
                    } else {
                        shapes[circleIndex]->hide();
                    }
                }
            }

            // CrossLine F-I menggunakan crossLineIndices (fleksibel!)
            for (int crossLineIndex : currentTemplate->crossLineIndices) {
                if (crossLineIndex < shapes.size()) {
                    if (currentTemplate->showCrossLinesOnPlay) {
                        shapes[crossLineIndex]->show();
                    } else {
                        shapes[crossLineIndex]->hide();
                    }
                }
            }

            // Parallelogram N-Q menggunakan parallelogramIndices (fleksibel!)
            for (int parallelogramIndex : currentTemplate->parallelogramIndices) {
                if (parallelogramIndex < shapes.size()) {
                    if (currentTemplate->showParallelogramsOnPlay) {
                        shapes[parallelogramIndex]->show();
                    } else {
                        shapes[parallelogramIndex]->hide();
                    }
                }
            }

            // RectangleLine RS, TU, VW, XY menggunakan rectangleLineIndices (fleksibel!)
            for (int rectangleLineIndex : currentTemplate->rectangleLineIndices) {
                if (rectangleLineIndex < shapes.size()) {
                    if (currentTemplate->showRectangleLinesOnPlay) {
                        shapes[rectangleLineIndex]->show();
                    } else {
                        shapes[rectangleLineIndex]->hide();
                    }
                }
            }

            // OctagramLine 0-7 menggunakan octagramLineIndices (fleksibel!)
            for (int octagramLineIndex : currentTemplate->octagramLineIndices) {
                if (octagramLineIndex < shapes.size()) {
                    if (currentTemplate->showOctagramLinesOnPlay) {
                        shapes[octagramLineIndex]->show();
                    } else {
                        shapes[octagramLineIndex]->hide();
                    }
                }
            }
        }
    }

    // Set sequential mode SETELAH showAllShapes (karena showAllShapes akan reset ke false)
    for (auto& shape : shapes) {
        shape->setSequentialMode(true);
    }

    fileManager.setLoadParallelMode(false);
    loadStage = LOAD_TEMPLATE;
    isStaggeredLoad = true;
    isSequentialShapeLoad = true;  // Sequential per shape
    currentTemplateIndex = 0;  // Reset index template
    currentState = UpdateState::STAGGERED_LOAD;  // STRATEGY PATTERN: Set state ke STAGGERED_LOAD
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

    // Initialize popup (not in guiComponents, drawn separately)
    successPopup = std::make_unique<SuccessPopup>(this);
    errorPopup = std::make_unique<ErrorPopup>(this);
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

    // Draw popup dialogs
    successPopup->draw();
    errorPopup->draw();

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

