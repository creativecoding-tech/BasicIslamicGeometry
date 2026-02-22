#include "CanvasSettings.h"
#include "../../ofApp.h"

CanvasSettings::CanvasSettings(ofApp *app) : app(app) {}

//--------------------------------------------------------------
void CanvasSettings::focusWindow() { focusRequested = true; }

//--------------------------------------------------------------
void CanvasSettings::showWindow() { windowOpen = true; }

//--------------------------------------------------------------
void CanvasSettings::resetTrails() {
  app->trailMode = 1;
  app->trailsValue = 25;
}

//--------------------------------------------------------------
void CanvasSettings::resetColors() {
  app->canvasBgColor[0] = 1.0f;
  app->canvasBgColor[1] = 1.0f;
  app->canvasBgColor[2] = 1.0f;
  app->canvasBgColor[3] = 1.0f;

  app->canvasGradientColor[0] = 1.0f;
  app->canvasGradientColor[1] = 1.0f;
  app->canvasGradientColor[2] = 1.0f;
  app->canvasGradientColor[3] = 1.0f;

  app->useCanvasGradient = false;
}

//--------------------------------------------------------------
void CanvasSettings::draw() {
  ImGui::SetNextWindowSize(ImVec2(250, 550), ImGuiCond_FirstUseEver);

  // Focus window jika di-request
  if (focusRequested) {
    ImGui::SetNextWindowFocus();
    focusRequested = false;
  }

  // Begin window dengan close button (windowOpen flag)
  if (ImGui::Begin("Canvas Settings", &windowOpen, ImGuiWindowFlags_None)) {
    // -------------------------
    // Color Settings Section
    // -------------------------
    ImGui::Text("Color Settings");

    // Background Color Picker
    ImGui::ColorEdit4("Base Color", app->canvasBgColor,
                      ImGuiColorEditFlags_NoInputs |
                          ImGuiColorEditFlags_AlphaBar);

    // Gradient Checkbox
    ImGui::Checkbox("Use Gradient", &app->useCanvasGradient);

    // Gradient Color Picker (hanya aktif/muncul jika gradient dicentang)
    if (app->useCanvasGradient) {
      ImGui::ColorEdit4("Gradient Color", app->canvasGradientColor,
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_AlphaBar);
    }

    // Reset Color Button centered
    ImGui::Spacing();
    float resetColorButtonWidth = ImGui::CalcTextSize("Reset Color").x +
                                  ImGui::GetStyle().FramePadding.x * 2.0f;
    float resetColorWindowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX((resetColorWindowWidth - resetColorButtonWidth) /
                         2.0f);
    if (ImGui::Button("Reset Color")) {
      resetColors();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // -------------------------
    // Trails Settings Section
    // -------------------------
    ImGui::Text("Trails Settings");

    // Table tanpa border untuk layout 2 kolom
    if (ImGui::BeginTable("TrailsTable", 2, ImGuiTableFlags_None)) {
      // Baris Pertama
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::RadioButton("No Trails", &app->trailMode, 0);

      // Baris Kedua
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::RadioButton("Use Trails", &app->trailMode, 1);

      // Kolom 2 pada Baris Kedua: Slider (hanya muncul kalau Use Trails = 1)
      ImGui::TableSetColumnIndex(1);
      if (app->trailMode == 1) {
        // Slider lebih pendek dan disembunyikan label value-nya pakai ##
        ImGui::SetNextItemWidth(100.0f);
        ImGui::SliderInt("##Value", &app->trailsValue, 20, 100);
      }

      ImGui::EndTable();
    }

    // Tombol Reset Trails di posisi tengah
    float resetTrailsButtonWidth = ImGui::CalcTextSize("Reset Trails").x +
                                   ImGui::GetStyle().FramePadding.x * 2.0f;
    float trailsWindowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX((trailsWindowWidth - resetTrailsButtonWidth) / 2.0f);
    if (ImGui::Button("Reset Trails", ImVec2(resetTrailsButtonWidth, 0))) {
      resetTrails();
    }

    ImGui::Separator();

    // Transform section
    ImGui::Text("Transform Canvas");
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::SliderFloat("Pan X", &app->canvasTranslation.x, -500.0f, 500.0f,
                           "%.0f")) {
      // Real-time update saat slider digeser
    }
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::SliderFloat("Pan Y", &app->canvasTranslation.y, -500.0f, 500.0f,
                           "%.0f")) {
      // Real-time update saat slider digeser
    }
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::SliderFloat("Rotate", &app->canvasRotation, 0.0f, 360.0f,
                           "%.0f°")) {
      // Real-time update saat slider digeser
    }
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::SliderFloat("Zoom", &app->canvasZoom, 0.5f, 3.0f, "%.2fx")) {
      // Real-time update saat slider digeser
    }

    // Reset Transform button
    float resetButtonWidth = ImGui::CalcTextSize("Reset Transform").x +
                             ImGui::GetStyle().FramePadding.x * 2.0f;
    float transformWindowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX((transformWindowWidth - resetButtonWidth) / 2.0f);
    if (ImGui::Button("Reset Transform", ImVec2(resetButtonWidth, 0))) {
      app->resetTransform();
    }

    ImGui::Separator();

    // Clean Canvas button - Disable jika canvas kosong
    bool isCanvasEmpty =
        (app->currentTemplate && app->currentTemplate->getShapes().empty()) &&
        app->customLines.empty() && app->polygonShapes.empty();

    float buttonWidth = ImGui::CalcTextSize("Clean Canvas").x +
                        ImGui::GetStyle().FramePadding.x * 2.0f;
    float windowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX((windowWidth - buttonWidth) / 2.0f);

    if (isCanvasEmpty) {
      // Disable tombol jika canvas kosong
      ImGui::BeginDisabled();
      ImGui::Button("Clean Canvas");
      ImGui::EndDisabled();
    } else {
      if (ImGui::Button("Clean Canvas")) {
        // cleanCanvas() sudah otomatis show confirmation popup
        app->cleanCanvas();
      }
    }
  }
  ImGui::End();

  // Sync window open state ke app
  app->showCanvasSettings = windowOpen;
}
