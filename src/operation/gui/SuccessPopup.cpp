#include "SuccessPopup.h"
#include "../../ofApp.h"

SuccessPopup::SuccessPopup(ofApp* app)
    : app(app), showPopup(false),
      title("Save Success"),
      message("File saved successfully!"),
      buttonLabel("OK") {}

void SuccessPopup::show(const std::string& title,
                        const std::string& message,
                        const std::string& buttonLabel) {
    this->title = title;
    this->message = message;
    this->buttonLabel = buttonLabel;
    showPopup = true;
}

void SuccessPopup::draw() {
    // Open popup di frame pertama
    if (showPopup) {
        // Set posisi popup ke tengah layar
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::OpenPopup(title.c_str());
        showPopup = false;
    }

    // Render popup setiap frame (setelah open)
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", message.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        // Center tombol
        float buttonWidth = 120.0f;
        float windowWidth = ImGui::GetWindowSize().x;
        float offsetX = (windowWidth - buttonWidth) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

        if (ImGui::Button(buttonLabel.c_str(), ImVec2(buttonWidth, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
