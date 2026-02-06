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
        string templateName = app->currentTemplate ? app->currentTemplate->getName() : string("None");
        if (ImGui::CollapsingHeader(templateName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Draw Template");
            // Radio button untuk Parallel/Sequential
            static int drawMode = -1;  // 0 = Parallel, 1 = Sequential
            if(ImGui::RadioButton("Parallel", &drawMode, 0)) {
                // Draw secara parallel
                if (app->currentTemplate && app->currentTemplate->getShapes().empty()) {
                    app->currentTemplate->setupShapes();
                    app->currentTemplate->applySpeedMultiplier();
                    app->currentTemplate->drawParallel();
                }
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Sequential", &drawMode, 1)) {
                // Draw secara sequential
                if (app->currentTemplate && app->currentTemplate->getShapes().empty()) {
                    app->currentTemplate->setupShapes();
                    app->currentTemplate->applySpeedMultiplier();
                    app->startSequentialDrawing();
                }
            }
            ImGui::Separator();
            if (app->currentTemplate) {
                ImGui::SetNextItemWidth(150.0f);
                ImGui::SliderFloat("Radius", &app->currentTemplate->radius, 50, 600);
            }
            ImGui::SetNextItemWidth(150.0f);
            if (app->currentTemplate && ImGui::SliderFloat("Line Width", &app->currentTemplate->lineWidth, 0, 4)) {
                app->updateLineWidth();
            }
            ImGui::Separator();
            if (app->currentTemplate) {
                if (ImGui::Checkbox("Labels", &app->currentTemplate->labelsVisible)) {
                    app->toggleLabels();
                }
                ImGui::SameLine();  // Pindah ke sebelah kanan
                if (ImGui::Checkbox("Dots", &app->currentTemplate->dotsVisible)) {
                    app->toggleDots();
                }
            }
            bool showCartesian = app->currentTemplate ? app->currentTemplate->showCartesianInSacredGeometry : true;
            if (ImGui::Checkbox("Cartesian", &showCartesian)) {
                app->currentTemplate->showCartesianInSacredGeometry = showCartesian;
                app->setCartesianAxesVisibility(showCartesian);
            }
            ImGui::Separator();
            // Clean Canvas button
            float buttonWidth = ImGui::CalcTextSize("Clean Canvas").x + ImGui::GetStyle().FramePadding.x * 2.0f;
            float windowWidth = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX((windowWidth - buttonWidth) / 2.0f);

            if (ImGui::Button("Clean Canvas")) {
                app->cleanCanvas();
            }
            ImGui::Separator();
            // Template-Specific Settings
            if (app->currentTemplate && app->currentTemplate->hasCustomSettings()) {
                ImGui::Separator();
                if (ImGui::CollapsingHeader("Template-Specific", ImGuiTreeNodeFlags_DefaultOpen)) {
                    app->currentTemplate->showSettingsUI();
                }
            }
        }
    }
    ImGui::End();

    // Sync window open state ke app
    app->showSacredGeometry = windowOpen;
}
