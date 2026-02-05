#include "SacredGeometry.h"
#include "../../ofApp.h"

SacredGeometry::SacredGeometry(ofApp* app) : app(app) {
    // Initialize color picker dari app->customLineColor (default biru)
    customLineColor[0] = app->customLineColor.r / 255.0f; // R
    customLineColor[1] = app->customLineColor.g / 255.0f; // G
    customLineColor[2] = app->customLineColor.b / 255.0f; // B
    customLineColor[3] = app->customLineColor.a / 255.0f; // A

    // Initialize color picker dari app->polygonColor (default biru)
    polygonColor[0] = app->polygonColor.r / 255.0f; // R
    polygonColor[1] = app->polygonColor.g / 255.0f; // G
    polygonColor[2] = app->polygonColor.b / 255.0f; // B
    polygonColor[3] = app->polygonColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void SacredGeometry::updateColorFromApp() {
    // Update color picker values dari app->customLineColor
    customLineColor[0] = app->customLineColor.r / 255.0f; // R
    customLineColor[1] = app->customLineColor.g / 255.0f; // G
    customLineColor[2] = app->customLineColor.b / 255.0f; // B
    customLineColor[3] = app->customLineColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void SacredGeometry::updatePolygonColorFromApp() {
    // Update polygon color picker values dari app->polygonColor
    polygonColor[0] = app->polygonColor.r / 255.0f; // R
    polygonColor[1] = app->polygonColor.g / 255.0f; // G
    polygonColor[2] = app->polygonColor.b / 255.0f; // B
    polygonColor[3] = app->polygonColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void SacredGeometry::draw() {
    ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Sacred Geometry", nullptr, ImGuiWindowFlags_None)) {
        string templateName = app->currentTemplate ? app->currentTemplate->getName() : string("None");
        if (ImGui::CollapsingHeader(templateName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Show/Hide Template");
            // Radio button untuk Parallel/Sequential
            static int drawMode = -1;  // 0 = Parallel, 1 = Sequential
            if(ImGui::RadioButton("Parallel", &drawMode, 0)) {
                if (app->currentTemplate && !app->currentTemplate->sequentialMode) {
                    app->showAllShapes();
                }
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Sequential", &drawMode, 1)) {
                app->startSequentialDrawing();
            }
            if (ImGui::RadioButton("Hide", &drawMode, 2)) {
                if (!app->isStaggeredLoad) app->hideAllShapes();
            }
            ImGui::Separator();
            if (app->currentTemplate) {
                ImGui::SetNextItemWidth(150.0f);
                ImGui::SliderFloat("Radius", &app->currentTemplate->radius, 50, 240);
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
            ImGui::Text("Custom Line");
            // Color picker untuk custom line (bentuk melingkar)
            if (ImGui::ColorPicker4("Line Color", customLineColor,
                ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar)) {
                // Event handler: warna berubah, update semua customLines
                ofColor newColor(
                    customLineColor[0] * 255,
                    customLineColor[1] * 255,
                    customLineColor[2] * 255,
                    customLineColor[3] * 255
                );
                app->updateCustomLineColor(newColor);
            }
            ImGui::Separator();
            ImGui::Text("Polygon");
            // Color picker untuk polygon (bentuk melingkar)
            if (ImGui::ColorPicker4("Polygon Color", polygonColor,
                ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar)) {
                // Event handler: warna berubah, update semua polygons
                ofColor newColor(
                    polygonColor[0] * 255,
                    polygonColor[1] * 255,
                    polygonColor[2] * 255,
                    polygonColor[3] * 255
                );
                app->updatePolygonColor(newColor);
            }

            ImGui::Separator();
            // Center the Clean Canvas button
            float buttonWidth = ImGui::CalcTextSize("Clean Canvas").x + ImGui::GetStyle().FramePadding.x * 2.0f;
            float windowWidth = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX((windowWidth - buttonWidth) / 2.0f);
            if (ImGui::Button("Clean Canvas")) {
                app->cleanCanvas();
            }

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
}
