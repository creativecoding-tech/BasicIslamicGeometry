#include "SacredGeometry.h"
#include "../../ofApp.h"

SacredGeometry::SacredGeometry(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
void SacredGeometry::focusWindow() {
    focusRequested = true;
}

//--------------------------------------------------------------
void SacredGeometry::showWindow() {
    windowOpen = true;
}

//--------------------------------------------------------------
void SacredGeometry::draw() {
    ImGui::SetNextWindowSize(ImVec2(250, 450), ImGuiCond_FirstUseEver);

    // Focus window jika di-request
    if (focusRequested) {
        ImGui::SetNextWindowFocus();
        focusRequested = false;
    }

    // Begin window dengan close button (windowOpen flag)
    if (ImGui::Begin("Sacred Geometry", &windowOpen, ImGuiWindowFlags_None)) {
        if (app->currentTemplate) {
            // Template Name dengan bold style
            ImGui::Text("Template:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 1.0f), "%s", app->currentTemplate->getName().c_str());

            // Template Description dengan wrap text
            string description = app->currentTemplate->getDescription();
            ImGui::Text("Description:");
            ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
            ImGui::Text("%s", description.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::Separator();
        // Radius slider
        if (app->currentTemplate) {
            ImGui::SetNextItemWidth(150.0f);
            ImGui::SliderFloat("Radius", &app->currentTemplate->radius, 25, 600);
        }
        // Line Width slider
        ImGui::SetNextItemWidth(150.0f);
        if (app->currentTemplate && ImGui::SliderFloat("Line Width", &app->currentTemplate->lineWidth, 0, 4)) {
            app->updateLineWidth();
        }
        ImGui::Separator();
        // Labels & Dots checkbox
        if (app->currentTemplate) {
            if (ImGui::Checkbox("Labels", &app->currentTemplate->labelsVisible)) {
                app->toggleLabels();
            }
            ImGui::SameLine();  // Pindah ke sebelah kanan
            if (ImGui::Checkbox("Dots", &app->currentTemplate->dotsVisible)) {
                app->toggleDots();
            }
        }
        ImGui::Separator();
        // Draw Template
        ImGui::Text("Draw Template");

        // Cek apakah shapes sudah terdraw
        bool isTemplateDrawn = (app->currentTemplate && !app->currentTemplate->getShapes().empty());

        // Button untuk Parallel/Sequential
        if (isTemplateDrawn) {
            // Disabled jika sudah terdraw
            ImGui::BeginDisabled();
            ImGui::Button("Parallel");
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled();
            ImGui::Button("Sequential");
            ImGui::EndDisabled();
        } else {
            // Enabled jika belum terdraw
            if (ImGui::Button("Parallel")) {
                // Draw secara parallel
                if (app->currentTemplate && app->currentTemplate->getShapes().empty()) {
                    // Enable semua draw settings (SacredGeometry INDEPENDENT dari Playground!)
                    app->currentTemplate->enableAllDrawSettings();
                    app->currentTemplate->setupShapes();
                    app->currentTemplate->applySpeedMultiplier();
                    app->currentTemplate->drawParallel();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Sequential")) {
                // Draw secara sequential
                if (app->currentTemplate && app->currentTemplate->getShapes().empty()) {
                    // Enable semua draw settings (SacredGeometry INDEPENDENT dari Playground!)
                    app->currentTemplate->enableAllDrawSettings();
                    app->currentTemplate->setupShapes();
                    app->currentTemplate->applySpeedMultiplier();
                    app->currentTemplate->startSequentialDrawing();
                }
            }
        }
        ImGui::Separator();

        // Transform section
        ImGui::Text("Transform Canvas");
        ImGui::SetNextItemWidth(200.0f);
        if (ImGui::SliderFloat("Pan X", &app->canvasTranslation.x, -500.0f, 500.0f, "%.0f")) {
            // Real-time update saat slider digeser
        }
        ImGui::SetNextItemWidth(200.0f);
        if (ImGui::SliderFloat("Pan Y", &app->canvasTranslation.y, -500.0f, 500.0f, "%.0f")) {
            // Real-time update saat slider digeser
        }
        ImGui::SetNextItemWidth(200.0f);
        if (ImGui::SliderFloat("Rotate", &app->canvasRotation, 0.0f, 360.0f, "%.0f°")) {
            // Real-time update saat slider digeser
        }
        ImGui::SetNextItemWidth(200.0f);
        if (ImGui::SliderFloat("Zoom", &app->canvasZoom, 0.5f, 3.0f, "%.2fx")) {
            // Real-time update saat slider digeser
        }

        // Reset Transform button
        float resetButtonWidth = ImGui::CalcTextSize("Reset Transform").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        float transformWindowWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((transformWindowWidth - resetButtonWidth) / 2.0f);
        if (ImGui::Button("Reset Transform", ImVec2(resetButtonWidth, 0))) {
            app->resetTransform();
        }

        ImGui::Separator();
        // Clean Canvas button - Disable jika canvas kosong
        bool isCanvasEmpty = (app->currentTemplate && app->currentTemplate->getShapes().empty()) &&
                            app->customLines.empty() &&
                            app->polygonShapes.empty();

        float buttonWidth = ImGui::CalcTextSize("Clean Canvas").x + ImGui::GetStyle().FramePadding.x * 2.0f;
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
    app->showSacredGeometry = windowOpen;
}
