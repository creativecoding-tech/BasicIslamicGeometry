#include "LeftPanel.h"
#include "../../ofApp.h"

LeftPanel::LeftPanel(ofApp* app) : app(app) {}

void LeftPanel::draw() {
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Sacred Geometry", nullptr, ImGuiWindowFlags_None)) {
        ImGui::Text("Template: %s", app->currentTemplate ? app->currentTemplate->getName().c_str() : "None");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Settings")) {
            ImGui::SliderFloat("Radius", &app->radiusCircle, 100, 500);
            ImGui::SliderFloat("Line Width", &app->currentLineWidth, 1, 10);
        }

        if (ImGui::CollapsingHeader("Display")) {
            ImGui::Checkbox("Show Labels", &app->labelsVisible);
            ImGui::Checkbox("Show Dots", &app->dotsVisible);
        }

        if (ImGui::CollapsingHeader("Stats")) {
            ImGui::Text("Custom Lines: %zu", app->customLines.size());
            ImGui::Text("Polygons: %zu", app->polygonShapes.size());
        }
    }
    ImGui::End();
}
