#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class Playground : public AbstractGuiComponent {
public:
    Playground(ofApp* app);
    void draw() override;
    void focusWindow();  // Request focus ke Playground window
    void showWindow();   // Show Playground window

private:
    ofApp* app;
    bool focusRequested = false;  // Flag untuk request focus pada next frame
    bool windowOpen = true;  // Window open state untuk close button
};
