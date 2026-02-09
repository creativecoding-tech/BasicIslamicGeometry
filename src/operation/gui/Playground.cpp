#include "Playground.h"
#include "../../ofApp.h"

Playground::Playground(ofApp* app) : app(app) {}

//--------------------------------------------------------------
void Playground::focusWindow() {
    focusRequested = true;
}

//--------------------------------------------------------------
void Playground::showWindow() {
    windowOpen = true;
}

//--------------------------------------------------------------
void Playground::draw() {
    // Set posisi window di sebelah kanan canvas
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() - 260, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);

    // Focus window jika di-request
    if (focusRequested) {
        ImGui::SetNextWindowFocus();
        focusRequested = false;
    }

    // Begin window dengan close button (windowOpen flag)
    if (ImGui::Begin("Playground", &windowOpen, ImGuiWindowFlags_None)) {
        ImGui::Text("Opened File:");
        ImGui::Separator();

        // Tampilkan nama file yang terakhir dibuka (extract dari lastSavedPath)
        if (!app->lastSavedPath.empty()) {
            // Extract filename dari full path
            size_t lastSlash = app->lastSavedPath.find_last_of("/\\");
            std::string filename = (lastSlash != std::string::npos)
                ? app->lastSavedPath.substr(lastSlash + 1)
                : app->lastSavedPath;
            ImGui::Text("%s", filename.c_str());
        } else {
            ImGui::TextDisabled("(No file opened)");
        }

        ImGui::Separator();

        // Template-specific playback UI
        if (app->currentTemplate && app->currentTemplate->hasPlaybackSettings()) {
            app->currentTemplate->showPlaybackUI(app);
        } else {
            ImGui::TextDisabled("No playground");
        }

    }
    ImGui::End();

    // Sync window open state ke app
    app->showPlayground = windowOpen;
}
