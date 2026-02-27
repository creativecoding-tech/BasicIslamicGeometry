#include "BasicZelligeTemplate.h"
#include "../../anim/WaveLineAnimation.h"
#include "../../imgui/imgui.h"
#include "../../ofApp.h"
#include "../../operation/FileManager.h"
#include "../../shape/CartesianAxes.h"
#include "../../shape/CircleShape.h"
#include "../../shape/CrossLine.h"
#include "../../shape/OctagramLine.h"
#include "../../shape/ParallelogramLine.h"
#include "../../shape/RectangleLine.h"

using glm::vec2;

//--------------------------------------------------------------
std::string BasicZelligeTemplate::getName() { return "Basic Zellige"; }

//--------------------------------------------------------------
std::string BasicZelligeTemplate::getDescription() {
  return "Moroccan Islamic Geometric Pattern - Basic Zellige mosaic with "
         "8-fold symmetry";
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupShapes() {
  // Setup shapes berdasarkan checkbox preference dari Playground (Draw Only
  // concept) Hanya shapes yang dicentang yang akan dibuat Note: 'shapes' adalah
  // protected member dari SacredGeometryTemplate

  if (drawCartesian) {
    setupCartesianAxes();
  }

  if (drawCircles) {
    setupCircles();
  }

  if (drawCrossLines) {
    setupCrossLines();
  }

  if (drawParallelograms) {
    setupParallelograms();
  }

  if (drawRectangleLines) {
    setupRectangleLines();
  }

  if (drawOctagramLines) {
    setupOctagramLines();
  }
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupCircles() {
  // Clear daftar circles dulu
  circleIndices.clear();

  // Circle A: Center (0,0) → distance=0
  shapes.push_back(std::make_unique<CircleShape>(radius, "A", 0, 0));
  circleIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Circle A

  // Circle B: Kanan (0°) → distance=radius
  shapes.push_back(std::make_unique<CircleShape>(radius, "B", 0, radius));
  circleIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Circle B

  // Circle C: Kiri (180°) → distance=radius
  shapes.push_back(std::make_unique<CircleShape>(radius, "C", PI, radius));
  circleIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Circle C

  // Circle D: Atas (90°) → distance=radius
  shapes.push_back(std::make_unique<CircleShape>(radius, "D", PI / 2, radius));
  circleIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Circle D

  // Circle E: Bawah (270°) → distance=radius
  shapes.push_back(std::make_unique<CircleShape>(radius, "E", -PI / 2, radius));
  circleIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Circle E
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupCartesianAxes() {
  shapes.push_back(std::make_unique<CartesianAxes>(radius));
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupCrossLines() {
  // Clear daftar crosslines dulu
  crossLineIndices.clear();

  // CrossLine F
  shapes.push_back(std::make_unique<CrossLine>(
      vec2(0, 0), vec2(-radius, -radius), "F", "J", radius));
  crossLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index CrossLine F

  // CrossLine G
  shapes.push_back(std::make_unique<CrossLine>(
      vec2(0, 0), vec2(radius, -radius), "G", "K", radius));
  crossLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index CrossLine G

  // CrossLine H
  shapes.push_back(std::make_unique<CrossLine>(
      vec2(0, 0), vec2(-radius, radius), "H", "L", radius));
  crossLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index CrossLine H

  // CrossLine I
  shapes.push_back(std::make_unique<CrossLine>(vec2(0, 0), vec2(radius, radius),
                                               "I", "M", radius));
  crossLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index CrossLine I
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupParallelograms() {
  // Clear daftar parallelograms dulu
  parallelogramIndices.clear();

  // Parallelogram dengan Polar Thinking
  // Intersection point dihitung menggunakan trigonometri: x = cos(angle) *
  // distance, y = sin(angle) * distance

  // C→E memotong F: Northwest (-120, -120)
  float angleC_E_F = -3 * PI / 4; // -135 derajat (Northwest)
  float distC_E_F = radius * sqrt(2) / 2;
  vec2 intersecC_E_F =
      vec2(cos(angleC_E_F) * distC_E_F, sin(angleC_E_F) * distC_E_F);
  shapes.push_back(std::make_unique<ParallelogramLine>(
      vec2(-radius, 0), vec2(0, -radius), intersecC_E_F, "N", radius));
  parallelogramIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Parallelogram N

  // E→B memotong G: Northeast (120, -120)
  float angleE_B_G = -PI / 4; // -45 derajat (Northeast)
  float distE_B_G = radius * sqrt(2) / 2;
  vec2 intersecE_B_G =
      vec2(cos(angleE_B_G) * distE_B_G, sin(angleE_B_G) * distE_B_G);
  shapes.push_back(std::make_unique<ParallelogramLine>(
      vec2(0, -radius), vec2(radius, 0), intersecE_B_G, "O", radius));
  parallelogramIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Parallelogram O

  // B→D memotong I: Southeast (120, 120)
  float angleB_D_I = PI / 4; // 45 derajat (Southeast)
  float distB_D_I = radius * sqrt(2) / 2;
  vec2 intersecB_D_I =
      vec2(cos(angleB_D_I) * distB_D_I, sin(angleB_D_I) * distB_D_I);
  shapes.push_back(std::make_unique<ParallelogramLine>(
      vec2(radius, 0), vec2(0, radius), intersecB_D_I, "P", radius));
  parallelogramIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Parallelogram P

  // D→C memotong H: Southwest (-120, 120)
  float angleD_C_H = 3 * PI / 4; // 135 derajat (Southwest)
  float distD_C_H = radius * sqrt(2) / 2;
  vec2 intersecD_C_H =
      vec2(cos(angleD_C_H) * distD_C_H, sin(angleD_C_H) * distD_C_H);
  shapes.push_back(std::make_unique<ParallelogramLine>(
      vec2(0, radius), vec2(-radius, 0), intersecD_C_H, "Q", radius));
  parallelogramIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index Parallelogram Q
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupRectangleLines() {
  // Clear daftar rectangleLines dulu
  rectangleLineIndices.clear();

  // RectangleLine dari F ke G dengan intersection points
  float angleF = -3 * PI / 4; // -135°
  vec2 posF = vec2(cos(angleF) * radius, sin(angleF) * radius);

  float angleG = -PI / 4; // -45°
  vec2 posG = vec2(cos(angleG) * radius, sin(angleG) * radius);

  // Hitung intersection F→G ∩ C→E (Dot R)
  vec2 intersecR = vec2(-radius * (1 - sqrt(2) / 2), posF.y);

  // Hitung intersection F→G ∩ B→E (Dot S)
  vec2 intersecS = vec2(radius * (1 - sqrt(2) / 2), posF.y);

  shapes.push_back(std::make_unique<RectangleLine>(
      posF, posG, intersecR, intersecS, "R", "S", radius));
  rectangleLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index RectangleLine RS

  // RectangleLine G ke I
  float angleI = PI / 4; // 45°
  vec2 posI = vec2(cos(angleI) * radius, sin(angleI) * radius);

  // Hitung intersection G→I ∩ B→E (Dot T)
  vec2 intersecT = vec2(radius * sqrt(2) / 2, radius * (sqrt(2) / 2 - 1));

  // Hitung intersection G→I ∩ B→D (Dot U)
  vec2 intersecU = vec2(radius * sqrt(2) / 2, radius * (1 - sqrt(2) / 2));

  shapes.push_back(std::make_unique<RectangleLine>(
      posG, posI, intersecT, intersecU, "T", "U", radius));
  rectangleLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index RectangleLine TU

  // RectangleLine I ke H
  float angleH = 3 * PI / 4; // 135°
  vec2 posH = vec2(cos(angleH) * radius, sin(angleH) * radius);
  vec2 intersecV = vec2(radius * (1 - sqrt(2) / 2), posH.y);
  vec2 intersecW = vec2(-radius * (1 - sqrt(2) / 2), posH.y);
  shapes.push_back(std::make_unique<RectangleLine>(
      posI, posH, intersecV, intersecW, "V", "W", radius));
  rectangleLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index RectangleLine VW

  // RectangleLine H ke F
  vec2 intersecX = vec2(-radius * sqrt(2) / 2, radius * (1 - sqrt(2) / 2));
  vec2 intersecY = vec2(-radius * sqrt(2) / 2, radius * (sqrt(2) / 2 - 1));
  shapes.push_back(std::make_unique<RectangleLine>(
      posH, posF, intersecX, intersecY, "X", "Y", radius));
  rectangleLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index RectangleLine XY
}

//--------------------------------------------------------------
void BasicZelligeTemplate::setupOctagramLines() {
  // Clear daftar octagramLines dulu
  octagramLineIndices.clear();

  // Posisi Circle E
  vec2 posE = vec2(0, -radius);

  // Posisi di -45° dari Circle E dengan distance r = 240
  float angle0 = -PI / 4; // -45°
  vec2 posEnd =
      vec2(posE.x + cos(angle0) * radius, posE.y + sin(angle0) * radius);

  // Posisi Dot G (di -45° dari center)
  vec2 posG = vec2(cos(angle0) * radius, sin(angle0) * radius);

  shapes.push_back(
      std::make_unique<OctagramLine>(posE, posEnd, posG, "0", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 0

  // OctagramLine 2: Dot G → -45° dari Circle B → pusat Circle B
  vec2 posB = vec2(radius, 0); // (240, 0)
  vec2 posEnd1 =
      vec2(posB.x + cos(angle0) * radius, posB.y + sin(angle0) * radius);
  shapes.push_back(
      std::make_unique<OctagramLine>(posG, posEnd1, posB, "1", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 1

  // OctagramLine 3: Circle B → 45° dari Circle B → Dot I
  float angle2 = PI / 4; // 45°
  vec2 posEnd2 =
      vec2(posB.x + cos(angle2) * radius, posB.y + sin(angle2) * radius);
  vec2 posI = vec2(cos(angle2) * radius, sin(angle2) * radius);
  shapes.push_back(
      std::make_unique<OctagramLine>(posB, posEnd2, posI, "2", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 2

  // OctagramLine 4: Circle D → +45° dari Dot I → Circle D
  vec2 posD = vec2(0, radius);
  float angle3 = angle2; // 45°
  vec2 posEnd3 =
      vec2(posD.x + cos(angle3) * radius, posD.y + sin(angle3) * radius);
  shapes.push_back(
      std::make_unique<OctagramLine>(posI, posEnd3, posD, "3", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 3

  // OctagramLine 5: Dot D → posEnd
  float angle4 = 3 * angle2; // 135°
  vec2 posEnd4 =
      vec2(posD.x + cos(angle4) * radius, posD.y + sin(angle4) * radius);
  vec2 posH = vec2(cos(angle4) * radius, sin(angle4) * radius);
  shapes.push_back(
      std::make_unique<OctagramLine>(posD, posEnd4, posH, "4", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 4

  // OctagramLine 6: posH → posEnd5 → posC
  vec2 posC = vec2(-radius, 0);
  float angle5 = angle4; // 135°
  vec2 posEnd5 =
      vec2(posC.x + cos(angle5) * radius, posC.y + sin(angle5) * radius);
  shapes.push_back(
      std::make_unique<OctagramLine>(posH, posEnd5, posC, "5", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 5

  // OctagramLine 7: posC → posEnd6 → posF
  float angle6 = -angle5; // -135°
  vec2 posEnd6 =
      vec2(posC.x + cos(angle6) * radius, posC.y + sin(angle6) * radius);
  vec2 posF = vec2(cos(angle6) * radius, sin(angle6) * radius);
  shapes.push_back(
      std::make_unique<OctagramLine>(posC, posEnd6, posF, "6", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 6

  // OctagramLine 8: posF → posEnd7 → posE
  float angle7 = angle6; // -135°
  vec2 posEnd7 =
      vec2(posE.x + cos(angle7) * radius, posE.y + sin(angle7) * radius);
  shapes.push_back(
      std::make_unique<OctagramLine>(posF, posEnd7, posE, "7", radius));
  octagramLineIndices.push_back(
      static_cast<int>(shapes.size() - 1)); // Simpan index OctagramLine 7
}

//--------------------------------------------------------------
void BasicZelligeTemplate::applyWaveAnimationToAllCustomLines(
    class ofApp *app, bool enableAnimation) {
  // Apply wave animation settings ke SEMUA customLines
  for (auto &line : app->customLines) {
    applyWaveAnimationToCustomLine(app, &line, enableAnimation);
  }
}

void BasicZelligeTemplate::applyWaveAnimationToCustomLine(
    class ofApp *app, class CustomLine *line, bool enableAnimation) {
  // Check if animation has finished naturally (auto-stop)
  if (line->hasFinishedAnimation()) {
    return;
  }

  if (enableAnimation &&
      app->lineAnimationMode == ofApp::LineAnimationMode::WAVE) {
    // Mode Wave: Cek apakah sudah punya WaveLineAnimation
    if (auto *existingWaveAnim =
            dynamic_cast<WaveLineAnimation *>(line->getAnimation().get())) {
      // Sudah punya WaveLineAnimation, cukup update parameter-nya
      existingWaveAnim->setAmplitude(app->lineWaveAmplitude);
      existingWaveAnim->setFrequency(app->lineWaveFrequency);
      existingWaveAnim->setSpeed(
          app->lineWaveSpeed); // Update speed juga from slider ⭐ NEW
    } else {
      // Belum punya atau bukan WaveLineAnimation, buat baru
      auto waveAnim = std::make_shared<WaveLineAnimation>(
          app->lineWaveAmplitude, app->lineWaveFrequency, app->lineWaveSpeed);
      // Set animation dengan durasi dari slider ⭐ NEW
      line->setAnimation(waveAnim, app->lineWaveDuration);
    }
  } else {
    // Mode No Animation (atau enableAnimation false): Hapus animation
    line->setAnimation(nullptr);
  }

  // Pastikan loadedFromFile = false untuk manual customLines
  line->setLoadedFromFile(false);
}

//--------------------------------------------------------------
bool BasicZelligeTemplate::hasCustomSettings() { return false; }

//--------------------------------------------------------------
void BasicZelligeTemplate::showSettingsUI() {
  ImGui::Text("Zellige Pattern Controls");

  static bool showCircles = true;
  static bool showCrossLines = true;
  static bool showParallelograms = true;
  static bool showRectangleLines = true;
  static bool showOctagramLines = true;

  if (ImGui::Checkbox("Show Circles (A-E)", &showCircles)) {
    // TODO: Toggle circle shapes visibility
  }
  if (ImGui::Checkbox("Show Cross Lines (F-I)", &showCrossLines)) {
    // TODO: Toggle cross lines visibility
  }
  if (ImGui::Checkbox("Show Parallelograms (N-Q)", &showParallelograms)) {
    // TODO: Toggle parallelogram lines visibility
  }
  if (ImGui::Checkbox("Show Rectangle Lines", &showRectangleLines)) {
    // TODO: Toggle rectangle lines visibility
  }
  if (ImGui::Checkbox("Show Octagram Lines (0-7)", &showOctagramLines)) {
    // TODO: Toggle octagram lines visibility
  }

  ImGui::Separator();

  static int octagramPoints = 8;
  ImGui::SetNextItemWidth(150.0f);
  if (ImGui::SliderInt("Octagram Points", &octagramPoints, 4, 12)) {
    // TODO: Rebuild octagram with new point count
  }
}

//--------------------------------------------------------------
bool BasicZelligeTemplate::hasPlaybackSettings() {
  return showPlaybackSettings;
}

//--------------------------------------------------------------
void BasicZelligeTemplate::showPlaybackUI(ofApp *app) {
  // Template CollapsingHeader
  if (ImGui::CollapsingHeader("Template", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("Mode Draw");

    if (ImGui::RadioButton("Parallel", &playMode, 0)) {
      // Radio button changed
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150.0f);
    if (ImGui::RadioButton("Sequential", &playMode, 1)) {
      // Radio button changed
    }
    ImGui::Separator();
    ImGui::Text("Draw Settings");
    if (ImGui::BeginTable("DrawSettingsTable", 2, ImGuiTableFlags_None)) {
      ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthStretch);

      // Row 1
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      if (ImGui::Checkbox("Cartesian", &drawCartesian)) {
      }
      ImGui::TableSetColumnIndex(1);
      if (ImGui::Checkbox("Circles", &drawCircles)) {
      }

      // Row 2
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      if (ImGui::Checkbox("CrossLines", &drawCrossLines)) {
      }
      ImGui::TableSetColumnIndex(1);
      if (ImGui::Checkbox("Parallelograms", &drawParallelograms)) {
      }

      // Row 3
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      if (ImGui::Checkbox("RectangleLines", &drawRectangleLines)) {
      }
      ImGui::TableSetColumnIndex(1);
      if (ImGui::Checkbox("OctagramLines", &drawOctagramLines)) {
      }

      ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Template Speed");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80.0f);
    ImGui::SliderFloat("##TemplateSpeed",
                       &app->currentTemplate->templateSpeedMultiplier, 0.1f,
                       3.0f, "%.2f");
  }

  ImGui::Separator();

  // Custom Line CollapsingHeader - hanya muncul jika ada customLine dari file
  // .nay
  bool hasCustomLinesInFile = app->loadedFileCustomLinesCount > 0;

  if (hasCustomLinesInFile) {
    if (ImGui::CollapsingHeader("Custom Line",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      // Draw Custom Lines checkbox - update flag di ofApp
      ImGui::Checkbox("Draw Custom Lines", &app->shouldDrawCustomLines);

      // Custom Line Appearance section - hanya muncul jika checkbox dicheck
      if (app->shouldDrawCustomLines) {
        // Mode Draw Custom Lines ⭐ NEW
        ImGui::Text("Mode Draw");
        if (ImGui::RadioButton(
                "Parallel##CL",
                reinterpret_cast<int *>(&app->customLineDrawMode), 0)) {
        }
        ImGui::SameLine();
        if (ImGui::RadioButton(
                "Sequential##CL",
                reinterpret_cast<int *>(&app->customLineDrawMode), 1)) {
        }
        ImGui::Separator();
        ImGui::Text("Custom Line Animation");

        if (ImGui::BeginTable("CustomLineAnimTable", 2, ImGuiTableFlags_None)) {
          // Column 1 setup
          ImGui::TableSetupColumn("AnimationTypes",
                                  ImGuiTableColumnFlags_WidthFixed, 180.0f);
          ImGui::TableSetupColumn("WaveSettings",
                                  ImGuiTableColumnFlags_WidthStretch);

          // Row 1: No Animation
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::RadioButton("No Animation Line",
                             reinterpret_cast<int *>(&app->lineAnimationMode),
                             0);

          // Row 2: Wave Animation
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::RadioButton("Wave Animation Line",
                             reinterpret_cast<int *>(&app->lineAnimationMode),
                             1);

          // Column 2: Wave Settings (only if Wave is selected)
          if (app->lineAnimationMode == ofApp::LineAnimationMode::WAVE) {
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Wave Settings");
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderFloat("Amplitude (px)", &app->lineWaveAmplitude, 2.0f,
                               5.0f);
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderFloat("Frequency", &app->lineWaveFrequency, 1.0f,
                               3.0f);
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderFloat("Duration (sec)", &app->lineWaveDuration, 0.0f,
                               60.0f,
                               "%.1f"); // Slider durasi 0-60 detik ⭐ NEW
          }

          ImGui::EndTable();
        }

        ImGui::Separator();

        // Step Animation Line UI (hanya muncul jika selain No Animation dipilih
        // DAN ada polygons) ⭐ NEW
        if (app->lineAnimationMode != ofApp::LineAnimationMode::NO_ANIMATION) {
          // Cek apakah ada polygon dari file .nay
          if (app->loadedFilePolygonCount > 0) {
            ImGui::Text("Step Animation Line");
            ImGui::RadioButton(
                "Before Polygon Draw",
                reinterpret_cast<int *>(&app->lineStepAnimationMode), 0);
            ImGui::RadioButton(
                "With Polygon Draw",
                reinterpret_cast<int *>(&app->lineStepAnimationMode), 1);
            ImGui::RadioButton(
                "After Polygon Draw",
                reinterpret_cast<int *>(&app->lineStepAnimationMode), 2);
            ImGui::Separator();
          } else {
            // Jika TIDAK ada polygon, paksa mode ke STEP_BEFORE_POLYGON_DRAW
            // (default safe) Agar animation tetap jalan tanpa nunggu polygon
            // yang tidak akan pernah ada
            ofApp::LineStepAnimationMode::STEP_BEFORE_POLYGON_DRAW;
          }
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Custom Line Speed");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80.0f);
        if (ImGui::SliderFloat("##CustomLineSpeed",
                               &app->currentTemplate->customLineSpeedMultiplier,
                               0.1f, 3.0f, "%.2f")) {
          // Update wave speed based on custom line speed
          app->lineWaveSpeed =
              app->currentTemplate->customLineSpeedMultiplier * 2.0f;
        }
      }
    }
  }

  // Polygon CollapsingHeader - hanya muncul jika ada polygon dari file .nay
  bool hasPolygonsInFile = app->loadedFilePolygonCount > 0;

  if (hasPolygonsInFile) {
    if (ImGui::CollapsingHeader("Polygon", ImGuiTreeNodeFlags_DefaultOpen)) {
      // ⭐ NEW: Mode Draw Polygon
      ImGui::Text("Mode Draw");
      if (ImGui::RadioButton("Parallel##PG",
                             reinterpret_cast<int *>(&app->nextPolygonDrawMode),
                             0)) {
        // PG_DRAW_PARALLEL
      }
      ImGui::SameLine();
      if (ImGui::RadioButton("Sequential##PG",
                             reinterpret_cast<int *>(&app->nextPolygonDrawMode),
                             1)) {
        // PG_DRAW_SEQUENTIAL
      }
      ImGui::Separator();

      // Polygon Appearance section
      ImGui::Text("Polygon Appearance");
      if (ImGui::BeginTable("PolygonAnimTable", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthStretch);

        // Row 1: No Animation Polygon | FadeIn Polygon
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::RadioButton("No Animation Polygon", &polygonAnimationMode, 0)) {
        }
        ImGui::TableSetColumnIndex(1);
        if (ImGui::RadioButton("FadeIn Polygon", &polygonAnimationMode, 1)) {
        }

        // Row 2: Wobble Polygon | Wave Fill Polygon
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::RadioButton("Wobble Polygon", &polygonAnimationMode, 2)) {
        }
        ImGui::TableSetColumnIndex(1);
        if (ImGui::RadioButton("Wave Fill Polygon", &polygonAnimationMode, 3)) {
        }

        // Row 3: Wobble Fill Polygon | Gradient Polygon
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::RadioButton("Wobble Fill Polygon", &polygonAnimationMode, 4)) {
        }
        ImGui::TableSetColumnIndex(1);
        if (ImGui::RadioButton("Gradient Polygon", &polygonAnimationMode, 5)) {
        }

        // Row 4: Appearance Speed
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        // Column 0: Label "Appearance Speed" di pojok kanan (right align)
        float labelWidth = ImGui::CalcTextSize("Appearance Speed").x;
        float col0Width = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + col0Width - labelWidth);
        ImGui::Text("Appearance Speed");

        // Column 1: Slider di pojok kiri (left align, default)
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(80.0f);
        ImGui::SliderFloat("##AppearanceSpeed",
                           &app->currentTemplate->polygonSpeedMultiplier, 0.1f,
                           3.0f, "%.2f");

        ImGui::EndTable();
      }

      ImGui::Separator();
      ImGui::Text("Polygon Tessellation");

      // ⭐ Wrap Table in a Scrollable Child Region
      ImGui::BeginChild("PolygonScrollRegion", ImVec2(0, 150), true);

      if (ImGui::BeginTable("PolygonTessellationTable", 2,
                            ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch,
                                0.35f);
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch,
                                0.65f);

        // Tessellation variables now part of the class State
        // (tessellationRadii, tessellationFiles)

        for (int i = 0; i < app->loadedFilePolygonCount; ++i) {
          // Skip polygons that are already tessellated (so they don't get
          // tessellated again)
          if (i < app->polygonShapes.size() &&
              app->polygonShapes[i].isTessellated()) {
            continue;
          }

          ImGui::TableNextRow();

          // Column 0: Polygon Name (Selectable)
          ImGui::TableSetColumnIndex(0);
          std::string polyName = "Polygon " + std::to_string(i);

          // Get selection state from SelectionManager
          bool isSelected = false;
          if (i < app->polygonShapes.size()) {
            isSelected = app->selectionManager.isPolygonSelected(i);
          }

          // We DO NOT sync from app->polygonShapes[i] here anymore because in
          // Sequential Mode, app->polygonShapes is empty until the animation
          // reaches that polygon. Instead, we just let the UI display whatever
          // is already in tessellationFiles/Radii, which was pre-populated by
          // FileOperationManager::loadWorkspaceSeq() immediately upon file
          // open. Ensure UI vectors are large enough
          if (i >= tessellationRadii.size())
            tessellationRadii.resize(i + 100, 10.0f);
          if (i >= tessellationFiles.size())
            tessellationFiles.resize(i + 100, "");

          // Vertically center the text relative to the taller right column
          float extraYOffset = ImGui::GetFrameHeightWithSpacing() -
                               ImGui::GetStyle().ItemSpacing.y;
          ImGui::SetCursorPosY(ImGui::GetCursorPosY() + extraYOffset);

          // Important: Removed ImGuiSelectableFlags_SpanAllColumns so the
          // second column (buttons) can be clicked
          if (ImGui::Selectable(polyName.c_str(), isSelected)) {
            // Check if bounds are correct before selecting (avoiding crash
            // if canvas cleans mid-frame)
            if (i < app->polygonShapes.size()) {
              if (app->isCtrlPressed) {
                // Multi-select if CTRL is held down
                app->selectionManager.togglePolygonSelection(i);
              } else {
                // Exclusive selection
                app->selectionManager.clearAllSelections();
                app->selectionManager.selectPolygon(i);
              }
            }
          }

          // Column 1: Browse Button & Short Radius Slider
          ImGui::TableSetColumnIndex(1);
          ImGui::PushID(i); // Push unique ID to separate controls per row

          // Ensure vectors are large enough
          if (i >= tessellationRadii.size())
            tessellationRadii.resize(i + 100, 10.0f);
          if (i >= tessellationFiles.size())
            tessellationFiles.resize(i + 100, "");

          // Browse Button
          std::string fileNameLabel =
              tessellationFiles[i].empty()
                  ? "-"
                  : ofFilePath::getFileName(tessellationFiles[i]);
          ImGui::TextWrapped("%s", fileNameLabel.c_str());

          if (ImGui::Button("Browse",
                            ImVec2(0, 0))) { // Let ImGui calculate width
            ofFileDialogResult openDialog =
                ofSystemLoadDialog("Open .nay File", false);
            if (openDialog.bSuccess) {
              std::string ext =
                  ofToLower(ofFilePath::getFileExt(openDialog.getPath()));
              if (ext == "nay") {
                tessellationFiles[i] = openDialog.getPath();
              } else {
                app->errorPopup->show("Invalid File",
                                      "Please select a valid .nay file!", "OK");
              }
            }
          }
          if (!tessellationFiles[i].empty() && ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tessellationFiles[i].c_str());
          }

          // Close/Clear File Button
          if (!tessellationFiles[i].empty()) {
            ImGui::SameLine();
            if (ImGui::Button("X", ImVec2(0, 0))) {
              tessellationFiles[i] = "";    // Clear file
              tessellationRadii[i] = 10.0f; // Reset slider to default
            }
            if (ImGui::IsItemHovered()) {
              ImGui::SetTooltip("Remove .nay file");
            }
          }

          // Short Radius Slider
          ImGui::SetNextItemWidth(
              -FLT_MIN); // Fit remaining space, or define fixed like 60.0f
          ImGui::SliderFloat("##Radius", &tessellationRadii[i], 10.0f, 30.0f,
                             "%.1f");

          ImGui::PopID();
        }
        ImGui::EndTable();
      }

      ImGui::EndChild();

      ImGui::Separator();

      // Special Polygon Animation UI ⭐ NEW
      ImGui::Text("Special Polygon Animation");

      ImGui::BeginChild("SpecialPolygonScrollRegion", ImVec2(0, 150), true);

      if (ImGui::BeginTable("SpecialPolygonAnimationTable", 3,
                            ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch,
                                0.30f);
        ImGui::TableSetupColumn("Animation",
                                ImGuiTableColumnFlags_WidthStretch, 0.35f);
        ImGui::TableSetupColumn("Angle",
                                ImGuiTableColumnFlags_WidthStretch, 0.35f);

        for (int i = 0; i < app->loadedFilePolygonCount; ++i) {
          // Skip polygons that are already tessellated
          if (i < static_cast<int>(app->polygonShapes.size()) &&
              app->polygonShapes[i].isTessellated()) {
            continue;
          }

          ImGui::TableNextRow();

          // Column 0: Polygon Name (Selectable)
          ImGui::TableSetColumnIndex(0);
          std::string polyName = "Polygon " + std::to_string(i);

          bool isSelected = false;
          if (i < static_cast<int>(app->polygonShapes.size())) {
            isSelected = app->selectionManager.isPolygonSelected(i);
          }

          float extraYOffset = ImGui::GetFrameHeightWithSpacing() -
                               ImGui::GetStyle().ItemSpacing.y;
          ImGui::SetCursorPosY(ImGui::GetCursorPosY() + extraYOffset);

          if (ImGui::Selectable((polyName + "##Special").c_str(), isSelected)) {
            if (i < static_cast<int>(app->polygonShapes.size())) {
              if (app->isCtrlPressed) {
                app->selectionManager.togglePolygonSelection(i);
              } else {
                app->selectionManager.clearAllSelections();
                app->selectionManager.selectPolygon(i);
              }
            }
          }

          // Column 1: Controls (Radio buttons)
          ImGui::TableSetColumnIndex(1);
          ImGui::PushID(i + 1000); // Push unique ID to separate controls per
                                    // row (offset to avoid collision with
                                    // Tessellation)

          if (i >= static_cast<int>(specialPolygonAnimations.size())) {
            specialPolygonAnimations.resize(
                i + 100, 0); // 0 corresponds to "No Animation" (default)
          }

          ImGui::RadioButton("No Animation", &specialPolygonAnimations[i], 0);
          ImGui::RadioButton("Rotate Left", &specialPolygonAnimations[i], 1);
          ImGui::RadioButton("Rotate Right", &specialPolygonAnimations[i], 2);

          ImGui::PopID();

          // Column 2: Rotate Angle Slider (untuk Rotate Left & Rotate Right) ⭐ NEW
          ImGui::TableSetColumnIndex(2);
          if (specialPolygonAnimations[i] == 1 || specialPolygonAnimations[i] == 2) { // Rotate Left or Right
            ImGui::PushID(i + 2000); // Unique ID untuk slider

            // Initialize vector jika perlu
            if (i >= static_cast<int>(specialPolygonRotateAngles.size())) {
              specialPolygonRotateAngles.resize(i + 100, 45.0f); // Default 45°
            }

            // DragFloat RT: 35 - 360 derajat dengan step 1
            ImGui::SetNextItemWidth(-1); // Full width of column
            ImGui::DragFloat("RT", &specialPolygonRotateAngles[i], 1.0f, 35.0f,
                             360.0f, "%.0f°");

            ImGui::PopID();
          }
        }
        ImGui::EndTable();
      }

      ImGui::EndChild();

      // Special Speed slider (alignment sama seperti Appearance Speed)
      if (ImGui::BeginTable("SpecialSpeedTable", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        // Column 0: Label "Special Speed" di pojok kanan (right align)
        float labelWidth = ImGui::CalcTextSize("Special Speed").x;
        float col0Width = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + col0Width - labelWidth);
        ImGui::Text("Special Speed");

        // Column 1: Slider di pojok kiri (left align, default)
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(80.0f);
        ImGui::SliderFloat("##SpecialSpeed", &specialSpeedMultiplier, 0.1f, 3.0f, "%.2f");

        ImGui::EndTable();
      }

      ImGui::Separator();
    }
  }

  float buttonWidth =
      ImGui::CalcTextSize("Draw").x + ImGui::GetStyle().FramePadding.x * 2.0f;
  float windowWidth = ImGui::GetContentRegionAvail().x;
  ImGui::SetCursorPosX((windowWidth - buttonWidth) / 2.0f);
  // Play arrow button
  if (ImGui::ArrowButton("Draw", ImGuiDir_Left)) {
    // Cek apakah sudah ada file yang di-open
    if (app->lastSavedPath.empty()) {
      // Belum ada file, munculkan error popup
      app->errorPopup->show(
          "No File Selected",
          "Please open a .nay file first before clicking Play!", "OK");
    } else {
      // Semua validasi OK! Cek apakah canvas kosong
      bool isCanvasEmpty =
          (app->currentTemplate && app->currentTemplate->getShapes().empty()) &&
          app->customLines.empty() && app->polygonShapes.empty();

      if (isCanvasEmpty) {
        // Canvas kosong, langsung load saja TANPA clean & TANPA confirmation
        // ⭐ FIX: TAPI TETAP force clear screen untuk hilangkan jejak ImGui!
        app->forceClearScreenCounter = 10;  // 10 frame (0.16 detik)
        app->forceNoTrailsCounter = 10;     // 10 frame lagi (0.16 detik)

        // Apply speed multiplier ke SEMUA (template shapes, polygons,
        // customLines)
        app->currentTemplate->applySpeedMultiplier();
        app->fileManager.setAnimationSpeedMultiplier(
            app->currentTemplate->speedMultiplier);

        // Simpan polygon animation mode - convert int ke PolygonAnimationMode
        PolygonAnimationMode polyMode;
        switch (polygonAnimationMode) {
        case 1:
          polyMode = PolygonAnimationMode::FADE_IN;
          break;
        case 2:
          polyMode = PolygonAnimationMode::WOBBLE;
          break;
        case 3:
          polyMode = PolygonAnimationMode::WAVE_FILL;
          break;
        case 4:
          polyMode = PolygonAnimationMode::WOBBLE_FILL;
          break;
        case 5:
          polyMode = PolygonAnimationMode::GRADIENT;
          break;
        case 0:
        default:
          polyMode = PolygonAnimationMode::NO_ANIMATION;
          break;
        }
        app->fileManager.setPolygonAnimationMode(polyMode);

        // ⭐ NEW: Set Polygon Draw Mode (Parallel vs Sequential)
        bool isParallel = (app->nextPolygonDrawMode == 0);
        app->fileManager.setLoadParallelMode(isParallel);

        // ⭐ NEW: Set Polygon Speed Multiplier
        app->fileManager.setPolygonSpeedMultiplier(
            this->polygonSpeedMultiplier);

        // ⭐ NEW: Set Special Polygon Animation Speed Multiplier
        app->fileManager.setSpecialSpeedMultiplier(
            this->specialSpeedMultiplier);

        // Set flag untuk delay load dan update state
        app->isWaitingForLoad = true;
        app->loadDelayAccumulator = 0.0f;
        app->pendingLoadMode = playMode;
        app->currentState = ofApp::UpdateState::DELAYED_LOAD;

        app->imguiVisible = false; // Hide ImGui
      } else {
        // Canvas ADA isinya, tampilkan confirmation clean + load
        app->confirmationPopup->show(
            "Load Workspace",
            "The canvas will be cleaned before loading.\n\nDo you want to "
            "continue?",
            "Yes, Load", "Cancel", [this, app]() {
              // Callback: User klik Yes, lanjut clean + load workspace

              // CLEAN CANVAS DULU - pakai internal method (tanpa popup lagi)
              app->cleanCanvasInternal();

              // Apply speed multiplier ke SEMUA (template shapes, polygons,
              // customLines)
              app->currentTemplate->applySpeedMultiplier();
              app->fileManager.setAnimationSpeedMultiplier(
                  app->currentTemplate->speedMultiplier);

              // Simpan polygon animation mode - convert int ke
              // PolygonAnimationMode
              PolygonAnimationMode polyMode;
              switch (polygonAnimationMode) {
              case 1:
                polyMode = PolygonAnimationMode::FADE_IN;
                break;
              case 2:
                polyMode = PolygonAnimationMode::WOBBLE;
                break;
              case 3:
                polyMode = PolygonAnimationMode::WAVE_FILL;
                break;
              case 4:
                polyMode = PolygonAnimationMode::WOBBLE_FILL;
                break;
              case 5:
                polyMode = PolygonAnimationMode::GRADIENT;
                break;
              case 0:
              default:
                polyMode = PolygonAnimationMode::NO_ANIMATION;
                break;
              }
              app->fileManager.setPolygonAnimationMode(polyMode);

              // ⭐ NEW: Set Polygon Draw Mode (Parallel vs Sequential)
              bool isParallel = (app->nextPolygonDrawMode == 0);
              app->fileManager.setLoadParallelMode(isParallel);

              // ⭐ NEW: Set Polygon Speed Multiplier
              app->fileManager.setPolygonSpeedMultiplier(
                  this->polygonSpeedMultiplier);

              // ⭐ NEW: Set Special Polygon Animation Speed Multiplier
              app->fileManager.setSpecialSpeedMultiplier(
                  this->specialSpeedMultiplier);

              // Set flag untuk delay load dan update state
              app->isWaitingForLoad = true;
              app->loadDelayAccumulator =
                  0.0f; // Reset accumulator untuk mulai delay
              app->pendingLoadMode = playMode;
              app->currentState = ofApp::UpdateState::DELAYED_LOAD;

              app->imguiVisible = false; // Hide ImGui
            });
      }
    }
  }
  ImGui::SameLine();
  ImGui::Text("Draw");
}
