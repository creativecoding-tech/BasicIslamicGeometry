#pragma once

#include "AbstractGuiComponent.h"
#include <string>

// Forward declaration
class ofApp;

class SuccessPopup : public AbstractGuiComponent {
public:
    SuccessPopup(ofApp* app);
    void draw() override;
    void show(const std::string& title = "Save Success",
              const std::string& message = "File saved successfully!",
              const std::string& buttonLabel = "OK");

private:
    ofApp* app;
    bool showPopup = false;
    std::string title;
    std::string message;
    std::string buttonLabel;
};
