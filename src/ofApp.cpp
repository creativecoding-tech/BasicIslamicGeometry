#include "ofApp.h"
#include "operation/UltralightManager.h" // Untuk SharedRendererManager
#include "shape/AbstractShape.h"
#include "template/templates/BasicZelligeTemplate.h"


//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(false); // Tidak ada limit dari refresh rate monitor
  ofSetFrameRate(60);       // Sync dengan Ultralight 60 FPS cap
  ofSetEscapeQuitsApp(false);
  ofSetBackgroundAuto(
      false); // Coba ubah ke TRUE untuk test (hilangkan trail effect)
  ofEnableAntiAliasing();
  ofEnableSmoothing();

  // Load font untuk custom line labels
  fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);

  // STEP 1: Setup template system (REGISTER SEMUA TEMPLATES)
  setupTemplateSystem();

  // Setup Dialog UI untuk wizard (600x400 centered)
  setupDialogUI();

  // Show dialog pertama (mode selection)
  showModeDialog();

  // STEP 2: Load default template (Basic Zellige)
  switchTemplate("Basic Zellige");

  // Initial dots cache build (cache di-build saat pertama kali getAllDots()
  // dipanggil)
  dotsCacheDirty = true;
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
  currentTemplate = registry.getTemplate(templateName);

  // Null check - template tidak ditemukan
  if (!currentTemplate) {
    // Template tidak ditemukan, jangan lanjut
    return;
  }

  // Clear shapes lama
  templateShapes.clear();

  // Setup shapes baru dari template
  currentTemplate->setupShapes(templateShapes);

  // Update radius dari template (SAFE: currentTemplate sudah di-check di atas)
  radiusCircle = currentTemplate->getDefaultRadius();

  // Reset dots cache agar di-rebuild
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::update() {
  // Update sequential drawing logic
  if (sequentialMode) {
    updateSequentialDrawing();
  }

  // Update staggered parallel load (CTRL+O)
  if (isStaggeredLoad) {
    switch (loadStage) {
      case LOAD_TEMPLATE:
        if (isSequentialShapeLoad) {
          // SEQUENTIAL MODE (CTRL+SHIFT+O): Update satu per satu
          if (currentTemplateIndex < templateShapes.size()) {
            templateShapes[currentTemplateIndex]->update();

            if (templateShapes[currentTemplateIndex]->isComplete()) {
              currentTemplateIndex++;
            }
          } else {
            loadStage = LOAD_CUSTOMLINES;
            currentTemplateIndex = 0;
          }
        } else {
          // PARALLEL MODE (CTRL+O): Update semua bareng
          for (auto &shape : templateShapes) {
            shape->update();
          }

          bool allComplete = true;
          for (auto &shape : templateShapes) {
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
        break;

      case LOAD_CUSTOMLINES:
        // Untuk sequential load: updateSequentialLoad akan menambah customLines bertahap
        fileManager.updateSequentialLoad(customLines, polygonShapes);

        // Update progress semua customLines yang sudah ada
        for (auto &line : customLines) {
          line.updateProgress();
        }

        // Cek apakah semua customLines sudah complete ATAU sequential load sudah selesai
        {
          bool allComplete = true;
          bool allLoaded = (fileManager.getCurrentLoadIndex() >= fileManager.getTotalLoadedLines());

          for (const auto &line : customLines) {
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
        break;

      case LOAD_POLYGONS:
        // Untuk sequential load: updateSequentialLoad akan menambah polygons bertahap
        fileManager.updateSequentialLoad(customLines, polygonShapes);

        // Update animation semua polygons yang sudah ada
        for (auto &polygon : polygonShapes) {
          polygon.update();
        }

        // Cek apakah semua polygons sudah complete DAN sequential load sudah selesai
        {
          bool allComplete = true;
          bool allLoaded = !fileManager.isLoadSequentialMode();  // Sequential mode selesai

          for (const auto &polygon : polygonShapes) {
            if (!polygon.isAnimationComplete()) {
              allComplete = false;
              break;
            }
          }

          // Selesai staggered load jika semua complete DAN semua sudah di-load
          if (allComplete && allLoaded) {
            loadStage = LOAD_DONE;
            isStaggeredLoad = false;
            isSequentialShapeLoad = false;
            // Matikan sequential mode di FileManager supaya CTRL+SHIFT+DEL bisa dipakai
            fileManager.cancelSequentialLoad();  // Reset semua flag sequential
          }
        }
        break;

      case LOAD_DONE:
        // Selesai, tidak ada update khusus
        break;
    }
  } else {
    // Update normal (bukan staggered load)
    // Update semua template shapes (untuk animasi progress)
    for (auto &shape : templateShapes) {
      shape->update();
    }

    // Update sequential load logic dari FileManager (lines + polygons)
    // TAPI JANGAN panggil kalau sedang staggered load!
    if (!isStaggeredLoad) {
      fileManager.updateSequentialLoad(customLines, polygonShapes);
    }

    // Update polygon animations (untuk parallel mode)
    // TAPI jangan kalau sedang staggered load!
    if (fileManager.isLoadParallelMode() && !isStaggeredLoad) {
      for (auto &polygon : polygonShapes) {
        polygon.update();
      }
    }
  }

  // Update Ultralight UI renderers
  // SEKARANG AMAN: Semua UltralightManager instances share 1 Renderer
  // melalui SharedRendererManager singleton. Tidak akan ada WebCore.dll
  // corruption!
  if (appState == SETUP_MODE || appState == SETUP_TEMPLATE) {
    dialogUI.update();
  }
  if (appState == RUNNING) {
    sacredGeoUI.update();
  }

  // Bind JavaScript functions setelah delay (untuk tunggu HTML load complete)
  if (framesUntilBindJS > 0) {
    framesUntilBindJS--;
    if (framesUntilBindJS == 0) {
      dialogUI.bindJSFunctions();
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  // Render berdasarkan application state
  if (appState == SETUP_MODE || appState == SETUP_TEMPLATE) {
    // Background hitam solid
    ofSetColor(0);
    ofFill();
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    // Draw dialog centered
    ofPushStyle();
    ofSetColor(255, 255);

    // Calculate centered position
    int dialogX = (ofGetWidth() - 600) / 2;
    int dialogY = (ofGetHeight() - 400) / 2;

    ofPushMatrix();
    ofTranslate(dialogX, dialogY);
    dialogUI.draw();
    ofPopMatrix();

    ofPopStyle();
  } else { // RUNNING state
    // Trail effect untuk geometry
    ofSetColor(255, 25);
    ofFill();
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    // Push matrix untuk geometry drawing (centered)
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

    // Draw semua template shapes (polymorphic iteration)
    for (auto &shape : templateShapes) {
      if (shape) {
        shape->draw();
      }
    }

    // Draw custom lines dan UI elements
    drawCustomLinesAndUI();

    // Reset transform
    ofPopMatrix();

    // Draw Ultralight UI dengan style terpisah (tidak ganggu trail effect)
    if (sacredGeoUIVisible) {
      ofPushStyle();
      ofSetColor(255, 255); // Override trail effect alpha
      sacredGeoUI.draw();
      ofPopStyle(); // Kembalikan ke trail effect (255, 25) untuk frame
                    // berikutnya
    }
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
    ofSetColor(0, 0, 255, 150);     // Biru transparan
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
  if (drawState != DRAGGING && dotsVisible) {
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
    toggleLabels();
  }

  // Toggle dot visibility dengan . atau >
  if (key == '.' || key == '>') {
    toggleDots();
  }

  // Toggle Ultralight UI visibility dengan G atau g (hanya tanpa CTRL)
  if ((key == 'G' || key == 'g') && !isCtrlPressed) {
    toggleUltralightUI();
  }

  if (key == OF_KEY_DEL) {
    bool isShiftPressed = ofGetKeyPressed(OF_KEY_SHIFT);

    if (isCtrlPressed && isShiftPressed) {
      // Jangan hapus jika sedang sequential load
      if (fileManager.isLoadSequentialMode()) {
        return; // Aborted, sedang loading
      }

      // CTRL+SHIFT+DEL: Hapus semua polygon, custom lines, dan hide template shapes
      if (!polygonShapes.empty()) {
        polygonShapes.clear();
        selectedPolygonIndex = -1;
      }
      FileManager::clearCustomLines(customLines);
      selectedLineIndices.clear();
      lastSelectedLineIndex = -1;

      // Hide semua template shapes (BUKAN clear, supaya CTRL+)/CTRL+! bisa jalan lagi)
      hideAllShapes();

      return; // Jangan lanjut ke logic DEL biasa
    } else if (isCtrlPressed) {
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

      // CTRL+DEL: Hapus semua polygon dan semua custom lines
      if (!polygonShapes.empty()) {
        polygonShapes.clear();
        selectedPolygonIndex = -1;
      }
      if (!fileManager.isLoadParallelMode()) {
        FileManager::clearCustomLines(customLines);
      }
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
      // Hapus polygon yang selected
      polygonShapes.erase(polygonShapes.begin() + selectedPolygonIndex);
      selectedPolygonIndex = -1;
    }
    // Kalau tidak ada polygon selected, cek customLine
    else if (!selectedLineIndices.empty()) {
      // Hapus SEMUA garis yang terselect
      // Sort descending agar aman untuk erase
      vector<int> toDelete(selectedLineIndices.begin(),
                           selectedLineIndices.end());
      std::sort(toDelete.rbegin(), toDelete.rend()); // Descending
      for (int index : toDelete) {
        customLines.erase(customLines.begin() + index);
      }
      selectedLineIndices.clear();
    }
    // Kalau tidak ada polygon dan customLine terselect, toggle CartesianAxes
    // (shape pertama di template)
    else {
      if (!templateShapes.empty()) {
        AbstractShape *cartesianAxes =
            templateShapes[0].get(); // CartesianAxes selalu index 0
        if (cartesianAxes->showing) {
          cartesianAxes->hide();
        } else {
          cartesianAxes->show();
        }
      }
    }
  }

  if (key == ')' && ofGetKeyPressed(OF_KEY_SHIFT)) {
    // Hanya boleh show semua jika TIDAK sedang sequential drawing
    if (!sequentialMode) {
      showAllShapes();
    }
  }

  // FIX: Juga handle SHIFT+) yang terbaca sebagai '+' atau '='
  if ((key == '+' || key == '=') && ofGetKeyPressed(OF_KEY_SHIFT)) {
    // Hanya boleh show semua jika TIDAK sedang sequential drawing
    if (!sequentialMode) {
      showAllShapes();
      return; // Return agar tidak trigger increaseLineWidth
    }
  }

  if (key == '-' || key == '_') {
    decreaseLineWidth();
  }

  if (key == '+' || key == '=') {
    increaseLineWidth();
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
      undoLastLine();
      break;

    case 's':
    case 'S':
    case 19: // CTRL+S (ASCII 19)
      if (currentTemplate) {
        // Save dengan additional state (line width, labels, dots)
        fileManager.saveAll(currentTemplate->getName(), radiusCircle,
                            customLines, polygonShapes, currentLineWidth,
                            labelsVisible, dotsVisible);
      }
      break;

    case 'o':
    case 'O':
    case 15: // CTRL+O (ASCII 15)
      // Cek apakah SHIFT juga ditekan
      if (ofGetKeyPressed(OF_KEY_SHIFT)) {
        // Sequential load dengan animasi
        string loadedTemplateName;
        float loadedRadius;
        float loadedLineWidth;
        bool loadedLabelsVisible;
        bool loadedDotsVisible;

        fileManager.loadAllSequential(loadedTemplateName, loadedRadius,
                                      loadedLineWidth, loadedLabelsVisible, loadedDotsVisible,
                                      customLines, polygonShapes);

        // Clear customLines dan polygons yang sudah ada sebelumnya
        customLines.clear();
        selectedLineIndices.clear();
        lastSelectedLineIndex = -1;
        polygonShapes.clear();
        selectedPolygonIndex = -1;

        // Switch ke template yang di-load
        switchTemplate(loadedTemplateName);
        radiusCircle = loadedRadius;

        // Update Settings
        currentLineWidth = loadedLineWidth;
        labelsVisible = loadedLabelsVisible;
        dotsVisible = loadedDotsVisible;

        // Apply settings ke semua template shapes
        for (auto &shape : templateShapes) {
          shape->setLineWidth(currentLineWidth);
          if (labelsVisible)
            shape->showLabel();
          else
            shape->hideLabel();
          if (dotsVisible)
            shape->showDot();
          else
            shape->hideDot();
        }

        showAllShapes();  // Reset animasi template

        // Set sequential mode SETELAH showAllShapes (karena showAllShapes akan reset ke false)
        for (auto &shape : templateShapes) {
          shape->setSequentialMode(true);
        }

        fileManager.setLoadParallelMode(false);
        loadStage = LOAD_TEMPLATE;
        isStaggeredLoad = true;
        isSequentialShapeLoad = true;  // Sequential per shape
        currentTemplateIndex = 0;  // Reset index template
      } else {
        // Load workspace dengan template name, radius, dan ALL settings
        string loadedTemplateName;
        float loadedRadius;
        float loadedLineWidth;
        bool loadedLabelsVisible;
        bool loadedDotsVisible;

        if (fileManager.loadAll(loadedTemplateName, loadedRadius, customLines,
                                polygonShapes, loadedLineWidth,
                                loadedLabelsVisible, loadedDotsVisible)) {

          // Switch ke template yang di-load
          switchTemplate(loadedTemplateName);

          // Update radius dengan loaded radius
          radiusCircle = loadedRadius;

          // Update Settings
          currentLineWidth = loadedLineWidth;
          labelsVisible = loadedLabelsVisible;
          dotsVisible = loadedDotsVisible;

          // Apply settings ke semua shapes yang baru di-load
          for (auto &shape : templateShapes) {
            shape->setLineWidth(currentLineWidth);

            if (labelsVisible)
              shape->showLabel();
            else
              shape->hideLabel();

            if (dotsVisible)
              shape->showDot();
            else
              shape->hideDot();

            shape->setSequentialMode(false);  // PARALLEL mode (CTRL+O)
          }

          // Mulai staggered parallel load
          showAllShapes();  // Ini akan memulai animasi dari awal

          // Matikan parallel mode dulu supaya customLines tidak langsung di-animate
          fileManager.setLoadParallelMode(false);

          loadStage = LOAD_TEMPLATE;
          isStaggeredLoad = true;
          isSequentialShapeLoad = false;  // PARALLEL mode (CTRL+O)
        }
      }
      break;

    case 'g':
    case 'G':
    case 7: // CTRL+G (ASCII 7)
      createInvisiblePolygonFromSelected();
      break;
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
  for (auto &shape : templateShapes) {
    if (shape && shape->showing) {
      shape->addDotsToCache(cachedDots);
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
void ofApp::undoLastLine() {
  // Hapus garis terakhir yang dibuat user
  if (!customLines.empty()) {
    customLines.pop_back();
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
      // Ambil sampled points (100 segments) untuk ikuti curve
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

  // 2. Create polygon dengan default color MERAH TRANSPARENT dan index
  int polygonIndex =
      static_cast<int>(polygonShapes.size()); // Index polygon yang akan dibuat
  PolygonShape newPolygon(allPoints, ofColor(255, 0, 0, 150),
                          polygonIndex); // Alpha 150 (semi-transparent)
  polygonShapes.push_back(newPolygon);

  // 3. Clear selection
  selectedLineIndices.clear();
  lastSelectedLineIndex = -1;
}

//--------------------------------------------------------------
// File operations sekarang ditangani oleh FileManager class

//--------------------------------------------------------------
// Mouse Event Handlers
void ofApp::mousePressed(int x, int y, int button) {
  // Handle dialog button clicks (Dialog active)
  if (appState == SETUP_MODE || appState == SETUP_TEMPLATE) {
    // Forward mouse event ke dialogUI untuk JavaScript bridge
    int dialogX = (ofGetWidth() - 600) / 2;
    int dialogY = (ofGetHeight() - 400) / 2;
    int relX = x - dialogX;
    int relY = y - dialogY;

    if (relX >= 0 && relX < 600 && relY >= 0 && relY < 400) {
      dialogUI.fireMouseDown(relX, relY, button);
    }

    return; // Jangan lanjut ke logic lain saat dialog aktif
  }

  // Forward mouse press ke Ultralight UI jika mouse di area UI dan UI visible
  if (sacredGeoUIVisible && sacredGeoUI.isMouseOverUI(x, y)) {
    sacredGeoUI.fireMouseDown(x, y, button);
    return; // Jangan lanjut ke logic lain
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
  // Handle dialog mouse release
  if (appState == SETUP_MODE || appState == SETUP_TEMPLATE) {
    int dialogX = (ofGetWidth() - 600) / 2;
    int dialogY = (ofGetHeight() - 400) / 2;
    int relX = x - dialogX;
    int relY = y - dialogY;

    if (relX >= 0 && relX < 600 && relY >= 0 && relY < 400) {
      dialogUI.fireMouseUp(relX, relY, button);
    }
    return;
  }

  // Forward mouse release ke Ultralight UI jika mouse di area UI dan UI visible
  if (sacredGeoUIVisible && sacredGeoUI.isMouseOverUI(x, y)) {
    sacredGeoUI.fireMouseUp(x, y, button);
    return; // Jangan lanjut ke logic lain
  }

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
          CustomLine newLine(currentPolylinePoints, ofColor(0, 0, 255),
                             mouseLineWidth);
          customLines.push_back(newLine);
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
  // Forward scroll ke Ultralight UI jika mouse di area UI dan UI visible
  if (sacredGeoUIVisible && sacredGeoUI.isMouseOverUI(x, y)) {
    sacredGeoUI.fireMouseScroll(x, y, scrollX,
                                -scrollY * 30); // Ultralight pakai pixel delta
    return;                                     // Jangan lanjut ke logic lain
  }

  // Update curve untuk SEMUA garis yang selected
  if (!selectedLineIndices.empty()) {
    float curveSpeed = 5.0f; // Kecepatan perubahan curve
    for (int lineIndex : selectedLineIndices) {
      if (lineIndex >= 0 && lineIndex < customLines.size()) {
        float newCurve =
            customLines[lineIndex].getCurve() + scrollY * curveSpeed;
        customLines[lineIndex].setCurve(newCurve);
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
  // Update mouse position untuk hover detection (adjust untuk center
  // translation)
  mousePos = vec2(x - ofGetWidth() / 2, y - ofGetHeight() / 2);

  // Handle dialog mouse move
  if (appState == SETUP_MODE || appState == SETUP_TEMPLATE) {
    int dialogX = (ofGetWidth() - 600) / 2;
    int dialogY = (ofGetHeight() - 400) / 2;
    int relX = x - dialogX;
    int relY = y - dialogY;

    if (relX >= 0 && relX < 600 && relY >= 0 && relY < 400) {
      dialogUI.fireMouseMove(relX, relY);
    }
  }

  // Forward mouse move ke Ultralight UI jika mouse di area UI dan UI visible
  if (sacredGeoUIVisible && sacredGeoUI.isMouseOverUI(x, y)) {
    sacredGeoUI.fireMouseMove(x, y);
  }
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
  // Cek apakah semua shapes sudah visible/showing
  bool allVisible = true;
  for (auto &shape : templateShapes) {
    if (!shape->showing) {
      allVisible = false;
      break;
    }
  }

  if (allVisible) {
    // Semua shapes sudah visible, jangan jalankan sequential
    return;
  }

  // Cek apakah ada shape yang sedang drawing (belum complete)
  bool stillDrawing = false;
  for (auto &shape : templateShapes) {
    if (!shape->isComplete()) {
      stillDrawing = true;
      break;
    }
  }

  if (stillDrawing) {
    // Ada shape yang masih drawing, jangan jalankan sequential
    return;
  }

  // Cek apakah sequential sudah pernah selesai
  if (sequentialCompleted) {
    // Sequential sudah selesai sebelumnya, jangan jalankan lagi
    return;
  }

  // Reset semua shapes ke hidden
  for (auto &shape : templateShapes) {
    shape->hide();
  }

  // Mulai sequential mode
  sequentialMode = true;
  sequentialCompleted = false; // Reset flag
  currentShapeIndex = 0;

  // Set sequential mode untuk semua shapes (setelah sequentialMode = true)
  for (auto &shape : templateShapes) {
    shape->setSequentialMode(sequentialMode);
  }

  // Show shape pertama
  if (!templateShapes.empty()) {
    templateShapes[0]->show();
  }
}

//--------------------------------------------------------------
void ofApp::updateSequentialDrawing() {
  AbstractShape *currentShape = nullptr;

  // Tentukan shape berdasarkan index (dari templateShapes)
  if (currentShapeIndex >= 0 && currentShapeIndex < templateShapes.size()) {
    currentShape = templateShapes[currentShapeIndex].get();
  }

  // Cek jika current shape sudah complete
  if (currentShape && currentShape->isComplete()) {
    // Pindah ke shape berikutnya
    currentShapeIndex++;

    // Show shape berikutnya jika masih ada
    if (currentShapeIndex < templateShapes.size()) {
      templateShapes[currentShapeIndex]->show();
    } else {
      // Semua shapes sudah complete, matikan sequential mode dan tandai selesai
      sequentialMode = false;
      sequentialCompleted = true;
      dotsCacheDirty = true; // Rebuild dots cache agar mouse hover bekerja
    }
  }
}

//--------------------------------------------------------------
void ofApp::toggleLabels() {
  // Toggle label visibility
  labelsVisible = !labelsVisible;

  // Iterate semua template shapes
  for (auto &shape : templateShapes) {
    if (labelsVisible) {
      shape->showLabel();
    } else {
      shape->hideLabel();
    }
  }
}

//--------------------------------------------------------------
void ofApp::toggleDots() {
  // Toggle dot visibility
  dotsVisible = !dotsVisible;

  // Iterate semua template shapes
  for (auto &shape : templateShapes) {
    if (dotsVisible) {
      shape->showDot();
    } else {
      shape->hideDot();
    }
  }
}

//--------------------------------------------------------------
void ofApp::hideAllShapes() {
  // Matikan sequential mode
  sequentialMode = false;

  // Set paralel mode untuk semua shapes (pakai sequentialMode yang sudah false)
  for (auto &shape : templateShapes) {
    shape->setSequentialMode(sequentialMode);
  }

  // Hide semua shapes
  for (auto &shape : templateShapes) {
    shape->hide();
  }

  // Reset sequential completed flag agar bisa jalankan lagi
  sequentialCompleted = false;

  // Mark dots cache dirty karena visibility berubah
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::showAllShapes() {
  // Matikan sequential mode (paralel mode)
  sequentialMode = false;

  // Set mode untuk semua shapes (pakai sequentialMode yang sudah false)
  for (auto &shape : templateShapes) {
    shape->setSequentialMode(sequentialMode);
  }

  // Show semua shapes (akan animasi barengan/paralel)
  for (auto &shape : templateShapes) {
    shape->show();
  }

  // Reset sequential completed flag agar bisa jalankan lagi
  sequentialCompleted = false;

  // Mark dots cache dirty karena visibility berubah
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::decreaseLineWidth() {
  // Kurangi line width secara bertahap
  currentLineWidth -= 0.5f;

  // Batasi minimum line width
  if (currentLineWidth < 0.f) {
    currentLineWidth = 0.f;
  }

  // Set line width ke semua shapes
  for (auto &shape : templateShapes) {
    shape->setLineWidth(currentLineWidth);
  }
}

//--------------------------------------------------------------
void ofApp::increaseLineWidth() {
  // Tambah line width secara bertahap
  currentLineWidth += 0.5f;

  // Batasi maximum line width
  if (currentLineWidth > 4.0f) {
    currentLineWidth = 4.0f;
  }

  // Set line width ke semua shapes
  for (auto &shape : templateShapes) {
    shape->setLineWidth(currentLineWidth);
  }
}

//--------------------------------------------------------------
void ofApp::toggleUltralightUI() { sacredGeoUIVisible = !sacredGeoUIVisible; }

//--------------------------------------------------------------
void ofApp::setupDialogUI() {
  // Set JavaScript callback dulu SEBELUM setup supaya ViewListener terpasang
  dialogUI.setJSCallback(
      [this](const std::string &action) { this->handleJSAction(action); });

  dialogUI.setup(600, 400);
}

//--------------------------------------------------------------
void ofApp::handleJSAction(const std::string &action) {
  if (action == "onDialogClose") {
    onDialogClose();
  } else if (action == "onNext") {
    showTemplateDialog();
  } else if (action == "onBack") {
    showModeDialog();
  } else if (action == "on2DMode") {
    selectedMode = "2D";
    showTemplateDialog("2D");
  } else if (action == "on3DMode") {
    selectedMode = "3D";
    showTemplateDialog("3D");
  } else if (action == "onCreate") {
    onCreateApp();
  }
}

//--------------------------------------------------------------
void ofApp::showModeDialog() {
  dialogUI.loadHTMLFile("html/dialog_mode.html");
  appState = SETUP_MODE;
  framesUntilBindJS = 10; // Bind JS functions setelah 10 frame
}

//--------------------------------------------------------------
void ofApp::showTemplateDialog(const std::string& mode) {
  // Load dialog template yang berbeda tergantung mode (2D vs 3D)
  if (mode == "3D") {
    dialogUI.loadHTMLFile("html/dialog_template_3d.html");
  } else {
    dialogUI.loadHTMLFile("html/dialog_template_2d.html");
  }
  appState = SETUP_TEMPLATE;
  framesUntilBindJS = 10; // Bind JS functions setelah 10 frame
}

//--------------------------------------------------------------
void ofApp::onCreateApp() {
  sacredGeoUI.setup(400, ofGetHeight() - 50);
  sacredGeoUI.loadHTMLFile("html/ui.html");
  appState = RUNNING;
}

//--------------------------------------------------------------
void ofApp::onDialogClose() { ofExit(); }

//--------------------------------------------------------------
void ofApp::exit() {
  // STEP 1: Cleanup setiap UltralightManager instance
  // Ini akan cleanup view dan viewListener untuk masing-masing instance
  dialogUI.cleanup();
  sacredGeoUI.cleanup();

  // STEP 2: Cleanup shared renderer (paling terakhir!)
  // SharedRendererManager cleanup dipanggil SETELAH semua instances cleanup
  // untuk memastikan tidak ada view yang mencoba akses renderer
  SharedRendererManager::cleanup();
}
