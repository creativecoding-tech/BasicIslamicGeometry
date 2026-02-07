#pragma once

#include "AbstractGuiComponent.h"
#include <string>
#include <functional>

// Forward declaration
class ofApp;

class ConfirmationPopup : public AbstractGuiComponent {
public:
    // Callback function type untuk action saat user klik Yes
    using ConfirmCallback = std::function<void()>;

    ConfirmationPopup(ofApp* app);
    void draw() override;

    // Show popup dengan callback yang akan dipanggil saat user klik Yes
    void show(const std::string& title = "Confirm",
              const std::string& message = "Are you sure?",
              const std::string& confirmButton = "Yes",
              const std::string& cancelButton = "No",
              ConfirmCallback onConfirm = nullptr);

    bool isVisible() const { return showPopup; }

private:
    ofApp* app;
    bool showPopup = false;
    std::string title;
    std::string message;
    std::string confirmButton;
    std::string cancelButton;
    ConfirmCallback onConfirmCallback;  // Callback saat user klik Yes
};
