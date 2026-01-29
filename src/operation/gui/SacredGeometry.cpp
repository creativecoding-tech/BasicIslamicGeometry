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
                if (!app->sequentialMode) {
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
            ImGui::SetNextItemWidth(150.0f);
            ImGui::SliderFloat("Radius", &app->radiusCircle, 100, 240);
            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::SliderFloat("Line Width", &app->currentLineWidth, 0, 4)) {
                app->updateLineWidth();
            }
            ImGui::Separator();
            if (ImGui::Checkbox("Labels", &app->labelsVisible)) {
                app->toggleLabels();
            }
            ImGui::SameLine();  // Pindah ke sebelah kanan
            if (ImGui::Checkbox("Dots", &app->dotsVisible)) {
                app->toggleDots();
            }
            static bool showCartesian = true;
            if (ImGui::Checkbox("Cartesian", &showCartesian)) {
                if (!app->templateShapes.empty()) {
                    AbstractShape* cartesianAxes =
                        app->templateShapes[0].get(); // CartesianAxes selalu index 0
                    if (showCartesian) {
                        cartesianAxes->show();
                    } else {
                        cartesianAxes->hide();
                    }
                }
            }
            ImGui::Separator();
            if (ImGui::Button("Clean Canvas")) {
                app->cleanCanvas();
            }
        }
    }
    ImGui::End();
}
