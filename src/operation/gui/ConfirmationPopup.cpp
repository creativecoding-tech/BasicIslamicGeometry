#include "ConfirmationPopup.h"
#include "../../ofApp.h"

ConfirmationPopup::ConfirmationPopup(ofApp* app)
    : app(app), showPopup(false),
      title("Confirm"),
      message("Are you sure?"),
      confirmButton("Yes"),
      cancelButton("No"),
      onConfirmCallback(nullptr),
      onCancelCallback(nullptr) {}

void ConfirmationPopup::show(const std::string& title,
                            const std::string& message,
                            const std::string& confirmButton,
                            const std::string& cancelButton,
                            ConfirmCallback onConfirm,
                            CancelCallback onCancel) {
    this->title = title;
    this->message = message;
    this->confirmButton = confirmButton;
    this->cancelButton = cancelButton;
    this->onConfirmCallback = onConfirm;
    this->onCancelCallback = onCancel;
    showPopup = true;
}

void ConfirmationPopup::draw() {
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
        // Text dengan warna kuning/orange untuk warning
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.7f, 0.2f, 1.0f));
        ImGui::Text("%s", message.c_str());
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::Spacing();

        // Dua tombol: Yes dan No
        // Hitung lebar tombol berdasarkan text content
        float yesButtonWidth = ImGui::CalcTextSize(confirmButton.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f;
        float noButtonWidth = ImGui::CalcTextSize(cancelButton.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f;
        float buttonSpacing = 10.0f;
        float totalButtonWidth = yesButtonWidth + buttonSpacing + noButtonWidth;
        float windowWidth = ImGui::GetWindowSize().x;
        float offsetX = (windowWidth - totalButtonWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

        // Yes button
        if (ImGui::Button(confirmButton.c_str(), ImVec2(yesButtonWidth, 0))) {
            // Panggil callback jika ada
            if (onConfirmCallback) {
                onConfirmCallback();
            }
            ImGui::CloseCurrentPopup();
        }

        // No button
        ImGui::SameLine(0.0f, buttonSpacing);
        if (ImGui::Button(cancelButton.c_str(), ImVec2(noButtonWidth, 0))) {
            // Panggil callback jika ada
            if (onCancelCallback) {
                onCancelCallback();
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
