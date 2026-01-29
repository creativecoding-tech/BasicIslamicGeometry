#include "Playground.h"
#include "../../ofApp.h"

Playground::Playground(ofApp* app) : app(app) {}

void Playground::draw() {
    // Set posisi window di sebelah kanan canvas
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() - 260, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Playground", nullptr, ImGuiWindowFlags_None)) {
        ImGui::Text("Opened File:");
        ImGui::Separator();

        // Tampilkan nama file yang terakhir dibuka
        if (!app->lastOpenedFileName.empty()) {
            ImGui::Text("%s", app->lastOpenedFileName.c_str());
        } else {
            ImGui::TextDisabled("(No file opened)");
        }

        ImGui::Separator();

        // Play arrow button
        if (ImGui::ArrowButton("Play", ImGuiDir_Left)) {
            // Event handler nanti
        }
        ImGui::SameLine();
        ImGui::Text("Play");

    }
    ImGui::End();
}
