#include "ofApp.h"
#include "operation/FileOperationManager.h"
#include "operation/gui/SacredGeometry.h"
#include "operation/gui/UserCustom.h"
#include "operation/gui/CanvasSettings.h"  // ⭐ NEW
#include "shape/AbstractShape.h"
#include "shape/DotShape.h"
#include "template/templates/BasicZelligeTemplate.h"
#include "utils/GeometryUtils.h"
#include <algorithm>
#include <vector>

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(false);
  ofSetFrameRate(60);
  ofSetEscapeQuitsApp(false);
  ofSetBackgroundAuto(false);
  ofEnableAntiAliasing();
  ofEnableSmoothing();
  ofDisableArbTex(); // Required for FBO texture sampling in FillAnimation

  // ⭐ NEW: Load custom line shader dengan geometry shader untuk thick lines
  customLineShader.load("shaders/line.vert", "shaders/line.frag", "shaders/line.geom");

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

  // Initialize TessellationManager (after ofApp is fully constructed)
  tessellationManager = std::make_unique<TessellationManager>();

  // define ImGUI
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
  SacredGeometryTemplate *oldTemplate = currentTemplate;
  currentTemplate = registry.getTemplate(templateName);

  // Null check - template tidak ditemukan
  if (!currentTemplate) {
    // Template tidak ditemukan, jangan lanjut
    return;
  }

  // Keep previous radius setting if available
  if (oldTemplate) {
    currentTemplate->radius =
        oldTemplate->radius; // Copy radius dari template lama
  }

  // Setup shapes SAH radius di-set (REQUIRED!)
  currentTemplate->setupShapes();

  // Reset dots cache agar di-rebuild
  dotsCacheDirty = true;
}

//--------------------------------------------------------------
void ofApp::update() {
  // ⭐ FIX: SKIP update selama force clear screen untuk pause animation
  bool isForceClearing = (forceClearScreenCounter > 0 || forceNoTrailsCounter > 0 || delayForceClear > 0);
  if (isForceClearing) {
    return; // Skip semua update selama force clear
  }

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
  for (auto &dot : userDots) {
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
      currentState = UpdateState::NORMAL; // Kembali ke normal setelah selesai
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
    // Apply selected draw mode saat mulai drawing (sebelum load)
    polygonDrawMode = nextPolygonDrawMode;
    currentLineStepAnimationMode = lineStepAnimationMode;

    // Clear all custom line animations at start
    for (auto &line : customLines) {
      line.setAnimation(nullptr);
    }

    // Timer selesai, panggil load method
    if (pendingLoadMode == 0) {
      fileOperationManager
          ->loadWorkspace(); // Parallel mode: Load All (but staggered)
    } else if (pendingLoadMode == 1) {
      fileOperationManager->loadWorkspaceSeq(); // Sequential mode: Load All
    }

    // ⭐ NEW: Apply speed multiplier after load (loading resets shapes to
    // default speed)
    if (currentTemplate) {
      currentTemplate->applySpeedMultiplier();
    }

    // Clear animation state
    waveAnimationApplied = false;
    waveAnimationTimer = 0.0f;
    currentCustomLineWaveIndex = 0;
    customLineWaveTimer = 0.0f;

    // Reset flag dan accumulator
    isWaitingForLoad = false;
    pendingLoadMode = -1;
    loadDelayAccumulator = 0.0f;
    isStaggeredLoad = true; // ⭐ NEW Enable visibility flags
    currentState = UpdateState::STAGGERED_LOAD; // Lanjut ke staggered load
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
    // Selesai drawing normal
    isStaggeredLoad = false;
    isSequentialShapeLoad = false;

    // ⭐ TESSELLATION LOGIC: Cek apakah perlu tessellation
    if (isTessellationEnabled && tessellationMode == 0) { // 0 = Post-Draw

      // ⭐ PRE-TESSELLATION PAUSE: Tunggu sebentar sebelum tessellation
      if (preTessellationPause > 0.0f && preTessellationTimer < preTessellationPause) {
        // Ada pause, gunakan delay system
        preTessellationTimer += ofGetLastFrameTime();

        if (preTessellationTimer >= preTessellationPause) {
          // Pause selesai, lanjut tessellation
          preTessellationTimer = 0.0f; // Reset timer untuk tessellation drawing
        } else {
          // Masih pause, jangan lanjut tessellation dulu
          return; // Keluar dari LOAD_DONE, nanti frame berikutnya masuk lagi
        }
      }

      // ⭐ Simpan original radius SEKARANG (sebelum tessellation) - DARI DRAW PERTAMA
      if (currentTemplate) {
        tessellationOriginalRadius = currentTemplate->radius;
        tessellationSpeedMultiplier = currentTemplate->templateSpeedMultiplier; // ⭐ Simpan speed yang dipakai di draw pertama!
      }

      // ⭐ STEP 1.5: Simpan copy dari customLines untuk tessellation SEBELUM clean canvas
      tessellationCustomLines = customLines;  // Copy semua customLines

      // ⭐ RESET wave timer setiap kali tessellation mulai (supaya Draw kedua bisa wave lagi)
      tessellationWaveTime = 0.0f;
      tessellationWaveTimer = 0.0f;
      tessellationWaveFinished = false;

      // ⭐ FLAG untuk reset wave animation di tessellation drawing
      tessellationWaveNeedsReset = false;  // Jangan reset lagi setelah tessellation mulai

      // ⭐ STEP 1.6: Scale tessellationCustomLines dari tessellationOriginalRadius ke tessellationRadius
      // Sama seperti template tessellation yang di-setup ulang dengan radius baru
      float scaleRatio = tessellationRadius / tessellationOriginalRadius;
      if (std::abs(scaleRatio - 1.0f) >= 0.001f) {
        for (auto &line : tessellationCustomLines) {
          // Scale points
          const auto &originalPoints = line.getPoints();
          std::vector<vec2> scaledPoints;
          scaledPoints.reserve(originalPoints.size());
          for (const auto &pt : originalPoints) {
            scaledPoints.push_back(vec2(pt.x * scaleRatio, pt.y * scaleRatio));
          }
          line.setPoints(scaledPoints);

          // Scale juga curve parameter untuk lengkungan yang benar
          line.setCurve(line.getCurve() * scaleRatio);
        }
      }

      tessellationCustomLinesDirty = false;   // Copy sudah valid
      batchedTessellatedCustomLinesMeshDirty = true;  // Rebuild mesh

      // ⭐ STEP 1.7: Reset progress ke 0 untuk animasi grow
      for (auto &line : tessellationCustomLines) {
        line.setProgress(0.0f);
      }

      // STEP 2: Clean canvas (tapi jangan reset speed)
      cleanCanvasInternal(false); // false = jangan reset speed multiplier

      // STEP 3: Set radius = tessellationRadius
      if (currentTemplate) {
        currentTemplate->radius = tessellationRadius; // Set radius tessellation

        // ⭐ Set SPEED MULTIPLIER TERSIMPAN (JANGAN enableAllDrawSettings - biarkan ikut checkbox Playground!)
        currentTemplate->templateSpeedMultiplier = tessellationSpeedMultiplier; // ⭐ PAKAI YANG TERSIMPAN (templateSpeedMultiplier!)

        // STEP 4: Setup shapes DULU (buat shapes baru dengan radius tessellation)
        currentTemplate->setupShapes(); // Setup ulang dengan radius baru - sudah menghormati draw flags!

        // ⭐ Apply speed MULTIPLIER SETELAH setupShapes() - penting!
        currentTemplate->applySpeedMultiplier(); // Apply ke shapes BARU

        // ⭐ TESSELLATION GRID: Generate square grid untuk canvas tessellation
        vec2 viewportSize = vec2(ofGetWidth(), ofGetHeight());
        tessellationManager->generateGrid(tessellationRadius, viewportSize,
                                          canvasTranslation, canvasRotation, canvasZoom);

        // ⭐ Reset template complete flag (template tessellation baru mulai)
        tessellationTemplateComplete = false;

        // ⭐ Set dirty flag untuk rebuild tessellated custom lines mesh
        batchedTessellatedCustomLinesMeshDirty = true;

        // ⭐ Reset semua tessellation animation states
        tessellationManager->resetRadialExpansion();
        diagonalElapsedTime = 0.0f;
        rowElapsedTime = 0.0f;

        // ⭐ Radial Expansion mode: group tiles by distance dari center
        if (tessellationTemplateParallelMode == 1) {
          tessellationManager->groupTilesByDistance(TessellationManager::CHEBYSHEV);

          // ⭐ Calculate ringDuration berdasarkan template animation time
          // ⭐ PARALLEL MODE: Semua shapes animate barengan, jadi ringDuration = shapeDuration
          // ⭐ USE tessellationSpeedMultiplier (sudah di-save di line 258)
          float baseSpeed = tessellationSpeedMultiplier;  // ⭐ Pakai saved value!
          float shapeDuration = 100.0f / (baseSpeed * 60.0f);  // Duration untuk complete template

          // Start radial expansion animation dengan calculated ring duration
          tessellationManager->startRadialExpansion(shapeDuration);
        }
        // ⭐ Diagonal Expansion mode: group tiles by diagonal
        else if (tessellationTemplateParallelMode == 2) {
          tessellationManager->groupTilesByDiagonal();
        }
        // ⭐ Seq Per Row mode: group tiles by row
        else if (tessellationTemplateParallelMode == 3) {
          tessellationManager->groupTilesByRow();
        }

        // ⭐ Gunakan playMode yang sama (Parallel atau Sequential)
        if (tessellationPlayMode == 1) {
          // Sequential mode
          currentTemplate->startSequentialDrawing();
        } else {
          // Parallel mode
          currentTemplate->drawParallel();
        }

        // ⭐ JANGAN restore radius! SacredGeometry slider harus tetap di tessellationRadius
        // currentTemplate->radius = tessellationOriginalRadius; // REMOVED - JANGAN restore!
      }

      // Reset tessellation flag agar tidak loop
      isTessellationEnabled = false;

      // Kembali ke normal
      currentState = UpdateState::NORMAL;
    } else {
      // Tidak ada tessellation, kembali ke normal
      currentState = UpdateState::NORMAL;
    }
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
    const auto &shapes = currentTemplate->getShapes();
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
    const auto &shapes = currentTemplate->getShapes();
    for (auto &shape : shapes) {
      if (!shape->isComplete()) {
        allComplete = false;
        break;
      }
    }

    if (allComplete) {
      loadStage = LOAD_CUSTOMLINES;
      fileManager.setLoadParallelMode(true);
      currentCustomLineIndex =
          0; // Reset index untuk sequential custom line draw ⭐ NEW
    }
  }
}

//--------------------------------------------------------------
void ofApp::updateStaggeredCustomLines() {
  // ⭐ NEW LOGIC: Centralized update logic
  // Determine if we should animate wave based on mode
  bool enableWaveAnimation = false; // Default: Static (Growth Only)
  bool isBeforePolygonDraw = (currentLineStepAnimationMode ==
                              LineStepAnimationMode::STEP_BEFORE_POLYGON_DRAW);
  bool isWithPolygonDraw = (currentLineStepAnimationMode ==
                            LineStepAnimationMode::STEP_WITH_POLYGON_DRAW);
  bool isAfterPolygonDraw = (currentLineStepAnimationMode ==
                             LineStepAnimationMode::STEP_AFTER_POLYGON_DRAW);

  // Check if all lines are loaded based on fileManager state
  bool allLinesLoaded =
      (fileManager.getCurrentLoadIndex() >= fileManager.getTotalLoadedLines());

  // Jika tidak ada custom lines, langsung ke polygon (skip semua logic)
  if (customLines.empty()) {
    loadStage = LOAD_POLYGONS;
    currentPolygonIndex = 0;
    return;
  }

  // TRANSITION LOGIC:
  // 1. With Polygon Draw: Transition IMMEDIATELY to Polygons.
  //    Drawing & Animation will happen in updateStaggeredPolygons.
  if (isWithPolygonDraw) {
    // ⭐ NEW: Transition IMMEDIATELY to Polygons stage
    // Custom Line Growth & Wave will happen concurrently in
    // updateStaggeredPolygons.
    loadStage = LOAD_POLYGONS;
    currentPolygonIndex = 0;
    polygonAnimationsApplied = false;
    return;
  }

  // 2. After Polygon Draw: Transition IMMEDIATELY to Polygons?
  //    NO. User said: "After Polygon Draw ... Custom Line Growth ... then
  //    Polygon ... then Wave". So we MUST draw Custom Lines here (Growth), but
  //    DISABLE Wave.
  if (isAfterPolygonDraw) {
    enableWaveAnimation = false;
    // We fall through to default update logic below.
  }

  // 3. Before Polygon Draw: Draw Growth + Wave. Wait for completion.
  if (isBeforePolygonDraw) {
    enableWaveAnimation = true;
    // We fall through to default update logic below.
  }

  // EXECUTE UPDATE (Growth + Optional Wave)
  float deltaTime = ofGetLastFrameTime();
  // Only enable wave if allowed by staggered logic (growth finished)
  updateCustomLinesLogic(deltaTime, allowStaggeredWaveTrigger);

  // CHECK COMPLETION
  // Only for Before and After modes (With mode already returned)

  bool allComplete = true;
  if (customLineDrawMode == CL_DRAW_PARALLEL) {
    for (const auto &line : customLines) {
      if (line.getProgress() < 1.0f) {
        allComplete = false;
        break;
      }
    }
  } else {
    // SEQUENTIAL Check
    if (currentCustomLineIndex < customLines.size()) {
      allComplete = false;
    }
  }

  // WAITING LOGIC
  if (allComplete && allLinesLoaded) {
    // If Before Polygon Draw, we also need to wait for Wave Animation duration?
    // User said: "After Polygon Draw: Skip Custom Line (Animation). Custom Line
    // Growth... then Polygon... then Wave". Wait, "Wave dimatikan" -> Means we
    // only wait for Growth.

    if (isAfterPolygonDraw) {
      // Growth complete. Wave disabled. Ready to move to Polygons.
      loadStage = LOAD_POLYGONS;
      currentPolygonIndex = 0;
    } else if (isBeforePolygonDraw) {
      // Growth complete. Wave enabled. Wait for Wave duration?
      // Existing logic had waveAnimationTimer.
      // Let's reuse it.

      if (!allowStaggeredWaveTrigger) {
        allowStaggeredWaveTrigger = true;
        waveAnimationApplied = false; // Reset trigger flag
        waveAnimationTimer = 0.0f;
        currentCustomLineWaveIndex = 0;
        customLineWaveTimer = 0.0f;
      }
      waveAnimationTimer += deltaTime;

      if (lineWaveDuration > 0.0f && waveAnimationTimer >= lineWaveDuration) {
        // Wave duration finished.
        loadStage = LOAD_POLYGONS;
        currentPolygonIndex = 0;
        // ⭐ REMOVED: waveAnimationApplied = false;
        // This prevents re-triggering in NORMAL state.

        // ⭐ REMOVED: waveAnimationTimer = 0.0f;

        // ⭐ REMOVED: line.setAnimation(nullptr);
        // We want animations to persist and finish naturally.

        // Load polygons if needed (re-trigger load because we might have
        // delayed it)
        fileManager.updateSequentialLoad(customLines, polygonShapes);
      }
    }
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

  float deltaTime = ofGetLastFrameTime();

  // ⭐ NEW LOGIC: Update Custom Lines during Polygon Load Stage
  bool isWithPolygonDraw = (currentLineStepAnimationMode ==
                            LineStepAnimationMode::STEP_WITH_POLYGON_DRAW);

  if (isWithPolygonDraw) {
    if (customLineDrawMode == CL_DRAW_PARALLEL) {
      // PARALLEL MODE: Update semua bareng + Wave Bareng (handled by helper)
      updateCustomLinesLogic(deltaTime, true);
    } else {
      // SEQUENTIAL MODE: "animasikan custom line secara sequential"
      // 1. Update existing animations (progressive wave)
      for (auto &line : customLines) {
        line.update(deltaTime);
      }

      // 2. Trigger new animations sequentially
      customLineWaveTimer += deltaTime;

      // Calculate delay based on draw speed (faster speed = shorter delay)
      float speed = 1.2f;                  // Use default sequential speed
      float delay = 0.2f / (speed * 2.0f); // Calibrated delay
      if (delay < 0.01f)
        delay = 0.01f;

      if (customLineWaveTimer >= delay) {
        customLineWaveTimer = 0.0f;

        if (currentCustomLineWaveIndex < customLines.size()) {
          if (currentTemplate) {
            BasicZelligeTemplate *zellige =
                dynamic_cast<BasicZelligeTemplate *>(currentTemplate);
            if (zellige) {
              zellige->applyWaveAnimationToCustomLine(
                  this, &customLines[currentCustomLineWaveIndex], true);
            }
          }
          currentCustomLineWaveIndex++;
        }
      }
    }
    bool isAfterPolygonDraw = (currentLineStepAnimationMode ==
                               LineStepAnimationMode::STEP_AFTER_POLYGON_DRAW);
    bool isWithPolygonDraw = (currentLineStepAnimationMode ==
                              LineStepAnimationMode::STEP_WITH_POLYGON_DRAW);

    // ⭐ NEW: Trigged wave in WITH mode or BEFORE mode (continuous) or AFTER
    // mode (if allowed)
    bool enableWave =
        isWithPolygonDraw || (!isAfterPolygonDraw) || allowStaggeredWaveTrigger;
    updateCustomLinesLogic(deltaTime, enableWave);
  }

  // ⭐ NEW: Re-apply polygon animations if not yet done
  if (!polygonAnimationsApplied) {
    reapplyPolygonAnimations();
    polygonAnimationsApplied = true;
  }

  // Update Polygons (Parallel / Sequential)
  // Logic existing sudah OK, tapi kita rapikan sedikit

  float speedMultiplier = 1.0f;
  if (currentTemplate) {
    speedMultiplier = currentTemplate->polygonSpeedMultiplier;
  }

  if (polygonDrawMode == PG_DRAW_PARALLEL) {
    for (auto &polygon : polygonShapes) {
      // Update jika appearance belum complete ATAU special animation belum complete
      if (!polygon.isAnimationComplete() ||
          !polygon.isSpecialAnimationComplete()) {
        polygon.setSpeedMultiplier(speedMultiplier);
        polygon.update(deltaTime);
      }
    }
  } else {
    // SEQUENTIAL POLYGON MODE
    if (currentPolygonIndex < polygonShapes.size()) {
      polygonShapes[currentPolygonIndex].setSpeedMultiplier(speedMultiplier);
      polygonShapes[currentPolygonIndex].update(deltaTime);

      if (polygonShapes[currentPolygonIndex].isAnimationComplete()) {
        currentPolygonIndex++;
      }
    }
    // Update completed polygons (wobble/pulse effects if any)
    for (int i = 0; i < currentPolygonIndex; i++) {
      if (i < polygonShapes.size()) {
        polygonShapes[i].setSpeedMultiplier(speedMultiplier);
        polygonShapes[i].update(deltaTime);
      }
    }
  }

  // Cek apakah semua polygons sudah complete DAN sequential load sudah selesai
  bool allComplete = true;
  bool allLoaded = !fileManager.isLoadSequentialMode();
  bool allPolygonsComplete = true;

  if (polygonDrawMode == PG_DRAW_PARALLEL) {
    for (const auto &polygon : polygonShapes) {
      if (!polygon.isAnimationComplete() ||
          !polygon.isSpecialAnimationComplete()) {
        allComplete = false;
        allPolygonsComplete = false; // logic ini sama saja
        break;
      }
    }
  } else {
    // SEQUENTIAL MODE Check
    if (currentPolygonIndex < polygonShapes.size()) {
      allComplete = false;
      allPolygonsComplete = false;
    } else {
      // Semua appearance complete, tapi cek juga special animations
      for (const auto &polygon : polygonShapes) {
        if (!polygon.isSpecialAnimationComplete()) {
          allComplete = false;
          allPolygonsComplete = false;
          break;
        }
      }
    }
  }

  // Selesai staggered load jika semua complete DAN semua sudah di-load
  if (allComplete && allLoaded) {
    bool isAfterPolygonDraw = (currentLineStepAnimationMode ==
                               LineStepAnimationMode::STEP_AFTER_POLYGON_DRAW);
    if (isAfterPolygonDraw) {
      // Saat polygon selesai, nyalakan flag agar updateCustomLinesLogic
      // mulai trigger wave (Parallel/Sequential)
      allowStaggeredWaveTrigger = true;
      waveAnimationApplied = false; // Reset trigger flag
      waveAnimationTimer = 0.0f;
      currentCustomLineWaveIndex = 0;
      customLineWaveTimer = 0.0f;
    }

    // ⭐ NEW: Hook Tessellation Logic
    processPolygonTessellation();

    loadStage = LOAD_DONE;
    isStaggeredLoad = false;
    fileManager.cancelSequentialLoad();
  }
}

//--------------------------------------------------------------
void ofApp::processPolygonTessellation() {
  BasicZelligeTemplate *zellige =
      dynamic_cast<BasicZelligeTemplate *>(currentTemplate);
  if (!zellige)
    return;

  int originalPolygonCount = static_cast<int>(polygonShapes.size());

  // 🔥 OPTIMIZATION 1: Cache for loaded .nay files to prevent redundant disk
  // reads
  struct CachedNayFile {
    std::vector<PolygonShape> dummyPolygons;
    std::vector<CustomLine> dummyCustomLines;
    float dummyRadius;
    bool hasPolygons;
    bool hasLines;
    float srcMinX, srcMaxX, srcMinY, srcMaxY;
  };
  std::map<std::string, CachedNayFile> nayCache;

  for (int i = 0; i < originalPolygonCount; ++i) {
    if (polygonShapes[i].isTessellated())
      continue;

    // ⭐ FIX: Baca tessellation info dari TEMPLATE UI state, bukan dari polygonShape!
    // Ini agar kita bisa detect perubahan tessellation file di UI (browse)
    std::string nayFile;
    float radius = 10.0f; // Default radius
    if (i < zellige->tessellationFiles.size()) {
      nayFile = zellige->tessellationFiles[i];
    }
    if (i < zellige->tessellationRadii.size()) {
      radius = zellige->tessellationRadii[i];
    }

    // ⭐ CLEANUP LOGIC
    // If this parent polygon already has tessellated children baked/loaded,
    // we need to check if the new UI settings match. If they DON'T match
    // (e.g. user changed radius, file, or clicked 'X' to clear),
    // we MUST delete the old children to prevent stacking or orphaned children.
    bool uiMatchesLoadedState = false;

    // Cek apakah polygon ini punya source tessellation info (dari load file sebelumnya)
    if (polygonShapes[i].hasSourceTessellation()) {
      // Ada source tessellation info! Cek apakah UI masih match atau berubah
      bool fileMatches =
          (nayFile == polygonShapes[i].getSourceTessellationFile());
      bool radiusMatches =
          (std::abs(radius - polygonShapes[i].getSourceTessellationRadius()) <
           0.001f);

      if (!nayFile.empty() && fileMatches && radiusMatches) {
        // UI masih menampilkan tessellation file yang SAMA dengan yang sudah di-load
        uiMatchesLoadedState = true;
      } else {
        // ⭐ UI BERUBAH! User ganti file ATAU klik X (nayFile kosong)
        // Delete old tessellated children!
        // 1. Cleanup old child polygons
        // We must scan from the VERY BEGINNING (index 0) because baked children
        // loaded from the .nay file could be anywhere before
        // `originalPolygonCount`.
        for (int j = 0; j < static_cast<int>(polygonShapes.size());) {
          if (j != i && polygonShapes[j].isTessellated()) {
            vec2 centroid(0, 0);
            const auto &verts = polygonShapes[j].getVertices();
            if (!verts.empty()) {
              for (const auto &v : verts)
                centroid += v;
              centroid /= verts.size();

              if (polygonShapes[i].containsPoint(centroid)) {
                polygonShapes.erase(polygonShapes.begin() + j);
                // Adjust indices since we just shifted the array left
                if (j < i) {
                  i--;
                }
                if (j < originalPolygonCount) {
                  originalPolygonCount--;
                }
                continue; // Do not increment j, check the new element at j
              }
            }
          }
          j++;
        }

        // ⭐ FIX: Clear source tessellation info dari parent polygon
        // Kalau user klik X (nayFile kosong), jangan simpan tessellation info lagi
        polygonShapes[i].setSourceTessellation("", 10.0f);
        batchedTessellatedMeshDirty = true;
      }
    }

    if (uiMatchesLoadedState) {
      // ⭐ FIX: Cek apakah children BENAR-BENAR sudah ada di polygonShapes!
      // Kalau belum ada, tessellation harus jalan walau uiMatchesLoadedState=true
      bool childrenExist = false;
      for (const auto &poly : polygonShapes) {
        if (poly.isTessellated()) {
          // Cek apakah polygon ini ada di dalam parent polygon
          vec2 centroid(0, 0);
          const auto &verts = poly.getVertices();
          if (!verts.empty()) {
            for (const auto &v : verts)
              centroid += v;
            centroid /= verts.size();

            if (polygonShapes[i].containsPoint(centroid)) {
              childrenExist = true;
              break;
            }
          }
        }
      }

      if (childrenExist) {
        // Children sudah ada, skip tessellation
        // ⭐ FIX: Tapi batch mesh perlu di-rebuild untuk render children yang sudah ada!
        batchedTessellatedMeshDirty = true;
        continue;
      }
    }

    // If UI is empty, we have already cleaned up above, so nothing more to do
    // for this polygon.
    if (nayFile.empty())
      continue;

    CachedNayFile cachedData;

    // Check if it's already cached
    auto cacheIt = nayCache.find(nayFile);
    if (cacheIt != nayCache.end()) {
      cachedData = cacheIt->second; // Use cached data
    } else {
      // Not cached, load from disk
      std::string dummyTemplateName;
      float dummyRadius = 0.0f;
      float dummyLineWidth;
      bool dummyLabelsVisible;
      bool dummyDotsVisible;
      std::vector<CustomLine> dummyCustomLines;
      std::vector<PolygonShape> dummyPolygons;
      std::vector<std::unique_ptr<DotShape>> dummyUserDots;
      bool dummyShowUserDot;

      fileManager.loadAllSequential(
          dummyTemplateName, dummyRadius, dummyLineWidth, dummyLabelsVisible,
          dummyDotsVisible, dummyCustomLines, dummyPolygons, dummyUserDots,
          dummyShowUserDot, nayFile);

      // Fetch the parsed shapes from internal buffers
      dummyPolygons = fileManager.getLoadedPolygonsBuffer();
      dummyCustomLines = fileManager.getLoadedLinesBuffer();

      bool hasPolygons = !dummyPolygons.empty();
      bool hasLines = !dummyCustomLines.empty();

      if (!hasPolygons && !hasLines) {
        fileManager.cancelSequentialLoad();
        continue;
      }

      float srcMinX = 999999.0f, srcMaxX = -999999.0f;
      float srcMinY = 999999.0f, srcMaxY = -999999.0f;

      if (hasPolygons) {
        for (auto &p : dummyPolygons) {
          p.update(0.0f);
          for (const auto &v : p.getVertices()) {
            srcMinX = std::min(srcMinX, (float)v.x);
            srcMaxX = std::max(srcMaxX, (float)v.x);
            srcMinY = std::min(srcMinY, (float)v.y);
            srcMaxY = std::max(srcMaxY, (float)v.y);
          }
        }
      } else {
        for (const auto &l : dummyCustomLines) {
          if (l.getPoints().size() < 2)
            continue;
          vec2 start = l.getPoints()[0];
          vec2 end = l.getPoints()[1];
          srcMinX = std::min({srcMinX, (float)start.x, (float)end.x});
          srcMaxX = std::max({srcMaxX, (float)start.x, (float)end.x});
          srcMinY = std::min({srcMinY, (float)start.y, (float)end.y});
          srcMaxY = std::max({srcMaxY, (float)start.y, (float)end.y});
        }
      }

      cachedData.dummyPolygons = dummyPolygons;
      cachedData.dummyCustomLines = dummyCustomLines;
      cachedData.dummyRadius = dummyRadius;
      cachedData.hasPolygons = hasPolygons;
      cachedData.hasLines = hasLines;
      cachedData.srcMinX = srcMinX;
      cachedData.srcMaxX = srcMaxX;
      cachedData.srcMinY = srcMinY;
      cachedData.srcMaxY = srcMaxY;

      nayCache[nayFile] = cachedData;
    }

    float srcWidth = cachedData.srcMaxX - cachedData.srcMinX;
    float srcHeight = cachedData.srcMaxY - cachedData.srcMinY;
    if (srcWidth <= 0 || srcHeight <= 0) {
      fileManager.cancelSequentialLoad();
      continue;
    }

    // Force update target polygon to recalculate bounds
    polygonShapes[i].update(0.0f);

    // Gunakan AABB target geometry langsung dari object cache (jika update()
    // sudah dipanggil) Tapi karena kita baca vertex, mari buat ulang secara
    // explicit untuk aman
    float targetMinX = 999999.0f, targetMaxX = -999999.0f;
    float targetMinY = 999999.0f, targetMaxY = -999999.0f;
    for (const auto &v : polygonShapes[i].getVertices()) {
      targetMinX = std::min(targetMinX, (float)v.x);
      targetMaxX = std::max(targetMaxX, (float)v.x);
      targetMinY = std::min(targetMinY, (float)v.y);
      targetMaxY = std::max(targetMaxY, (float)v.y);
    }

    float scale = (cachedData.dummyRadius > 0.0f)
                      ? (radius / cachedData.dummyRadius)
                      : 1.0f;
    float scaledSrcWidth = srcWidth * scale;
    float scaledSrcHeight = srcHeight * scale;

    vec2 targetCenter = vec2((targetMinX + targetMaxX) / 2.0f,
                             (targetMinY + targetMaxY) / 2.0f);

    int startCol = std::floor((targetMinX - targetCenter.x) / scaledSrcWidth);
    int endCol = std::ceil((targetMaxX - targetCenter.x) / scaledSrcWidth);
    int startRow = std::floor((targetMinY - targetCenter.y) / scaledSrcHeight);
    int endRow = std::ceil((targetMaxY - targetCenter.y) / scaledSrcHeight);

    std::vector<PolygonShape> newPolys;
    std::vector<CustomLine> newLines;
    vec2 srcCenter = vec2((cachedData.srcMinX + cachedData.srcMaxX) / 2.0f,
                          (cachedData.srcMinY + cachedData.srcMaxY) / 2.0f);

    for (int col = startCol; col <= endCol; ++col) {
      for (int row = startRow; row <= endRow; ++row) {
        vec2 gridCenter =
            targetCenter + vec2(col * scaledSrcWidth, row * scaledSrcHeight);

        // Calculate Cell AABB for early exit
        float cellMinX = gridCenter.x - scaledSrcWidth / 2.0f;
        float cellMaxX = gridCenter.x + scaledSrcWidth / 2.0f;
        float cellMinY = gridCenter.y - scaledSrcHeight / 2.0f;
        float cellMaxY = gridCenter.y + scaledSrcHeight / 2.0f;

        // 🔥 OPTIMIZATION 2: Cell-Target AABB Early Exit
        if (cellMaxX < targetMinX || cellMinX > targetMaxX ||
            cellMaxY < targetMinY || cellMinY > targetMaxY) {
          continue; // Skip cell completely if bounds don't even intersect
        }

        if (cachedData.hasPolygons) {
          for (const auto &p : cachedData.dummyPolygons) {
            std::vector<vec2> transformedVerts;
            bool allInside = true;
            for (const auto &v : p.getVertices()) {
              vec2 localV = v - srcCenter;
              localV *= scale;
              vec2 worldV = gridCenter + localV;
              transformedVerts.push_back(worldV);

              if (!polygonShapes[i].containsPoint(worldV)) {
                allInside = false;
                break; // 🔥 OPTIMIZATION 3: Gagal awal, berhenti mengecek titik
                       // sisanya!
              }
            }
            if (!transformedVerts.empty() && allInside) {
              PolygonShape newPoly(transformedVerts, p.getColor());
              newPoly.setLoadedFromFile(true);
              newPoly.setTessellated(true);
              newPolys.push_back(newPoly);
            }
          }
        } else {
          for (const auto &l : cachedData.dummyCustomLines) {
            if (l.getPoints().size() < 2)
              continue;
            vec2 origStart = l.getPoints()[0];
            vec2 origEnd = l.getPoints()[1];

            vec2 start = gridCenter + (origStart - srcCenter) * scale;
            vec2 end = gridCenter + (origEnd - srcCenter) * scale;

            if (polygonShapes[i].containsPoint(start) &&
                polygonShapes[i].containsPoint(end)) {
              std::vector<vec2> pts = {start, end};
              CustomLine newLine(pts, l.getColor(), l.getLineWidth());
              newLine.setCurve(l.getCurve());
              newLine.setLoadedFromFile(true);
              newLines.push_back(newLine);
            }
          }
        }
      }
    }

    fileManager.cancelSequentialLoad();

    if (cachedData.hasPolygons) {
      polygonShapes.insert(polygonShapes.end(), newPolys.begin(),
                           newPolys.end());

      // ⭐ NEW: Mark batch mesh dirty agar rebuild
      batchedTessellatedMeshDirty = true;

      // Explicitly advance the current polygon index so sequential draw doesn't
      // skip the new ones
      if (polygonDrawMode == PG_DRAW_SEQUENTIAL) {
        currentPolygonIndex += static_cast<int>(newPolys.size());
      }
    } else {
      customLines.insert(customLines.end(), newLines.begin(), newLines.end());

      // Explicitly advance the custom line index so sequential draw doesn't
      // skip the new ones
      if (customLineDrawMode == CL_DRAW_SEQUENTIAL) {
        currentCustomLineIndex += static_cast<int>(newLines.size());
      }
    }

    // ⭐ NEW: Save source tessellation state to parent polygon
    polygonShapes[i].setSourceTessellation(nayFile, radius);
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
  const auto &shapes = currentTemplate->getShapes();
  for (auto &shape : shapes) {
    shape->setRadius(currentTemplate->radius);
  }

  // Scale customLines & polygons
  scaleCustomLinesAndPolygons(previousRadius, currentTemplate->radius);
  previousRadius = currentTemplate->radius;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::updateCustomLines() {
  // JANGAN update di NORMAL jika template sedang sequential (Stage 0)
  if (currentTemplate && currentTemplate->sequentialMode && isStaggeredLoad) {
    return;
  }

  fileManager.updateSequentialLoad(customLines, polygonShapes);
  float deltaTime = ofGetLastFrameTime();

  // Jika kita berada di NORMAL mode, dan waveAnimation sudah pernah dijalankan
  // selama draw cycle, maka update logic wave. Tapi JANGAN paksa inisiasi
  // WaveAnimation baru jika tidak ada trigger dari siklus Draw. Wave animation
  // (jika masih berjalan) akan tetap di-update oleh updateCustomLinesLogic.

  bool shouldEnableWaveUpdate = true;
  updateCustomLinesLogic(deltaTime, shouldEnableWaveUpdate);
}

//--------------------------------------------------------------
void ofApp::updateCustomLinesLogic(float deltaTime, bool enableWaveAnimation) {
  // Logic ini dipindahkan dari updateStaggeredCustomLines agar bisa dipakai
  // di fase loading maupun normal update (untuk sequential drawing konsisten)

  // 1. Calculate Draw Speed
  float speedMultiplier = 1.0f;
  if (currentTemplate) {
    speedMultiplier = currentTemplate->customLineSpeedMultiplier;
  }
  float baseSpeed = 1.2f; // Default speed from CustomLine.cpp

  // Tune base speed for Parallel mode (slower because all lines draw at once)
  if (customLineDrawMode == CL_DRAW_PARALLEL) {
    baseSpeed = 0.4f; // ~3x slower base speed for Parallel
  }

  float drawSpeed = baseSpeed * speedMultiplier;

  // 2. Clear animations if disabled (before update loop)
  if (!enableWaveAnimation) {
    for (auto &line : customLines) {
      line.setAnimation(nullptr);
    }
  }

  // 3. Update Lines based on Draw Mode
  if (customLineDrawMode == CL_DRAW_PARALLEL) {
    // PARALLEL MODE: Update semua line sekaligus
    for (auto &line : customLines) {
      line.setSpeed(drawSpeed);
      line.update(deltaTime);
    }
  } else {
    // SEQUENTIAL MODE: Update satu per satu
    if (currentCustomLineIndex < customLines.size()) {
      customLines[currentCustomLineIndex].setSpeed(drawSpeed);
      customLines[currentCustomLineIndex].update(deltaTime);

      if (customLines[currentCustomLineIndex].getProgress() >= 1.0f) {
        currentCustomLineIndex++;
      }
    }

    // Line yang SUDAH selesai tetap di-update
    for (int i = 0; i < currentCustomLineIndex; i++) {
      if (i < customLines.size()) {
        customLines[i].setSpeed(drawSpeed);
        customLines[i].update(deltaTime);
      }
    }
  }

  // 4. Wave Animation Triggering (Sequential vs Parallel) ⭐ NEW
  if (enableWaveAnimation) {
    if (customLineDrawMode == CL_DRAW_PARALLEL) {
      // PARALLEL: Trigger sekali saja untuk semua
      if (!waveAnimationApplied) {
        waveAnimationApplied = true;
        if (currentTemplate && lineAnimationMode == LineAnimationMode::WAVE) {
          BasicZelligeTemplate *zellige =
              dynamic_cast<BasicZelligeTemplate *>(currentTemplate);
          if (zellige) {
            zellige->applyWaveAnimationToAllCustomLines(this, true);
          }
        }
      }
    } else {
      // SEQUENTIAL: Trigger satu per satu dengan delay
      // Hanya mulai trigger jika enableWaveAnimation true
      // DAN jika kita SEDANG menjalankan staggered load atau wave masih proses
      bool isWaveRunning = (currentCustomLineWaveIndex > 0 &&
                            currentCustomLineWaveIndex < customLines.size());
      if (isStaggeredLoad || isWaveRunning) {
        customLineWaveTimer += deltaTime;
        float interval = 0.2f / (drawSpeed * 2.0f); // Calibrated delay
        if (interval < 0.02f)
          interval = 0.02f;

        if (customLineWaveTimer >= interval) {
          customLineWaveTimer = 0.0f;
          if (currentCustomLineWaveIndex < customLines.size()) {
            if (currentTemplate &&
                lineAnimationMode == LineAnimationMode::WAVE) {
              BasicZelligeTemplate *zellige =
                  dynamic_cast<BasicZelligeTemplate *>(currentTemplate);
              if (zellige) {
                zellige->applyWaveAnimationToCustomLine(
                    this, &customLines[currentCustomLineWaveIndex], true);
              }
            }
            currentCustomLineWaveIndex++;
          }
        }
      } else if (!isStaggeredLoad &&
                 lineAnimationMode != LineAnimationMode::WAVE &&
                 currentCustomLineWaveIndex < customLines.size()) {
        // Cepat loncat ke akhir jika draw sudah selesai tapi no animation
        currentCustomLineWaveIndex = static_cast<int>(customLines.size());
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::updatePolygons() {
  float deltaTime = ofGetLastFrameTime();

  // JANGAN update polygons jika sedang staggered load dan belum di stage
  // POLYGONS KHUSUS untuk BEFORE_POLYGON_DRAW mode - tunggu animasi custom
  // lines selesai dulu
  bool isBeforePolygonDraw = (lineStepAnimationMode ==
                              LineStepAnimationMode::STEP_BEFORE_POLYGON_DRAW);
  bool shouldUpdatePolygons = true;

  if (isStaggeredLoad && loadStage != LOAD_POLYGONS && loadStage != LOAD_DONE) {
    // Cek apakah mode BEFORE_POLYGON_DRAW
    if (isBeforePolygonDraw) {
      shouldUpdatePolygons = false; // Jangan update polygons
    }
  }

  // Hanya update polygons jika diperbolehkan
  if (shouldUpdatePolygons) {
    // Update polygons yang BELUM complete (bebas apa pun modenya)
    for (auto &polygon : polygonShapes) {
      if (!polygon.isAnimationComplete()) {
        polygon.update(deltaTime);
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  // ⭐ NEW: Background dengan optional gradient dan trails effect

  // ⭐ FIX: DETECT jika sedang force clear atau force no trails
  bool isForceClearing = (forceClearScreenCounter > 0 || forceNoTrailsCounter > 0);

  // Force clear screen (tanpa trails) setelah clean canvas untuk menghilangkan sisa
  if (forceClearScreenCounter > 0) {
    // Tahap 1: Full clear screen, SKIP semua drawing
    // ⭐ FIX: Hapus glClearColor yang clear ke hitam, langsung gambar background saja
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Lalu gambar background dengan setting yang benar
    if (useCanvasGradient) {
      // Gradient background - gambar full gradient mesh dengan FULL OPAQUE
      ofMesh mesh;
      mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

      ofColor topColor(canvasBgColor[0] * 255, canvasBgColor[1] * 255,
                       canvasBgColor[2] * 255, 255); // FULL OPAQUE
      ofColor bottomColor(canvasGradientColor[0] * 255,
                          canvasGradientColor[1] * 255,
                          canvasGradientColor[2] * 255, 255); // FULL OPAQUE

      mesh.addColor(topColor);
      mesh.addVertex(glm::vec3(0, 0, 0));
      mesh.addColor(topColor);
      mesh.addVertex(glm::vec3(ofGetWidth(), 0, 0));
      mesh.addColor(bottomColor);
      mesh.addVertex(glm::vec3(0, ofGetHeight(), 0));
      mesh.addColor(bottomColor);
      mesh.addVertex(glm::vec3(ofGetWidth(), ofGetHeight(), 0));

      mesh.draw();
    } else {
      // Solid background
      ofBackground(canvasBgColor[0] * 255, canvasBgColor[1] * 255,
                   canvasBgColor[2] * 255);

      ofPushStyle();
      ofSetColor(canvasBgColor[0] * 255, canvasBgColor[1] * 255,
                 canvasBgColor[2] * 255, 255);
      ofFill();
      ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
      ofPopStyle();
    }

    forceClearScreenCounter--;
    return; // Skip SEMUA drawing
  }

  // Tahap 2: Force no trails (solid background) setelah clear screen
  int alpha;
  if (forceNoTrailsCounter > 0) {
    alpha = 255; // Force solid background (tanpa trails)
    forceNoTrailsCounter--;
  } else {
    alpha = (trailMode == 1) ? trailsValue : 255; // Normal trails
  }

  if (useCanvasGradient) {
    // Gradient background (vertical gradient dari top ke bottom)
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    ofColor topColor(canvasBgColor[0] * 255, canvasBgColor[1] * 255,
                     canvasBgColor[2] * 255, alpha);
    ofColor bottomColor(canvasGradientColor[0] * 255,
                        canvasGradientColor[1] * 255,
                        canvasGradientColor[2] * 255, alpha);

    mesh.addColor(topColor);
    mesh.addVertex(glm::vec3(0, 0, 0));
    mesh.addColor(topColor);
    mesh.addVertex(glm::vec3(ofGetWidth(), 0, 0));
    mesh.addColor(bottomColor);
    mesh.addVertex(glm::vec3(0, ofGetHeight(), 0));
    mesh.addColor(bottomColor);
    mesh.addVertex(glm::vec3(ofGetWidth(), ofGetHeight(), 0));

    mesh.draw();
  } else {
    // Solid background color dengan optional trails
    ofSetColor(canvasBgColor[0] * 255, canvasBgColor[1] * 255,
               canvasBgColor[2] * 255, alpha);
    ofFill();
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
  }

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

  drawUserDots();

  // ⭐ TESSELLATION CANVAS: Draw template dengan tessellation grid
  if (currentTemplate) {
    if (tessellationManager && tessellationManager->isActive) {
      // ⭐ TESSELLATION CANVAS: Direct rendering untuk smooth lines!

      if (tessellationTemplateParallelMode == 0) {
        // ⚡ SYNCHRONOUS MODE: Semua tiles animate bersamaan
        const auto& grid = tessellationManager->grid;

        // ⭐ Cek apakah template tessellation sudah complete
        if (currentTemplate && !tessellationTemplateComplete) {
          const auto &shapes = currentTemplate->getShapes();
          bool allComplete = true;
          for (const auto &shape : shapes) {
            if (!shape->isComplete()) {
              allComplete = false;
              break;
            }
          }
          if (allComplete) {
            tessellationTemplateComplete = true;
          }
        }

        for (const auto& gridPos : grid) {
          ofPushMatrix();
          ofTranslate(gridPos.offset.x, gridPos.offset.y);
          currentTemplate->draw();  // Draw full template
          ofPopMatrix();
        }
      } else if (tessellationTemplateParallelMode == 1) {
        // 🌊 RADIAL EXPANSION MODE: Setiap tile animate dari awal dengan timing berbeda
        const auto& grid = tessellationManager->grid;
        const auto& rings = tessellationManager->rings;

        // ⭐ FIX: Calculate ringDuration berdasarkan template animation time
        // ⭐ PARALLEL MODE: Semua shapes animate barengan, jadi total time = shapeDuration (bukan * shapeCount)
        // ⭐ USE SAVED tessellationSpeedMultiplier, bukan live currentTemplate->templateSpeedMultiplier!
        // Ini mencegah perubahan speed saat runtime mengganggu tessellation yang sudah jalan.
        float baseSpeed = tessellationSpeedMultiplier;  // ⭐ Pakai saved value!
        float shapeDuration = 100.0f / (baseSpeed * 60.0f);  // Duration untuk complete template

        // Ring duration = shapeDuration (semua shapes animate barengan)
        float ringDuration = shapeDuration;

        tessellationManager->updateRadialExpansion(ofGetLastFrameTime(), ringDuration);

        // Draw tiles ring by ring dengan masing-masing virtual time
        for (size_t ringIdx = 0; ringIdx < rings.size(); ++ringIdx) {
          const auto& ring = rings[ringIdx];

          // Cek apakah ring ini sedang animasi atau sudah complete
          if (ring.isAnimating || ring.isComplete) {
            // Get elapsed time untuk ring ini (virtual time)
            float virtualTime = tessellationManager->getRingElapsedTime(static_cast<int>(ringIdx));

            // Draw semua tiles di ring ini dengan virtual time yang sama
            for (size_t tileIdx : ring.tileIndices) {
              if (tileIdx < grid.size()) {
                const auto& gridPos = grid[tileIdx];
                ofPushMatrix();
                ofTranslate(gridPos.offset.x, gridPos.offset.y);
                // ⭐ KEY: Draw dengan virtual time - setiap tile animate dari awal!
                currentTemplate->drawAtVirtualTime(virtualTime, shapeDuration);
                ofPopMatrix();
              }
            }
          }
        }
      } else if (tessellationTemplateParallelMode == 2) {
        // 🔷 DIAGONAL EXPANSION MODE: Pattern menyebar dari kiri-atas secara diagonal
        const auto& grid = tessellationManager->grid;
        const auto& diagonals = tessellationManager->diagonals;

        // ⭐ Calculate diagonalDuration berdasarkan template animation time
        float baseSpeed = tessellationSpeedMultiplier;
        float shapeDuration = 100.0f / (baseSpeed * 60.0f);  // Duration untuk complete template
        float diagonalDuration = shapeDuration;

        // Update diagonal animation progress
        diagonalElapsedTime += ofGetLastFrameTime();

        // Draw semua diagonals berdasarkan elapsed time
        for (size_t diagIdx = 0; diagIdx < diagonals.size(); ++diagIdx) {
          // Calculate start dan end time untuk diagonal ini
          float diagonalStartTime = diagIdx * diagonalDuration;
          float diagonalEndTime = (diagIdx + 1) * diagonalDuration;

          // Cek apakah diagonal ini sudah mulai
          if (diagonalElapsedTime >= diagonalStartTime) {
            // Calculate virtual time untuk diagonal ini
            float virtualTime = diagonalElapsedTime - diagonalStartTime;

            // Clamp ke maksimum diagonalDuration
            if (virtualTime > diagonalDuration) {
              virtualTime = diagonalDuration;
            }

            // Draw semua tiles di diagonal ini
            for (size_t tileIdx : diagonals[diagIdx].tileIndices) {
              if (tileIdx < grid.size()) {
                const auto& gridPos = grid[tileIdx];
                ofPushMatrix();
                ofTranslate(gridPos.offset.x, gridPos.offset.y);
                currentTemplate->drawAtVirtualTime(virtualTime, shapeDuration);
                ofPopMatrix();
              }
            }
          }
        }
      } else if (tessellationTemplateParallelMode == 3) {
        // 📊 SEQ PER ROW MODE: Pattern menyebar per baris dari atas ke bawah
        const auto& grid = tessellationManager->grid;
        const auto& rows = tessellationManager->rows;

        // ⭐ Calculate rowDuration berdasarkan template animation time
        float baseSpeed = tessellationSpeedMultiplier;
        float shapeDuration = 100.0f / (baseSpeed * 60.0f);  // Duration untuk complete template
        float rowDuration = shapeDuration;

        // Update row animation progress
        rowElapsedTime += ofGetLastFrameTime();

        // Draw semua rows berdasarkan elapsed time
        for (size_t rowIdx = 0; rowIdx < rows.size(); ++rowIdx) {
          // Calculate start dan end time untuk row ini
          float rowStartTime = rowIdx * rowDuration;
          float rowEndTime = (rowIdx + 1) * rowDuration;

          // Cek apakah row ini sudah mulai
          if (rowElapsedTime >= rowStartTime) {
            // Calculate virtual time untuk row ini
            float virtualTime = rowElapsedTime - rowStartTime;

            // Clamp ke maksimum rowDuration
            if (virtualTime > rowDuration) {
              virtualTime = rowDuration;
            }

            // Draw semua tiles di row ini
            for (size_t tileIdx : rows[rowIdx].tileIndices) {
              if (tileIdx < grid.size()) {
                const auto& gridPos = grid[tileIdx];
                ofPushMatrix();
                ofTranslate(gridPos.offset.x, gridPos.offset.y);
                currentTemplate->drawAtVirtualTime(virtualTime, shapeDuration);
                ofPopMatrix();
              }
            }
          }
        }
      }
    } else {
      // Normal mode: Draw template sekali (existing behavior)
      currentTemplate->draw();
    }

    // ⭐ NEW: TESSELLATION CANVAS - CUSTOM LINES (Synchronous Mode)
    // Draw customLines tessellation HANYA setelah template tessellation complete
    if (tessellationTemplateComplete &&
        tessellationManager && tessellationManager->isActive &&
        !tessellationCustomLines.empty() && tessellationCustomLineParallelMode == 0) {
      // ⚡ SYNCHRONOUS MODE: Semua tiles animate bersamaan
      const auto& grid = tessellationManager->grid;

      // ⭐ Calculate draw speed mengikuti custom line speed slider (sama seperti customLines biasa)
      float speedMultiplier = 1.0f;
      if (currentTemplate) {
        speedMultiplier = currentTemplate->customLineSpeedMultiplier;
      }
      float baseSpeed = 1.2f; // Default speed dari CustomLine.cpp
      float drawSpeed = baseSpeed * speedMultiplier;

      // ⭐ Update progress untuk animasi grow dengan speed yang benar
      float deltaTime = ofGetLastFrameTime();
      for (auto &line : tessellationCustomLines) {
        if (line.getProgress() < 1.0f) {
          line.setSpeed(drawSpeed);  // ⭐ Set speed sesuai slider
          line.updateProgress(deltaTime);
        }
      }

      // ⭐ Hitung average progress untuk cek apakah grow sudah selesai
      float avgProgress = 1.0f;
      if (!tessellationCustomLines.empty()) {
        float totalProgress = 0.0f;
        for (const auto &line : tessellationCustomLines) {
          totalProgress += line.getProgress();
        }
        avgProgress = totalProgress / tessellationCustomLines.size();
      }

      // ⭐ WAVE TIME: HANYA update waveTime jika grow animation SUDAH SELESAI (avgProgress >= 1.0f)
      // Ini SAMA dengan CPU wave yang hanya apply wave setelah progress >= 1.0f
      bool isWaveMode = (lineAnimationMode == LineAnimationMode::WAVE);
      bool isBeforePolygonDraw = (currentLineStepAnimationMode ==
                                  LineStepAnimationMode::STEP_BEFORE_POLYGON_DRAW);
      bool growComplete = (avgProgress >= 1.0f);

      // ⭐ RESET wave state saat grow MULAI (untuk Draw kedua dengan mode yang sama)
      if (!growComplete) {
        tessellationWaveTime = 0.0f;
        tessellationWaveTimer = 0.0f;
        tessellationWaveFinished = false;
      }

      if (isWaveMode && isBeforePolygonDraw && growComplete && !tessellationWaveFinished) {
        // ⭐ Wave time update sesuai CPU wave animation
        tessellationWaveTime += lineWaveSpeed * deltaTime;
        if (tessellationWaveTime > 1.0f) {
          tessellationWaveTime -= 1.0f;  // Loop back to 0.0 (infinite wave)
        }

        // ⭐ Update duration timer dari slider lineWaveDuration
        tessellationWaveTimer += deltaTime;
        if (tessellationWaveTimer >= lineWaveDuration) {
          tessellationWaveFinished = true;  // STOP wave
          tessellationWaveTime = 0.0f;  // Reset wave time
        }
      }

      // ⭐ Reset wave state saat wave disabled (mode berubah)
      if (!isWaveMode || !isBeforePolygonDraw) {
        tessellationWaveTime = 0.0f;
        tessellationWaveTimer = 0.0f;
        tessellationWaveFinished = false;
      }

      for (const auto& gridPos : grid) {
        ofPushMatrix();
        ofTranslate(gridPos.offset.x, gridPos.offset.y);

        // ⭐ Draw tessellation custom lines dengan shader-based rendering
        drawBatchedTessellatedCustomLines(tessellationWaveTime, growComplete);

        ofPopMatrix();
      }
    }
    // TODO: Add Radial, Diagonal, Seq Per Row modes untuk custom lines (modes 1, 2, 3)
  }

  drawUserDots();

  // Draw custom lines dan UI elements
  drawCustomLinesAndUI();

  // ⭐ NEW: Batch tessellated polygons dalam 1 draw call untuk performance
  // Dipanggil PALING AKHIR setelah semuanya
  drawBatchedTessellatedPolygons();

  // Reset transform
  ofPopMatrix();

  // ImGUI (always render if context menu, popups, or visible)
  if (imguiVisible || contextMenu->isVisible() || successPopup->isVisible() ||
      errorPopup->isVisible() || confirmationPopup->isVisible()) {
    drawImGui();
  }
}

//--------------------------------------------------------------
void ofApp::drawCustomLinesAndUI() {
  // TAPI jangan draw kalau sedang staggered load dan belum di stage CUSTOMLINES
  // Ini memastikan lines tidak muncul saat template sedang di-animate
  bool shouldDrawCustomLines = true;
  if (isStaggeredLoad && loadStage == LOAD_TEMPLATE) {
    shouldDrawCustomLines = false;
  }

  if (shouldDrawCustomLines) {
    // Update selection state untuk semua lines
    for (int i = 0; i < customLines.size(); i++) {
      bool isSelected = selectionManager.isLineSelected(i);
      customLines[i].setSelected(isSelected);
      customLines[i].draw();
    }
  }

  // Draw invisible polygons
  // TAPI jangan draw kalau sedang staggered load dan belum di stage POLYGONS
  bool shouldDrawPolygons = true;
  if (isStaggeredLoad && loadStage != LOAD_POLYGONS && loadStage != LOAD_DONE) {
    shouldDrawPolygons = false; // Jangan draw polygons
  }

  if (shouldDrawPolygons) {
    // ⭐ OPTIMIZED: Single loop untuk set selected, draw, dan collect tessellated
    int numPolys = static_cast<int>(polygonShapes.size());

    for (int i = 0; i < numPolys; i++) {
      // SEQUENTIAL MODE: Skip invisible polygons (future)
      if (polygonDrawMode == PG_DRAW_SEQUENTIAL && i > currentPolygonIndex) {
        continue;
      }

      // Set selected state
      polygonShapes[i].setSelected(selectionManager.isPolygonSelected(i));

      // Draw non-tessellated polygons (untuk animation support)
      if (!polygonShapes[i].isTessellated()) {
        polygonShapes[i].draw();
      }
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
  if (drawState != DRAGGING &&
      (!currentTemplate || currentTemplate->dotsVisible)) {
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
void ofApp::keyPressed(int key) { inputManager->handleKeyPressed(key); }

//--------------------------------------------------------------
// Interactive Line Creation Helpers
void ofApp::updateDotsCache() {
  cachedDots.clear();

  // Iterate semua template shapes dan collect dots secara polymorphic
  // Setiap shape bertanggung jawab untuk menambahkan dots-nya sendiri
  if (currentTemplate) {
    const auto &shapes = currentTemplate->getShapes();
    for (auto &shape : shapes) {
      if (shape) {
        shape->addDotsToCache(cachedDots);
      }
    }
  }

  // Tambahkan user-created dots ke cache untuk hover detection
  if (showUserDot) {
    for (auto &dot : userDots) {
      if (dot) {
        dot->addDotsToCache(cachedDots);
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
  return GeometryUtils::isMouseOverLine(mousePos, lineStart, lineEnd,
                                        lineWidth);
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
  return GeometryUtils::getLineIndexAtPosition(pos, customLines,
                                               mouseLineWidth);
}

//--------------------------------------------------------------
void ofApp::pushUndoAction(UndoAction action) {
  // Saat action baru, clear redo stack (redo tidak valid lagi)
  clearRedoStack();

  // Add ke stack, jika sudah full, hapus yang paling lama
  if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
    undoStack.erase(undoStack.begin()); // Hapus yang paling lama (front)
  }
  undoStack.push_back(action);
}

//--------------------------------------------------------------
void ofApp::clearUndoStack() { undoStack.clear(); }

//--------------------------------------------------------------
void ofApp::clearRedoStack() { redoStack.clear(); }

//--------------------------------------------------------------
void ofApp::undo() {
  if (undoStack.empty()) {
    return; // Tidak ada apa-apa untuk di-undo
  }

  // Ambil action terakhir
  UndoAction action = undoStack.back();
  undoStack.pop_back();

  // Siapkan redo action (perlu modifikasi untuk CHANGE actions)
  UndoAction redoAction = action;

  // Handle berdasarkan tipe action
  switch (action.type) {
  case CREATE_LINE: {
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

  case CREATE_POLYGON: {
    // Undo create polygon = hapus polygon terakhir
    if (!polygonShapes.empty()) {
      // SIMPAN polygon yang akan dihapus ke redoAction SEBELUM pop_back
      redoAction.deletedPolygon = polygonShapes.back();
      redoAction.deletedPolygonIndex =
          static_cast<int>(polygonShapes.size()) - 1;

      polygonShapes.pop_back();
      selectionManager.clearPolygonSelection();
    }
    // Push ke redo stack
    redoStack.push_back(redoAction);
    break;
  }

  case CREATE_DOT: {
    // Undo create dot = hapus dot terakhir
    if (!userDots.empty()) {
      // SIMPAN position dan radius dot yang akan dihapus ke redoAction SEBELUM
      // pop_back
      redoAction.deletedDotPos = userDots.back()->getPosition();
      redoAction.deletedDotRadius = userDots.back()->getRadius();

      // Hapus dot terakhir
      userDots.pop_back();
    }
    // Push ke redo stack
    redoStack.push_back(redoAction);
    break;
  }

  case CHANGE_COLOR_LINE: {
    // Capture current colors dulu (sebelum restore) untuk redo
    std::vector<ofColor> currentColors;
    for (size_t i = 0; i < action.colorIndices.size(); i++) {
      int idx = static_cast<int>(action.colorIndices[i]);
      if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
        currentColors.push_back(customLines[idx].getColor());
      }
    }

    // Restore old colors
    for (size_t i = 0;
         i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
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

  case CHANGE_COLOR_POLYGON: {
    // Capture current color dulu (sebelum restore) untuk redo
    ofColor currentColor;
    if (!action.colorIndices.empty() && action.colorIndices[0] >= 0 &&
        action.colorIndices[0] < static_cast<int>(polygonShapes.size())) {
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

  case CHANGE_COLOR_DOT: {
    // Capture current colors dulu (sebelum restore) untuk redo
    std::vector<ofColor> currentColors;
    for (size_t i = 0; i < action.colorIndices.size(); i++) {
      int idx = static_cast<int>(action.colorIndices[i]);
      if (idx >= 0 && idx < static_cast<int>(userDots.size()) &&
          userDots[idx]) {
        currentColors.push_back(userDots[idx]->getColor());
      }
    }

    // Restore old colors
    for (size_t i = 0;
         i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
      int idx = static_cast<int>(action.colorIndices[i]);
      if (idx >= 0 && idx < static_cast<int>(userDots.size()) &&
          userDots[idx]) {
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
    if (action.deletedLineIndex >= 0 &&
        action.deletedLineIndex <= static_cast<int>(customLines.size())) {
      customLines.insert(customLines.begin() + action.deletedLineIndex,
                         action.deletedLine);
    }
    // Push ke redo stack
    redoStack.push_back(redoAction);
    break;

  case DELETE_POLYGON:
    // Restore polygon yang dihapus
    if (action.deletedPolygonIndex >= 0 &&
        action.deletedPolygonIndex <= static_cast<int>(polygonShapes.size())) {
      polygonShapes.insert(polygonShapes.begin() + action.deletedPolygonIndex,
                           action.deletedPolygon);
    }
    // Push ke redo stack
    redoStack.push_back(redoAction);
    break;

  case DELETE_DOT: {
    // Restore userDot yang dihapus
    if (action.deletedDotIndex >= 0 &&
        action.deletedDotIndex <= static_cast<int>(userDots.size())) {
      auto dotShape = std::make_unique<DotShape>(action.deletedDotPos, "Dot",
                                                 action.deletedDotRadius);
      dotShape->progress = 1.0f;
      dotShape->setLowerBound(action.deletedDotLowerBound);
      userDots.insert(userDots.begin() + action.deletedDotIndex,
                      std::move(dotShape));
    }
    // Push ke redo stack
    redoStack.push_back(redoAction);
    break;
  }

  case CHANGE_CURVE: {
    // Capture current curves dulu (sebelum restore) untuk redo
    std::vector<float> currentCurves;
    for (size_t i = 0; i < action.curveLineIndices.size(); i++) {
      int idx = static_cast<int>(action.curveLineIndices[i]);
      if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
        currentCurves.push_back(customLines[idx].getCurve());
      }
    }

    // Restore old curves
    for (size_t i = 0;
         i < action.curveLineIndices.size() && i < action.oldCurves.size();
         i++) {
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
    return; // Tidak ada apa-apa untuk di-redo
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
      auto dotShape = std::make_unique<DotShape>(action.deletedDotPos, "Dot",
                                                 action.deletedDotRadius);
      dotShape->progress = 1.0f;
      userDots.push_back(std::move(dotShape));
    }
    // Push langsung ke undo stack
    if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
      undoStack.erase(undoStack.begin());
    }
    undoStack.push_back(action);
    break;

  case CHANGE_COLOR_LINE: {
    // Capture current colors dulu untuk membuat undo action baru
    UndoAction newUndoAction;
    newUndoAction.type = CHANGE_COLOR_LINE;
    newUndoAction.colorIndices = action.colorIndices;
    newUndoAction.newColor = action.newColor;

    // Apply redo colors (apply newColor) dan simpan current colors
    for (size_t i = 0;
         i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
      int idx = action.colorIndices[i];
      if (idx >= 0 && idx < static_cast<int>(customLines.size())) {
        newUndoAction.oldColors.push_back(customLines[idx].getColor());
        customLines[idx].setColor(action.newColor); // Apply NEW color (redo)
      }
    }
    // Push langsung ke undo stack
    if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
      undoStack.erase(undoStack.begin());
    }
    undoStack.push_back(newUndoAction);
    break;
  }

  case CHANGE_COLOR_POLYGON: {
    // Capture current color
    UndoAction newUndoActionPoly;
    newUndoActionPoly.type = CHANGE_COLOR_POLYGON;
    newUndoActionPoly.newColor = action.newColor;

    if (!action.colorIndices.empty()) {
      int idx = action.colorIndices[0];
      if (idx >= 0 && idx < static_cast<int>(polygonShapes.size())) {
        newUndoActionPoly.colorIndices = action.colorIndices;
        newUndoActionPoly.oldColors.push_back(polygonShapes[idx].getColor());
        polygonShapes[idx].setColor(action.newColor); // Apply NEW color (redo)
      }
    }
    // Push langsung ke undo stack
    if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
      undoStack.erase(undoStack.begin());
    }
    undoStack.push_back(newUndoActionPoly);
    break;
  }

  case CHANGE_COLOR_DOT: {
    // Capture current colors dulu untuk membuat undo action baru
    UndoAction newUndoAction;
    newUndoAction.type = CHANGE_COLOR_DOT;
    newUndoAction.colorIndices = action.colorIndices;
    newUndoAction.newColor = action.newColor;

    // Apply redo colors (apply newColor) dan simpan current colors
    for (size_t i = 0;
         i < action.colorIndices.size() && i < action.oldColors.size(); i++) {
      int idx = action.colorIndices[i];
      if (idx >= 0 && idx < static_cast<int>(userDots.size()) &&
          userDots[idx]) {
        newUndoAction.oldColors.push_back(userDots[idx]->getColor());
        userDots[idx]->setColor(action.newColor); // Apply NEW color (redo)
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
    if (action.deletedLineIndex >= 0 &&
        action.deletedLineIndex < static_cast<int>(customLines.size())) {
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
    if (action.deletedPolygonIndex >= 0 &&
        action.deletedPolygonIndex < static_cast<int>(polygonShapes.size())) {
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
    if (action.deletedDotIndex >= 0 &&
        action.deletedDotIndex < static_cast<int>(userDots.size())) {
      userDots.erase(userDots.begin() + action.deletedDotIndex);
    }
    selectionManager.clearUserDotSelection();
    // Push langsung ke undo stack
    if (undoStack.size() >= UndoStack::MAX_UNDO_STEPS) {
      undoStack.erase(undoStack.begin());
    }
    undoStack.push_back(action);
    break;

  case CHANGE_CURVE: {
    // Capture current curves dan apply redo curves
    UndoAction newUndoActionCurve;
    newUndoActionCurve.type = CHANGE_CURVE;
    newUndoActionCurve.curveLineIndices = action.curveLineIndices;
    newUndoActionCurve.newCurve = action.newCurve;

    for (size_t i = 0;
         i < action.curveLineIndices.size() && i < action.oldCurves.size();
         i++) {
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
    vec2 start;                 // Start point (points[0])
    vec2 end;                   // End point (points[1])
    vector<vec2> sampledPoints; // Full sampled points untuk curve
  };
  vector<LineData> lines;

  // Copy indices ke local vector untuk menghindari iterator invalidation
  std::vector<int> selectedLineIndices(
      selectionManager.getSelectedLineIndices().begin(),
      selectionManager.getSelectedLineIndices().end());
  for (int lineIndex : selectedLineIndices) {
    if (lineIndex >= 0 && lineIndex < customLines.size()) {
      const CustomLine &line = customLines[lineIndex];
      const vector<vec2> &points = line.getPoints();

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
    return; // Tidak ada valid lines
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
        std::reverse(reversed.sampledPoints.begin(),
                     reversed.sampledPoints.end());
        std::swap(reversed.start, reversed.end);
        orderedLines.push_back(reversed);
        lines.erase(lines.begin() + i);
        found = true;
        break;
      }
    }

    // Kalau tidak ketemu yang cocok, coba cari dari awal (untuk handle kasus
    // khusus)
    if (!found) {
      // Coba connect dari start point line pertama
      vec2 currentStart = orderedLines.front().start;
      for (size_t i = 0; i < lines.size(); i++) {
        if (glm::length(lines[i].end - currentStart) < 1.0f) {
          // Found! End point cocok dengan start point pertama (insert di depan)
          LineData reversed = lines[i];
          std::reverse(reversed.sampledPoints.begin(),
                       reversed.sampledPoints.end());
          std::swap(reversed.start, reversed.end);
          orderedLines.insert(orderedLines.begin(), reversed);
          lines.erase(lines.begin() + i);
          found = true;
          break;
        } else if (glm::length(lines[i].start - currentStart) < 1.0f) {
          // Found! Start point cocok dengan start point pertama (insert di
          // depan, no reverse)
          orderedLines.insert(orderedLines.begin(), lines[i]);
          lines.erase(lines.begin() + i);
          found = true;
          break;
        }
      }
    }

    // Kalau masih tidak ketemu, break untuk avoid infinite loop
    if (!found) {
      ofLog(OF_LOG_WARNING) << "createInvisiblePolygon - Cannot connect all "
                               "lines, path is broken";
      break;
    }
  }

  // 3. Extract semua titik dari ordered lines
  vector<vec2> allPoints;
  for (const LineData &lineData : orderedLines) {
    for (const vec2 &p : lineData.sampledPoints) {
      allPoints.push_back(p);
    }
  }

  if (allPoints.size() < 3) {
    // Kurang dari 3 titik, tidak bisa buat polygon
    return;
  }

  int polygonIndex = static_cast<int>(polygonShapes.size());
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
void ofApp::reapplyPolygonAnimations() {
  if (!currentTemplate)
    return;

  // Sync mode dari UI (currentTemplate->polygonAnimationMode)
  // PolygonAnimationMode adalah enum di FileManager.h
  PolygonAnimationMode mode =
      static_cast<PolygonAnimationMode>(currentTemplate->polygonAnimationMode);

  for (int i = 0; i < polygonShapes.size(); i++) {
    // Re-create animation object berdasarkan settings
    // Pakai FileManager helper agar parameternya konsisten
    PolygonShape tempShape = fileManager.createPolygonWithAnimation(
        polygonShapes[i].getVertices(), polygonShapes[i].getColor(), i);

    // Set animation ke shape yang ada
    polygonShapes[i].setAnimation(tempShape.getAnimationPtr());
  }

  // ⭐ NEW: Terapkan juga special animations untuk polygon spesifik
  BasicZelligeTemplate *zellige =
      dynamic_cast<BasicZelligeTemplate *>(currentTemplate);
  if (zellige && !zellige->specialPolygonAnimations.empty()) {
    // Gunakan fungsi dari fileManager untuk menerapkan special animations
    fileManager.applySpecialPolygonAnimations(
        polygonShapes, zellige->specialPolygonAnimations,
        zellige->specialPolygonRotateAngles,
        zellige->specialPolygonPauseDurations); // ⭐ NEW: Pass pause durations
  }
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
void ofApp::mouseMoved(int x, int y) { inputManager->handleMouseMoved(x, y); }

//--------------------------------------------------------------
void ofApp::keyReleased(int key) { inputManager->handleKeyReleased(key); }

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}
void ofApp::mouseExited(int x, int y) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::gotMessage(ofMessage msg) {}
void ofApp::dragEvent(ofDragInfo dragInfo) {}

//--------------------------------------------------------------
void ofApp::startSequentialDrawing() {
  if (!currentTemplate)
    return;

  // 1. Reset state shapes & animations
  resetAllShapesForRedraw();

  // 2. Snapshot animation mode saat drawing dimulai
  currentLineStepAnimationMode = lineStepAnimationMode;
  polygonDrawMode = nextPolygonDrawMode;

  // 3. Clear animation state members
  allowStaggeredWaveTrigger = false;
  waveAnimationApplied = false;
  polygonAnimationsApplied = false; // ⭐ NEW
  waveAnimationTimer = 0.0f;
  currentCustomLineWaveIndex = 0;
  customLineWaveTimer = 0.0f;

  // 4. Reset indices
  currentTemplateIndex = 0;
  currentCustomLineIndex = 0;
  currentPolygonIndex = 0;

  // 5. Delegate ke template untuk inisialisasi construction shapes
  currentTemplate->startSequentialDrawing();
  currentTemplate->applySpeedMultiplier(); // Ensure speed is applied at start

  // 6. Transition ke STAGGERED_LOAD (menggantikan logic NORMAL) ⭐ NEW
  isStaggeredLoad = true;
  currentState = UpdateState::STAGGERED_LOAD;
  loadStage = LOAD_TEMPLATE;
}

//--------------------------------------------------------------
void ofApp::resetAllShapesForRedraw() {
  lineWaveDuration = 5.0f; // Reset default duration

  // Reset Flags
  allowStaggeredWaveTrigger = false;
  polygonAnimationsApplied = false; // ⭐ NEW

  // Reset Custom Lines: Progress 0, No Animation
  for (auto &line : customLines) {
    line.setProgress(0.0f);
    line.setAnimation(nullptr);
  }

  // Reset Polygons: No Animation (agar FadeIn/Wobble mulai dari awal saat
  // update) Polygons tidak punya 'progress' eksplisit tapi AbstractAnimation
  // internal.
  for (auto &polygon : polygonShapes) {
    // Force re-apply animation agar start dari frame 0
    if (currentTemplate) {
      BasicZelligeTemplate *zellige =
          dynamic_cast<BasicZelligeTemplate *>(currentTemplate);
      if (zellige) {
        // Polygons manual biasanya mengikuti setting UI
        // Kita cukup set nullptr dulu agar isAnimationComplete() jadi false
        // (jika sequential) atau restart.
        // Actually, zelligeTemplate->applyAnimationToPolygon(i, mode) adalah yg
        // paling benar.
      }
    }
    // Simple reset: remove animation.
    // Nanti updateStaggeredPolygons akan meng-update-nya.
    // Polygons di BIG biasanya sudah punya animation object sejak dibuat di
    // finalizePolygon.
  }
}

//--------------------------------------------------------------
// NOTE: updateSequentialDrawing() sudah dihapus!
// Logic sekarang ada di SacredGeometryTemplate::updateSequentialDrawing()
// Template handle sequential drawing secara autonomous

//--------------------------------------------------------------
void ofApp::toggleLabels() {
  if (!currentTemplate)
    return;

  // NOTE: ImGui Checkbox SUDAH mengubah nilai labelsVisible
  // Jadi JANGAN toggle lagi, langsung apply saja
  currentTemplate->toggleLabels();
}

//--------------------------------------------------------------
void ofApp::toggleDots() {
  if (!currentTemplate)
    return;

  // NOTE: ImGui Checkbox SUDAH mengubah nilai dotsVisible
  // Jadi JANGAN toggle lagi, langsung apply saja
  currentTemplate->toggleDots();
}

//--------------------------------------------------------------
// (hideAllShapes, showAllShapes, dan setCartesianAxesVisibility sudah dihapus -
// tidak diperlukan lagi dengan Draw Only concept)
//--------------------------------------------------------------
void ofApp::updateLineWidth() {
  if (!currentTemplate)
    return;

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
    const std::set<int> &indices = selectionManager.getSelectedUserDotIndices();
    for (int index : indices) {
      if (index >= 0 && index < userDots.size()) {
        if (userDots[index]) {
          userDots[index]->setRadius(radius);
        }
      }
    }
  } else {
    // Jika tidak ada yang terseleksi, update semua userDots
    for (auto &dot : userDots) {
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
    for (auto &gui : guiComponents) {
      // Cari UserCustom component
      UserCustom *userCustom = dynamic_cast<UserCustom *>(gui.get());
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
    for (auto &gui : guiComponents) {
      // Cari UserCustom component
      UserCustom *userCustom = dynamic_cast<UserCustom *>(gui.get());
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
    // colorManager->userDotColor = loadedColor;  // ColorManager will sync
    // internally

    // Update UserCustom color picker UI
    for (auto &gui : guiComponents) {
      // Cari UserCustom component
      UserCustom *userCustom = dynamic_cast<UserCustom *>(gui.get());
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
  for (auto &gui : guiComponents) {
    UserCustom *userCustom = dynamic_cast<UserCustom *>(gui.get());
    if (userCustom) {
      userCustom->updateUserDotColorFromApp(); // Update userDot color picker
      userCustom->updateColorFromApp();        // Update customLine color picker
      userCustom->updatePolygonColorFromApp(); // Update polygon color picker
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

  // Hapus SEMUA polygons termasuk tessellated - ini untuk CLEAN CANVAS
  deleteAllPolygons(true); // includeTessellated = true

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
void ofApp::deleteAllPolygons(bool includeTessellated) {
  // Jangan hapus jika sedang sequential load
  if (fileManager.isLoadSequentialMode()) {
    return;
  }

  if (includeTessellated) {
    // ⭐ CLEAN CANVAS: Hapus SEMUA polygons termasuk tessellated
    // Reset source tessellation info dari semua polygons SEBELUM clear
    for (auto &poly : polygonShapes) {
      poly.setSourceTessellation("", 10.0f); // Clear tessellation info
    }

    // Hapus semua polygons
    if (!polygonShapes.empty()) {
      polygonShapes.clear();
      selectionManager.clearPolygonSelection();
    }

    // ⭐ FIX: Clear batch tessellated mesh juga
    batchedTessellatedMesh.clear();
    batchedTessellatedMeshDirty = false;
  } else {
    // ⭐ DELETE LINES & POLYGONS: Hanya hapus NON-tessellated polygons
    // Tessellated polygons DIPERTAHANKAN
    // Kita perlu iterate backward untuk aman hapus elements
    for (int i = static_cast<int>(polygonShapes.size()) - 1; i >= 0; i--) {
      if (!polygonShapes[i].isTessellated()) {
        // Hanya NON-tessellated polygons yang dihapus
        polygonShapes.erase(polygonShapes.begin() + i);
      }
      // Tessellated polygons DIPERTAHANKAN (tidak dihapus)
    }

    selectionManager.clearPolygonSelection();

    // ⭐ FIX: Clear batch tessellated mesh juga
    batchedTessellatedMesh.clear();
    batchedTessellatedMeshDirty = true;
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
  const std::set<int> &indices = selectionManager.getSelectedUserDotIndices();
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
      "Are you sure you want to clean the canvas?\n\nEverything on the canvas "
      "will be deleted.",
      "Yes, Clean", "Cancel", [this]() {
        // User klik Yes, execute clean (Reset Speed = true)
        cleanCanvasInternal(true);
      });
}

//--------------------------------------------------------------
void ofApp::cleanCanvasInternal(bool resetSpeed) {
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
    // ⭐ JANGAN reset speed multiplier saat clean canvas - pertahankan speed dari user!
    // if (resetSpeed) {
    //   currentTemplate->templateSpeedMultiplier = 1.0f; // REMOVED - jangan reset speed!
    // }
  }

  // ⭐ TESSELLATION: Clear tessellation grid saat clean canvas
  if (tessellationManager) {
    // Clear tessellation custom lines HANYA jika tessellation sedang aktif
    // (sedang menampilkan tessellation, bukan akan mengaktifkan tessellation)
    if (tessellationManager->isActive) {
      tessellationCustomLines.clear();
    }
    tessellationManager->clearGrid();
  } else {
    // Jika tessellationManager null, clear tessellationCustomLines juga
    tessellationCustomLines.clear();
  }

  // ⭐ Reset Diagonal tessellation animation state
  diagonalElapsedTime = 0.0f;
        rowElapsedTime = 0.0f;

  // Reset semua color pickers ke warna biru default
  colorManager->resetAllColorPickers();

  // Reset UserDot settings ke default
  showUserDot = true;   // Checkbox Dot menjadi checked
  userDotRadius = 8.0f; // Slider radius ke default (8.0f)

  // ⭐ NEW: Force clear screen untuk menghilangkan sisa trails effect
  // ⭐ FIX: 10 frame saja - super cepat!
  forceClearScreenCounter = 10;    // 10 frame untuk clear (0.16 detik)
  forceNoTrailsCounter = 10;       // 10 frame lagi untuk no trails (0.16 detik)
  isFirstDrawAfterClean = false;   // Jangan pakai flag delay
  delayForceClear = 0;             // Reset delay
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
    for (auto &gui : guiComponents) {
      SacredGeometry *sacredGeo = dynamic_cast<SacredGeometry *>(gui.get());
      if (sacredGeo) {
        sacredGeo->showWindow();
        break;
      }
    }
  } else {
    // SacredGeometry already visible, focus it
    for (auto &gui : guiComponents) {
      SacredGeometry *sacredGeo = dynamic_cast<SacredGeometry *>(gui.get());
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
    for (auto &gui : guiComponents) {
      Playground *playground = dynamic_cast<Playground *>(gui.get());
      if (playground) {
        playground->showWindow();
        break;
      }
    }
  } else {
    // Playground already visible, focus it
    for (auto &gui : guiComponents) {
      Playground *playground = dynamic_cast<Playground *>(gui.get());
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
    // Jangan hide SacredGeometry dan Playground, biarkan user punya banyak
    // windows terbuka

    // Set windowOpen flag di UserCustom
    for (auto &gui : guiComponents) {
      UserCustom *userCustom = dynamic_cast<UserCustom *>(gui.get());
      if (userCustom) {
        userCustom->showWindow();
        break;
      }
    }
  } else {
    // UserCustom already visible, focus it
    for (auto &gui : guiComponents) {
      UserCustom *userCustom = dynamic_cast<UserCustom *>(gui.get());
      if (userCustom) {
        userCustom->focusWindow();
        break;
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::toggleCanvasSettingsWindow() {  // ⭐ NEW
  if (!imguiVisible || !showCanvasSettings) {
    // Show CanvasSettings window
    imguiVisible = true;
    showCanvasSettings = true;

    // Set windowOpen flag di CanvasSettings
    for (auto &gui : guiComponents) {
      CanvasSettings *canvasSettings = dynamic_cast<CanvasSettings *>(gui.get());
      if (canvasSettings) {
        canvasSettings->showWindow();
        break;
      }
    }
  } else {
    // CanvasSettings already visible, focus it
    for (auto &gui : guiComponents) {
      CanvasSettings *canvasSettings = dynamic_cast<CanvasSettings *>(gui.get());
      if (canvasSettings) {
        canvasSettings->focusWindow();
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
void ofApp::duplicateDotAbove() { duplicateManager->duplicateDotAbove(); }

//--------------------------------------------------------------
void ofApp::duplicateDotBelow() { duplicateManager->duplicateDotBelow(); }

//--------------------------------------------------------------
void ofApp::duplicateDotLeft() { duplicateManager->duplicateDotLeft(); }

//--------------------------------------------------------------
void ofApp::duplicateDotRight() { duplicateManager->duplicateDotRight(); }

//--------------------------------------------------------------
void ofApp::duplicateDotTrack() { duplicateManager->duplicateDotTrack(); }

//--------------------------------------------------------------
void ofApp::duplicateLineR180() { duplicateManager->duplicateLineR180(); }

//--------------------------------------------------------------
void ofApp::drawUserDots() {

  // Hanya draw userDot jika showUserDot == true
  if (showUserDot) {
    for (auto &dot : userDots) {
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
  for (auto &line : customLines) {
    vector<vec2> points = line.getPoints();
    for (auto &point : points) {
      point = point * scaleRatio; // Scale setiap titik
    }
    line.setPoints(points);

    // Scale juga curve parameter agar kelengkungan proporsional
    float oldCurve = line.getCurve();
    float newCurve = oldCurve * scaleRatio;
    line.setCurve(newCurve);
  }

  // ⭐ FIX: Scale NON-tessellated polygons saja, hapus tessellated children
  // Kita perlu iterate backward untuk aman hapus elements
  for (int i = static_cast<int>(polygonShapes.size()) - 1; i >= 0; i--) {
    if (polygonShapes[i].isTessellated()) {
      // Hapus tessellated children
      polygonShapes.erase(polygonShapes.begin() + i);
    } else {
      // Scale NON-tessellated polygons saja
      vector<vec2> vertices = polygonShapes[i].getVertices();
      for (auto &vertex : vertices) {
        vertex = vertex * scaleRatio; // Scale setiap vertex
      }
      polygonShapes[i].setVertices(vertices);

      // ⭐ FIX: Update source tessellation radius untuk re-tessellation
      if (polygonShapes[i].hasSourceTessellation()) {
        polygonShapes[i].setSourceTessellation(
            polygonShapes[i].getSourceTessellationFile(), newRadius);
      }
    }
  }

  // ⭐ FIX: Mark batch tessellated mesh dirty untuk rebuild
  batchedTessellatedMeshDirty = true;

  // ⭐ FIX: Trigger tessellation ulang dengan radius baru
  processPolygonTessellation();

  // Scale semua userDots (duplikat dots)
  for (auto &dot : userDots) {
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
    return false; // Ada template shapes, canvas tidak kosong
  }

  // Cek customLines
  if (!customLines.empty()) {
    return false; // Ada customLines, canvas tidak kosong
  }

  // Cek polygons
  if (!polygonShapes.empty()) {
    return false; // Ada polygons, canvas tidak kosong
  }

  return true; // Canvas benar-bener kosong
}

//--------------------------------------------------------------
vec2 ofApp::screenToWorld(vec2 screenPos) {
  // Delegate to GeometryUtils
  return GeometryUtils::screenToWorld(screenPos, canvasTranslation, canvasZoom,
                                      canvasRotation);
}

//--------------------------------------------------------------
void ofApp::exit() {
  // Cleanup jika ada

  // ImGUI
  exitImGui();
}

void ofApp::setupImGui() {
  IMGUI_CHECKVERSION(); // cek versi imgui.h and imgui.cpp harus sama
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
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
  guiComponents.push_back(std::make_unique<CanvasSettings>(this));  // ⭐ NEW

  // Initialize Context Menu (bukan bagian dari guiComponents karena draw-nya
  // khusus)
  contextMenu = std::make_unique<ContextMenu>(this);

  // Initialize popup (not in guiComponents, drawn separately)
  successPopup = std::make_unique<SuccessPopup>(this);
  errorPopup = std::make_unique<ErrorPopup>(this);
  confirmationPopup = std::make_unique<ConfirmationPopup>(this);
  saveConfirmationPopup = std::make_unique<SaveConfirmationPopup>(this);

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
    return; // Skip ImGui render jika window minimized
  }

  // Update display size BEFORE NewFrame! (WAJIB!)
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2(width, height);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  // Draw GUI components
  // MenuBar (component 0) selalu draw jika imguiVisible
  if (imguiVisible && !guiComponents.empty()) {
    guiComponents[0]->draw(); // MenuBar
  }

  // SacredGeometry (component 1)
  if (imguiVisible && showSacredGeometry && guiComponents.size() > 1) {
    guiComponents[1]->draw(); // SacredGeometry
  }

  // Playground (component 2)
  if (imguiVisible && showPlayground && guiComponents.size() > 2) {
    guiComponents[2]->draw(); // Playground
  }

  // UserCustom (component 3)
  if (imguiVisible && showUserCustom && guiComponents.size() > 3) {
    guiComponents[3]->draw(); // UserCustom
  }

  // CanvasSettings (component 4) ⭐ NEW
  if (imguiVisible && showCanvasSettings && guiComponents.size() > 4) {
    guiComponents[4]->draw(); // CanvasSettings
  }

  // Draw popup dialogs
  successPopup->draw();
  errorPopup->draw();
  confirmationPopup->draw();
  saveConfirmationPopup->draw();

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

//--------------------------------------------------------------
// ⭐ NEW: Batch tessellated polygons untuk performance optimization
// Render semua tessellated polygons dalam 1 draw call instead of 1 draw call per polygon
void ofApp::drawBatchedTessellatedPolygons() {
  // ⭐ OPTIMIZED: Hanya rebuild mesh jika dirty (polygons berubah)
  if (batchedTessellatedMeshDirty) {
    batchedTessellatedMesh.clear();
    batchedTessellatedMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    int tessellatedCount = 0;

    if (currentTemplate) {
      const auto &shapes = currentTemplate->getShapes();
    }

    // Loop semua shapes untuk cari tessellated polygons
    if (currentTemplate) {
      const auto &shapes = currentTemplate->getShapes();
      for (const auto &shape : shapes) {
        if (!shape) continue;

        // Cek apakah ini PolygonShape dan tessellated
        PolygonShape *poly = dynamic_cast<PolygonShape*>(shape.get());
        if (!poly || !poly->isTessellated()) {
          continue;  // Skip non-tessellated
        }

        tessellatedCount++;

        const auto &vertices = poly->getVertices();
        if (vertices.size() < 3) {
          continue;  // Skip jika kurang dari 3 vertices (bukan polygon)
        }

        const ofColor &color = poly->getColor();

        // Triangulasi polygon dan tambah ke mesh
        // Caranya: triangle fan dari first vertex
        vec2 firstVertex = vertices[0];
        int numVerts = static_cast<int>(vertices.size());

        for (int j = 1; j < numVerts - 1; j++) {
          // Triangle: [0, j, j+1]
          // Vertex 0
          batchedTessellatedMesh.addVertex(glm::vec3(firstVertex.x, firstVertex.y, 0.0f));
          batchedTessellatedMesh.addColor(color);

          // Vertex j
          batchedTessellatedMesh.addVertex(glm::vec3(vertices[j].x, vertices[j].y, 0.0f));
          batchedTessellatedMesh.addColor(color);

          // Vertex j+1
          batchedTessellatedMesh.addVertex(
              glm::vec3(vertices[j + 1].x, vertices[j + 1].y, 0.0f));
          batchedTessellatedMesh.addColor(color);
        }
      }
    }

    // Loop juga polygonShapes (untuk tessellated children yang sudah di-insert)
    for (const auto &polygon : polygonShapes) {
      if (polygon.isTessellated()) {
        tessellatedCount++;

        const auto &vertices = polygon.getVertices();
        if (vertices.size() < 3) {
          continue;  // Skip jika kurang dari 3 vertices (bukan polygon)
        }

        const ofColor &color = polygon.getColor();

        // Triangulasi polygon dan tambah ke mesh
        // Caranya: triangle fan dari first vertex
        vec2 firstVertex = vertices[0];
        int numVerts = static_cast<int>(vertices.size());

        for (int j = 1; j < numVerts - 1; j++) {
          // Triangle: [0, j, j+1]
          // Vertex 0
          batchedTessellatedMesh.addVertex(glm::vec3(firstVertex.x, firstVertex.y, 0.0f));
          batchedTessellatedMesh.addColor(color);

          // Vertex j
          batchedTessellatedMesh.addVertex(glm::vec3(vertices[j].x, vertices[j].y, 0.0f));
          batchedTessellatedMesh.addColor(color);

          // Vertex j+1
          batchedTessellatedMesh.addVertex(
              glm::vec3(vertices[j + 1].x, vertices[j + 1].y, 0.0f));
          batchedTessellatedMesh.addColor(color);
        }
      }
    }

    batchedTessellatedMeshDirty = false;  // Mark sebagai clean
  }

  // Render cached mesh
  if (batchedTessellatedMesh.getNumVertices() > 0) {
    // ⭐ FIX: Setup OpenGL state untuk tessellated mesh
    ofPushStyle();
    ofEnableAlphaBlending();
    batchedTessellatedMesh.enableColors();
    batchedTessellatedMesh.draw();
    ofPopStyle();
  }
}

//--------------------------------------------------------------
// ⭐ NEW: Build mesh dari tessellationCustomLines untuk shader-based rendering
void ofApp::buildTessellatedCustomLinesMesh() {
  batchedTessellatedCustomLinesMesh.clear();
  batchedTessellatedCustomLinesMesh.setMode(OF_PRIMITIVE_LINES);

  // ⭐ PARALLEL MODE: Setiap line punya texCoord 0.0 - 1.0 INDEPENDEN
  // Jadi semua lines animate bersamaan, bukan sequential

  for (const auto &line : tessellationCustomLines) {
    if (line.getPoints().size() < 2) {
      continue;
    }

    const auto &points = line.getPoints();
    const ofColor &color = line.getColor();

    if (std::abs(line.getCurve()) < 0.001f) {
      // ⭐ Straight line - pakai multiple segments untuk wave animation!
      // Tanpa segments, wave hanya akan terlihat di ujung-ujung line
      int segments = 50;  // Cukup untuk smooth wave

      for (int j = 0; j < segments; j++) {
        float t1 = (float)j / segments;  // 0.0 - 1.0 untuk setiap line
        float t2 = (float)(j + 1) / segments;

        // Interpolate posisi untuk straight line
        vec2 p1 = points[0] + (points[1] - points[0]) * t1;
        vec2 p2 = points[0] + (points[1] - points[0]) * t2;

        batchedTessellatedCustomLinesMesh.addVertex(glm::vec3(p1.x, p1.y, 0.0f));
        batchedTessellatedCustomLinesMesh.addColor(color);
        batchedTessellatedCustomLinesMesh.addTexCoord(glm::vec2(t1, 0.0f));

        batchedTessellatedCustomLinesMesh.addVertex(glm::vec3(p2.x, p2.y, 0.0f));
        batchedTessellatedCustomLinesMesh.addColor(color);
        batchedTessellatedCustomLinesMesh.addTexCoord(glm::vec2(t2, 0.0f));
      }
    } else {
      // Curved line - lebih banyak segments untuk smooth
      int segments = 100;
      vec2 controlPoint = (points[0] + points[1]) / 2.0f;
      vec2 dir = points[1] - points[0];
      vec2 perp = vec2(-dir.y, dir.x);
      float perpLen = glm::length(perp);
      if (perpLen > 0) {
        perp = perp / perpLen;
      }
      controlPoint = controlPoint + perp * line.getCurve();

      for (int j = 0; j < segments; j++) {
        float t1 = (float)j / segments;  // 0.0 - 1.0 untuk setiap line
        float t2 = (float)(j + 1) / segments;

        vec2 p1 = points[0] * (1 - t1) * (1 - t1) + controlPoint * 2 * (1 - t1) * t1 + points[1] * t1 * t1;
        vec2 p2 = points[0] * (1 - t2) * (1 - t2) + controlPoint * 2 * (1 - t2) * t2 + points[1] * t2 * t2;

        // texCoord untuk PARALLEL animation (setiap line 0.0 - 1.0)
        batchedTessellatedCustomLinesMesh.addVertex(glm::vec3(p1.x, p1.y, 0.0f));
        batchedTessellatedCustomLinesMesh.addColor(color);
        batchedTessellatedCustomLinesMesh.addTexCoord(glm::vec2(t1, 0.0f));

        batchedTessellatedCustomLinesMesh.addVertex(glm::vec3(p2.x, p2.y, 0.0f));
        batchedTessellatedCustomLinesMesh.addColor(color);
        batchedTessellatedCustomLinesMesh.addTexCoord(glm::vec2(t2, 0.0f));
      }
    }
  }

  batchedTessellatedCustomLinesMeshDirty = false;
}

//--------------------------------------------------------------
// ⭐ NEW: Draw tessellated custom lines dengan Shader (GPU rendering)
void ofApp::drawBatchedTessellatedCustomLines(float waveTime, bool growComplete) {
  if (tessellationCustomLines.empty()) {
    return;
  }

  // Build mesh jika dirty
  if (batchedTessellatedCustomLinesMeshDirty) {
    buildTessellatedCustomLinesMesh();
  }

  ofPushStyle();
  ofEnableAlphaBlending();

  // Get line width dari tessellationCustomLines (sesuai file .nay)
  float lineWidth = 3.0f;
  if (!tessellationCustomLines.empty()) {
    lineWidth = tessellationCustomLines[0].getLineWidth();
  }
  ofSetLineWidth(lineWidth);

  // Bind shader
  if (customLineShader.isLoaded()) {
    customLineShader.begin();

    // Set modelViewProjectionMatrix - combine modelview dan projection
    glm::mat4 modelView = ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
    glm::mat4 projection = ofGetCurrentMatrix(OF_MATRIX_PROJECTION);
    glm::mat4 mvp = projection * modelView;
    customLineShader.setUniformMatrix4f("modelViewProjectionMatrix", mvp);

    // Set lineWidth uniform (untuk reference, actual width set via glLineWidth)
    customLineShader.setUniform1f("lineWidth", lineWidth);

    // ⭐ Set progress uniform untuk grow animation (0.0 - 1.0)
    float avgProgress = 1.0f;
    if (!tessellationCustomLines.empty()) {
      float totalProgress = 0.0f;
      for (const auto &line : tessellationCustomLines) {
        totalProgress += line.getProgress();
      }
      avgProgress = totalProgress / tessellationCustomLines.size();
    }
    customLineShader.setUniform1f("progress", avgProgress);

    // ⭐ WAVE ANIMATION: Set wave parameters jika mode Wave + Before Polygon Draw aktif di Playground
    // DAN grow animation SUDAH SELESAI (sama seperti CPU wave yang hanya apply setelah progress >= 1.0f)
    float waveAmp = 0.0f;
    float waveFreq = 0.0f;
    float waveT = 0.0f;

    // Cek apakah wave animation di-enable dan Before Polygon Draw mode DAN grow complete
    bool isWaveMode = (lineAnimationMode == LineAnimationMode::WAVE);
    bool isBeforePolygonDraw = (currentLineStepAnimationMode ==
                                LineStepAnimationMode::STEP_BEFORE_POLYGON_DRAW);

    // ⭐ HANYA apply wave JIKA wave belum finished (duration belum habis)
    // Setelah duration habis, waveAmp = 0 (tidak ada gelombang, kembali ke posisi semula)
    if (isWaveMode && isBeforePolygonDraw && growComplete && !tessellationWaveFinished) {
      waveAmp = lineWaveAmplitude;
      waveFreq = lineWaveFrequency;
      waveT = waveTime;  // ⭐ Use looping waveTime dari tessellation drawing
    }

    // ⭐ KIRIM wave uniforms ke shader
    customLineShader.setUniform1f("waveAmplitude", waveAmp);
    customLineShader.setUniform1f("waveFrequency", waveFreq);
    customLineShader.setUniform1f("waveTime", waveT);

    // Draw mesh
    batchedTessellatedCustomLinesMesh.draw();

    customLineShader.end();
  } else {
    // Fallback: CPU rendering
    ofSetLineWidth(lineWidth);
    for (const auto &line : tessellationCustomLines) {
      line.draw();
    }
  }

  ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::exitImGui() {
  // Shutdown OpenGL3 Backend
  ImGui_ImplOpenGL3_Shutdown();

  // Shutdown Win32 Backend
  ImGui_ImplWin32_Shutdown();

  // Destroy ImGui Context
  ImGui::DestroyContext();
}
