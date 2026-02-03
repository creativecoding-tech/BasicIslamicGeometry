#include "SacredGeometry.h"
#include "../../ofApp.h"

SacredGeometry::SacredGeometry(ofApp* app) : app(app) {}

void SacredGeometry::draw() {
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);

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
                ImGui::SliderFloat("Radius", &app->currentTemplate->radius, 100, 240);
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
            static bool showCartesian = true;
            if (ImGui::Checkbox("Cartesian", &showCartesian)) {
                if (app->currentTemplate) {
                    const auto& shapes = app->currentTemplate->getShapes();
                    if (!shapes.empty()) {
                        AbstractShape* cartesianAxes =
                            shapes[0].get(); // CartesianAxes selalu index 0
                        if (showCartesian) {
                            cartesianAxes->show();
                        } else {
                            cartesianAxes->hide();
                        }
                    }
                }
            }
            ImGui::Separator();
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
