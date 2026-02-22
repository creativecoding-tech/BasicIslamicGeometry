#include "FileOperationManager.h"
#include "../ofApp.h"
#include "../template/templates/BasicZelligeTemplate.h"
#include "gui/Playground.h"

//--------------------------------------------------------------
FileOperationManager::FileOperationManager(ofApp *app) : app(app) {}

//--------------------------------------------------------------
bool FileOperationManager::hasTessellatedGeometry() const {
  for (const auto &line : app->customLines) {
    if (line.isTessellated())
      return true;
  }
  for (const auto &poly : app->polygonShapes) {
    if (poly.isTessellated())
      return true;
  }
  return false;
}

//--------------------------------------------------------------
void FileOperationManager::saveWorkspace() {
  if (!app->currentTemplate) {
    return;
  }

  // Kalau belum pernah Save As, langsung buka dialog (Save As)
  if (app->lastSavedPath.empty()) {
    saveWorkspaceAs();
    return;
  }

  if (hasTessellatedGeometry()) {
    app->saveConfirmationPopup->show(false); // isSaveAs = false
    return;
  }

  executeSaveWorkspace(false);
}

//--------------------------------------------------------------
void FileOperationManager::executeSaveWorkspace(bool filterTessellation) {
  std::vector<CustomLine> linesToSave = app->customLines;
  std::vector<PolygonShape> polysToSave = app->polygonShapes;

  if (filterTessellation) {
    linesToSave.erase(
        std::remove_if(linesToSave.begin(), linesToSave.end(),
                       [](const CustomLine &l) { return l.isTessellated(); }),
        linesToSave.end());

    polysToSave.erase(
        std::remove_if(polysToSave.begin(), polysToSave.end(),
                       [](const PolygonShape &p) { return p.isTessellated(); }),
        polysToSave.end());
  }

  // Sudah ada Save As sebelumnya, save langsung ke file tersebut
  app->fileManager.saveAll(
      app->currentTemplate->getName(), app->currentTemplate->radius,
      linesToSave, polysToSave, app->userDots, app->currentTemplate->lineWidth,
      app->currentTemplate->labelsVisible, app->currentTemplate->dotsVisible,
      app->showUserDot, app->lastSavedPath);

  // Update info jumlah customLines dan polygons (untuk CollapsingHeader)
  app->loadedFileCustomLinesCount = static_cast<int>(app->customLines.size());
  app->loadedFilePolygonCount = static_cast<int>(app->polygonShapes.size());

  // Show MenuBar agar popup terlihat
  app->imguiVisible = true;
  app->successPopup->show();

  // Enable playback settings (karena sekarang sudah ada file yang disave)
  BasicZelligeTemplate *zelligeTemplate =
      dynamic_cast<BasicZelligeTemplate *>(app->currentTemplate);
  if (zelligeTemplate) {
    zelligeTemplate->enablePlaybackSettings();
  }
}

//--------------------------------------------------------------
void FileOperationManager::saveWorkspaceAs() {
  if (!app->currentTemplate) {
    return; // Tidak ada template aktif
  }

  if (hasTessellatedGeometry()) {
    app->saveConfirmationPopup->show(true); // isSaveAs = true
    return;
  }

  executeSaveWorkspaceAs(false);
}

//--------------------------------------------------------------
void FileOperationManager::executeSaveWorkspaceAs(bool filterTessellation) {
  // Buka save dialog untuk pilih lokasi dan nama file
  ofFileDialogResult saveDialog =
      ofSystemSaveDialog("workspace.nay", "Save Workspace As");

  // Cek apakah user memilih file (tidak cancel)
  if (saveDialog.getPath().empty()) {
    return; // User cancel
  }

  // Ambil filepath dari dialog
  std::string filepath = saveDialog.getPath();

  // Tambah extension .nay kalau belum ada
  if (filepath.find(".nay") == std::string::npos) {
    filepath += ".nay";
  }

  std::vector<CustomLine> linesToSave = app->customLines;
  std::vector<PolygonShape> polysToSave = app->polygonShapes;

  if (filterTessellation) {
    linesToSave.erase(
        std::remove_if(linesToSave.begin(), linesToSave.end(),
                       [](const CustomLine &l) { return l.isTessellated(); }),
        linesToSave.end());

    polysToSave.erase(
        std::remove_if(polysToSave.begin(), polysToSave.end(),
                       [](const PolygonShape &p) { return p.isTessellated(); }),
        polysToSave.end());
  }

  // Simpan langsung ke filepath yang user pilih
  app->fileManager.saveAll(
      app->currentTemplate->getName(), app->currentTemplate->radius,
      linesToSave, polysToSave, app->userDots, app->currentTemplate->lineWidth,
      app->currentTemplate->labelsVisible, app->currentTemplate->dotsVisible,
      app->showUserDot, filepath);

  // Simpan path ini sebagai lastSavedPath (CTRL+S selanjutnya akan kesini)
  app->lastSavedPath = filepath;

  // Update info jumlah customLines dan polygons (untuk CollapsingHeader)
  app->loadedFileCustomLinesCount = static_cast<int>(app->customLines.size());
  app->loadedFilePolygonCount = static_cast<int>(app->polygonShapes.size());

  // Show MenuBar agar popup terlihat
  app->imguiVisible = true;
  app->successPopup->show();

  // Enable playback settings (karena sekarang sudah ada file yang disave)
  BasicZelligeTemplate *zelligeTemplate =
      dynamic_cast<BasicZelligeTemplate *>(app->currentTemplate);
  if (zelligeTemplate) {
    zelligeTemplate->enablePlaybackSettings();
  }
}

//--------------------------------------------------------------
void FileOperationManager::openWorkspace() {
  // Buka open file dialog
  ofFileDialogResult openDialog = ofSystemLoadDialog("Open Workspace", false);

  // Cek apakah user memilih file (tidak cancel)
  if (openDialog.getPath().empty()) {
    return; // User cancel
  }

  // Ambil filepath dari dialog
  std::string filepath = openDialog.getPath();

  // Cek apakah file extension .nay
  if (filepath.find(".nay") == std::string::npos) {
    // File bukan format .nay, tampilkan error popup
    // Show MenuBar agar popup terlihat
    app->imguiVisible = true;
    app->showSacredGeometry = false;
    app->showPlayground = false;
    app->errorPopup->show("Invalid File Format",
                          "Please select a .nay file format!", "OK");
    return;
  }

  // Set lastSavedPath ke filepath yang di-open (untuk load dan save)
  app->lastSavedPath = filepath;

  // Cek jumlah customLines di file (untuk CollapsingHeader di playground)
  peekFileCustomLinesCount(filepath, app->loadedFileCustomLinesCount);

  // Cek jumlah polygons di file (untuk CollapsingHeader di playground)
  peekFilePolygonCount(filepath, app->loadedFilePolygonCount);

  // Enable playback settings (show playback UI di Playground)
  BasicZelligeTemplate *zelligeTemplate =
      dynamic_cast<BasicZelligeTemplate *>(app->currentTemplate);
  if (zelligeTemplate) {
    zelligeTemplate->enablePlaybackSettings();
  }

  // Show dan focus Playground window (file valid, akan diload)
  app->imguiVisible = true;
  app->showPlayground = true;

  // Set windowOpen flag dan focus ke Playground
  for (auto &gui : app->guiComponents) {
    Playground *playground = dynamic_cast<Playground *>(gui.get());
    if (playground) {
      playground->showWindow();
    }
  }
}

//--------------------------------------------------------------
void FileOperationManager::loadWorkspace() {
  // NOTE: Validasi canvas sudah dilakukan di UI level (Playground)
  // Jangan cek lagi di sini agar autoCleanCanvas bisa berfungsi

  // Cek apakah ada file yang di-open (lastSavedPath)
  if (app->lastSavedPath.empty())
    return;

  // Load workspace dengan template name, radius, dan ALL settings
  std::string loadedTemplateName;
  float loadedRadius;
  float loadedLineWidth;
  bool loadedLabelsVisible;
  bool loadedDotsVisible;

  // Set flag untuk customLines loading (untuk konsistency, meskipun parallel
  // load tidak pakai buffer)
  app->fileManager.setShouldLoadCustomLines(app->shouldDrawCustomLines);

  if (app->fileManager.loadAll(loadedTemplateName, loadedRadius,
                               app->customLines, app->polygonShapes,
                               app->userDots, loadedLineWidth,
                               loadedLabelsVisible, loadedDotsVisible,
                               app->showUserDot, app->lastSavedPath)) {

    // Switch ke template yang di-load DULU
    app->switchTemplate(loadedTemplateName);

    // Rebuild shapes dengan loaded radius yang benar!
    app->currentTemplate->setRadius(loadedRadius);

    // Update tracking untuk scaling
    app->previousRadius =
        loadedRadius; // Reset tracking agar tidak scaling saat load

    // Sync Settings ke template
    app->currentTemplate->lineWidth = loadedLineWidth;
    app->currentTemplate->labelsVisible = loadedLabelsVisible;
    app->currentTemplate->dotsVisible = loadedDotsVisible;

    // Apply settings ke semua shapes yang baru di-load
    const auto &shapes = app->currentTemplate->getShapes();
    for (auto &shape : shapes) {
      shape->setRadius(
          app->currentTemplate->radius); // Update radius dengan loadedRadius!
      shape->setLineWidth(app->currentTemplate->lineWidth);

      if (app->currentTemplate->labelsVisible)
        shape->showLabel();
      else
        shape->hideLabel();

      if (app->currentTemplate->dotsVisible)
        shape->showDot();
      else
        shape->hideDot();
    }

    // Reset dots cache agar di-rebuild
    app->dotsCacheDirty = true;

    // Reset canvas transform
    app->canvasTranslation = vec2(0, 0);
    app->canvasRotation = 0.0f;
    app->canvasZoom = 1.0f;

    // Cek flag Draw Custom Lines - jika false, clear customLines yang baru
    // diload
    if (!app->shouldDrawCustomLines) {
      app->customLines.clear();
      app->selectionManager.clearLineSelection();
    }

    // Matikan parallel dulu supaya customLines tidak langsung di-animate
    app->fileManager.setLoadParallelMode(false);

    // Sync ColorPicker dengan warna customLines yang diload
    app->syncColorPickerFromLoadedLines();
    app->syncColorPickerFromLoadedPolygons();
    app->syncUserDotFromLoaded();

    app->loadStage = ofApp::LOAD_TEMPLATE;
    app->isStaggeredLoad = true;
    app->isSequentialShapeLoad = false; // PARALLEL mode (CTRL+O)
    app->currentState =
        ofApp::UpdateState::STAGGERED_LOAD; // STRATEGY PATTERN: Set state ke
                                            // STAGGERED_LOAD
  } else {
    app->errorPopup->show("Failed to Load Workspace",
                          "Invalid file format or corrupted data!", "OK");
  }
}

//--------------------------------------------------------------
void FileOperationManager::loadWorkspaceSeq() {
  // NOTE: Validasi canvas sudah dilakukan di UI level (Playground)
  // Jangan cek lagi di sini agar autoCleanCanvas bisa berfungsi

  // Cek apakah ada file yang di-open (lastSavedPath)
  if (app->lastSavedPath.empty())
    return;

  // Load workspace sequential dengan template name, radius, dan ALL settings
  std::string loadedTemplateName;
  float loadedRadius;
  float loadedLineWidth;
  bool loadedLabelsVisible;
  bool loadedDotsVisible;

  // Set flag untuk customLines loading SEBELUM load
  app->fileManager.setShouldLoadCustomLines(app->shouldDrawCustomLines);

  if (app->fileManager.loadAll(loadedTemplateName, loadedRadius,
                               app->customLines, app->polygonShapes,
                               app->userDots, loadedLineWidth,
                               loadedLabelsVisible, loadedDotsVisible,
                               app->showUserDot, app->lastSavedPath)) {

    // Switch ke template yang di-load DULU
    app->switchTemplate(loadedTemplateName);

    // Rebuild shapes dengan loaded radius yang benar!
    app->currentTemplate->setRadius(loadedRadius);

    // Update tracking untuk scaling
    app->previousRadius =
        loadedRadius; // Reset tracking agar tidak scaling saat load

    // Sync Settings ke template
    app->currentTemplate->lineWidth = loadedLineWidth;
    app->currentTemplate->labelsVisible = loadedLabelsVisible;
    app->currentTemplate->dotsVisible = loadedDotsVisible;

    // Apply settings ke semua shapes yang baru di-load
    const auto &shapes = app->currentTemplate->getShapes();
    for (auto &shape : shapes) {
      shape->setRadius(
          app->currentTemplate->radius); // Update radius dengan loadedRadius!
      shape->setLineWidth(app->currentTemplate->lineWidth);

      if (app->currentTemplate->labelsVisible)
        shape->showLabel();
      else
        shape->hideLabel();

      if (app->currentTemplate->dotsVisible)
        shape->showDot();
      else
        shape->hideDot();

      // Set sequential mode ke SEMUA shapes (penting untuk OctagramLine 2-phase
      // animation!)
      shape->setSequentialMode(true);
      // Reset progress untuk sequential drawing
      shape->progress = 0;
    }

    // Cek flag Draw Custom Lines - jika false, clear customLines yang baru
    // diload (Sama seperti loadWorkspace)
    if (!app->shouldDrawCustomLines) {
      app->customLines.clear();
      app->selectionManager.clearLineSelection();
    }

    // Enable parallel mode untuk customLines & polygons agar langsung ready
    // untuk di-draw/animate sesuai mode masing-masing
    app->fileManager.setLoadParallelMode(true);

    // Sync ColorPicker
    app->syncColorPickerFromLoadedLines();
    app->syncColorPickerFromLoadedPolygons();
    app->syncUserDotFromLoaded();

    // START SEQUENTIAL MODE untuk template shapes!
    // Set sequential mode flag di template
    app->currentTemplate->sequentialMode = true;
    app->currentTemplate->currentShapeIndex = 0;
    app->currentTemplate->sequentialCompleted = false;

    // Reset dots cache agar di-rebuild
    app->dotsCacheDirty = true;

    // Reset canvas transform
    app->canvasTranslation = vec2(0, 0);
    app->canvasRotation = 0.0f;
    app->canvasZoom = 1.0f;

    // Sequential load akan mengupdate template secara bertahap
    // TAPI customLines dan polygons sudah loaded semua (siap animate)
    app->loadStage = ofApp::LOAD_TEMPLATE;
    app->isStaggeredLoad = true;
    app->isSequentialShapeLoad = true; // SEQUENTIAL mode untuk Template
    app->currentState =
        ofApp::UpdateState::STAGGERED_LOAD; // STRATEGY PATTERN: Set state ke
                                            // STAGGERED_LOAD
  } else {
    app->errorPopup->show("Failed to Load Workspace",
                          "Invalid file format or corrupted data!", "OK");
  }
}

//--------------------------------------------------------------
bool FileOperationManager::peekFileCustomLinesCount(const std::string &filepath,
                                                    int &outCount) {
  // Baca file header saja untuk mendapatkan jumlah customLines
  ofFile file(filepath);
  if (!file.exists()) {
    outCount = 0;
    return false;
  }

  // Read file ke buffer
  ofBuffer buffer = ofBufferFromFile(filepath);
  char *data = buffer.getData();
  size_t bufferSize = buffer.size();

  if (bufferSize < 64) {
    outCount = 0;
    return false;
  }

  size_t offset = 0;

  // Validasi Magic Number
  if (memcmp(data + offset, "NA01", 4) != 0) {
    outCount = 0;
    return false;
  }
  offset += 4;

  // Skip Version (4 bytes)
  offset += sizeof(int);

  // Skip Template Name Length (4 bytes) + Template Name
  int nameLen = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int) + nameLen;

  // Skip Global Radius (4 bytes)
  offset += sizeof(float);

  // Skip Additional Settings (20 bytes untuk v1, atau float + 3 bool + 13 bytes
  // untuk v2)
  offset += 20;

  // Baca jumlah Custom Lines
  if (offset + sizeof(int) > bufferSize) {
    outCount = 0;
    return false;
  }

  int numLines = *reinterpret_cast<int *>(data + offset);
  outCount = numLines;

  return true;
}

//--------------------------------------------------------------
bool FileOperationManager::peekFilePolygonCount(const std::string &filepath,
                                                int &outCount) {
  // Cara MUDAH: Gunakan loadAllSequential untuk load polygons ke buffer, lalu
  // ambil count-nya saja Kita tidak akan memakai customLines yang diload, jadi
  // langsung clear saja

  std::string dummyTemplateName;
  float dummyRadius;
  float dummyLineWidth;
  bool dummyLabelsVisible;
  bool dummyDotsVisible;

  std::vector<CustomLine> dummyCustomLines;
  std::vector<PolygonShape> dummyPolygons;
  std::vector<std::unique_ptr<DotShape>> dummyUserDots;

  // Load ke buffer dulu (file manager akan load ke loadedLinesBuffer dan
  // loadedPolygonsBuffer)
  app->fileManager.loadAllSequential(
      dummyTemplateName, dummyRadius, dummyLineWidth, dummyLabelsVisible,
      dummyDotsVisible, dummyCustomLines, dummyPolygons, dummyUserDots,
      app->showUserDot, filepath);

  // Ambil jumlah polygons NON-TESSELATED dari buffer via getter
  outCount = app->fileManager.getTotalOriginalPolygons();

  // ⭐ NEW: Extract tessellation metadata HERE so the UI populates immediately
  // upon "Open" (before "Draw")
  if (app->currentTemplate) {
    BasicZelligeTemplate *zellige =
        dynamic_cast<BasicZelligeTemplate *>(app->currentTemplate);
    if (zellige) {
      zellige->tessellationFiles.clear();
      zellige->tessellationRadii.clear();
      const auto &bufferPolys = app->fileManager.getLoadedPolygonsBuffer();
      for (size_t i = 0; i < bufferPolys.size(); i++) {
        zellige->tessellationFiles.push_back(
            bufferPolys[i].getSourceTessellationFile());
        zellige->tessellationRadii.push_back(
            bufferPolys[i].getSourceTessellationRadius());
      }
    }
  }

  // Clear buffer agar tidak mempengaruhi load selanjutnya
  app->fileManager.cancelSequentialLoad();

  return outCount > 0; // Return true jika ada polygons
}

//--------------------------------------------------------------
void FileOperationManager::closeFile() {
  // Clear file reference TANPA clean canvas
  // Canvas content (shapes, customLines, polygons) tetap ada

  // Clear lastSavedPath
  app->lastSavedPath.clear();

  // Clear file count info
  app->loadedFileCustomLinesCount = 0;
  app->loadedFilePolygonCount = 0;

  // Reset draw mode ke belum memilih dan hide playback UI
  if (app->currentTemplate) {
    BasicZelligeTemplate *zelligeTemplate =
        dynamic_cast<BasicZelligeTemplate *>(app->currentTemplate);
    if (zelligeTemplate) {
      zelligeTemplate->resetDrawMode();
    }
  }

  // Reset speed multiplier ke default (1.0f)
  if (app->currentTemplate) {
    app->currentTemplate->speedMultiplier = 1.0f;
  }

  // Canvas TIDAK dibersihkan - user bisa terus bekerja dengan content yang ada
}
