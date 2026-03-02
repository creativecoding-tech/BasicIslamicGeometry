#include "Playground.h"
#include "../../ofApp.h"

Playground::Playground(ofApp *app) : app(app) {}

//--------------------------------------------------------------
void Playground::focusWindow() { focusRequested = true; }

//--------------------------------------------------------------
void Playground::showWindow() { windowOpen = true; }

//--------------------------------------------------------------
void Playground::draw() {
  // Simpan state windowOpen sebelum ImGui::Begin()
  bool wasWindowOpen = windowOpen;

  // Set posisi window di sebelah kanan canvas
  ImGui::SetNextWindowPos(ImVec2(ofGetWidth() - 260, 30),
                          ImGuiCond_FirstUseEver);
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

      // Close File button - hanya enabled jika ada file yang di-open
      ImGui::Spacing();
      float closeButtonWidth = ImGui::CalcTextSize("Close File").x +
                               ImGui::GetStyle().FramePadding.x * 2.0f;
      float windowWidth = ImGui::GetContentRegionAvail().x;
      ImGui::SetCursorPosX((windowWidth - closeButtonWidth) / 2.0f);

      if (ImGui::Button("Close File")) {
        // Close file tanpa clean canvas
        app->fileOperationManager->closeFile();
        windowOpen = false; // Close window otimatis
      }
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

  // Cek apakah user baru saja menutup window (klik tombol X)
  if (wasWindowOpen && !windowOpen) {
    // Window baru saja ditutup, cek apakah ada file yang terbuka
    if (!app->lastSavedPath.empty()) {
      // Ada file yang terbuka, tampilkan confirmation
      app->confirmationPopup->show(
          "Close File",
          "Do you want to close the opened file?\n\nThe canvas will remain "
          "intact.",
          "Yes, Close File", "No, Keep File Open",
          [this]() {
            // Callback: User klik Yes, Close File
            app->fileOperationManager->closeFile();
            app->showPlayground = false;
            windowOpen = false;
          },
          [this]() {
            // Callback: User klik No, Keep File Open
            // Window tertutup, tapi file tetap aktif (app->lastSavedPath tidak
            // di-clear)
            app->showPlayground = false;
            windowOpen = false;
          });
    } else {
      // Tidak ada file yang terbuka, biarkan window tutup
      app->showPlayground = false;
    }
  } else {
    // Sync window open state ke app
    app->showPlayground = windowOpen;
  }
}
