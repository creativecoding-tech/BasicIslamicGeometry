#pragma once

#include "AbstractGuiComponent.h"
#include <string>

// Forward declaration
class ofApp;

class ErrorPopup : public AbstractGuiComponent {
public:
    ErrorPopup(ofApp* app);
    void draw() override;
    void show(const std::string& title = "Error",
              const std::string& message = "An error occurred!",
              const std::string& buttonLabel = "OK");

private:
    ofApp* app;
    bool showPopup = false;
    std::string title;
    std::string message;
    std::string buttonLabel;
};
