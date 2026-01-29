#include "Playground.h"
#include "../../ofApp.h"

Playground::Playground(ofApp* app) : app(app) {}

void Playground::draw() {
    // Set posisi window di sebelah kanan canvas
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() - 260, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Playground", nullptr, ImGuiWindowFlags_None)) {
        ImGui::Text("");
        ImGui::Separator();

    }
    ImGui::End();
}
