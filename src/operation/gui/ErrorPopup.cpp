#include "ErrorPopup.h"
#include "../../ofApp.h"

ErrorPopup::ErrorPopup(ofApp* app)
    : app(app), showPopup(false),
      title("Error"),
      message("An error occurred!"),
      buttonLabel("OK") {}

void ErrorPopup::show(const std::string& title,
                      const std::string& message,
                      const std::string& buttonLabel) {
    this->title = title;
    this->message = message;
    this->buttonLabel = buttonLabel;
    showPopup = true;
}

void ErrorPopup::draw() {
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
        // Text dengan warna merah untuk error
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        ImGui::Text("%s", message.c_str());
        ImGui::PopStyleColor();

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
