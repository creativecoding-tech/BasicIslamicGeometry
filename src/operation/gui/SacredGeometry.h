#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class SacredGeometry : public AbstractGuiComponent {
public:
    SacredGeometry(ofApp* app);
    void draw() override;
    void focusWindow();  // Request focus ke SacredGeometry window
    void showWindow();   // Show SacredGeometry window

private:
    ofApp* app;
    bool focusRequested = false;  // Flag untuk request focus pada next frame
    bool windowOpen = true;  // Window open state untuk close button
};
