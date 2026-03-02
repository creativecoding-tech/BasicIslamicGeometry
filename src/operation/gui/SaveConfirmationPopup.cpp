#include "SaveConfirmationPopup.h"
#include "../../ofApp.h"
#include "imgui.h"

SaveConfirmationPopup::SaveConfirmationPopup(ofApp *app) : app(app) {}

void SaveConfirmationPopup::show(bool isSaveAs) {
  showPopup = true;
  needToOpenPopup = true;
  this->isSaveAs = isSaveAs;
}

void SaveConfirmationPopup::draw() {
  if (!showPopup)
    return;

  if (needToOpenPopup) {
    ImGui::OpenPopup("Save Tessellated Geometry?");
    needToOpenPopup = false;
  }

  // Center the popup
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_Appearing);

  if (ImGui::BeginPopupModal("Save Tessellated Geometry?", &showPopup,
                             ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoSavedSettings)) {
    ImGui::TextWrapped(
        "Tessellated geometry was detected on the canvas.\nDo you want to "
        "include the tessellated shapes in the save file?");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    // Center buttons
    float buttonWidth = 80;
    float spacing = 20;
    float totalWidth = (buttonWidth * 3) + (spacing * 2);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - totalWidth) / 2.0f);

    if (ImGui::Button("Yes", ImVec2(buttonWidth, 0))) {
      showPopup = false;
      ImGui::CloseCurrentPopup();

      // Lanjut save NORMAL (termasuk teselasi)
      if (isSaveAs) {
        app->fileOperationManager->executeSaveWorkspaceAs(
            false); // false = jangan filter
      } else {
        app->fileOperationManager->executeSaveWorkspace(false);
      }
    }
    ImGui::SameLine(0, spacing);

    if (ImGui::Button("No", ImVec2(buttonWidth, 0))) {
      showPopup = false;
      ImGui::CloseCurrentPopup();

      // Lanjut save FILTERED (tanpa teselasi)
      if (isSaveAs) {
        app->fileOperationManager->executeSaveWorkspaceAs(
            true); // true = filter teselasi
      } else {
        app->fileOperationManager->executeSaveWorkspace(true);
      }
    }
    ImGui::SameLine(0, spacing);

    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
      showPopup = false;
      ImGui::CloseCurrentPopup();
      // Do nothing, abort save
    }

    ImGui::EndPopup();
  }
}
