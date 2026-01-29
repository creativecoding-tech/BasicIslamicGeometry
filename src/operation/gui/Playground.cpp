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
        static int playMode = -1;
        if (ImGui::RadioButton("Parallel Per Group", &playMode, 0)) {

        }

        if (ImGui::RadioButton("Sequential Per Group", &playMode, 1)) {

        }
        ImGui::Separator();

        // Play arrow button
        if (ImGui::ArrowButton("Play", ImGuiDir_Left)) {
            // Cek apakah sudah ada file yang di-open
            if (app->lastOpenedFileName.empty()) {
                // Belum ada file, munculkan error popup
                app->errorPopup->show("No File Selected",
                                     "Please open a .nay file first before clicking Play!",
                                     "OK");
            } else {
                // Event handler play
                switch (playMode) {
                case 0:
                    app->loadWorkspace();
                        break;
                case 1:
                    app->loadWorkspaceSeq();
                    break;
                }
            }
        }
        ImGui::SameLine();
        ImGui::Text("Play");

    }
    ImGui::End();
}
