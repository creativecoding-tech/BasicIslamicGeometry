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
            if (ImGui::Checkbox("Show Labels", &app->labelsVisible)) {
                app->toggleLabels();
            }
            if (ImGui::Checkbox("Show Dots", &app->dotsVisible)) {
                app->toggleDots();
            }
        }
    }
    ImGui::End();
}
