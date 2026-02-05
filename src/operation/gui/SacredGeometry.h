#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class SacredGeometry : public AbstractGuiComponent {
public:
    SacredGeometry(ofApp* app);
    void draw() override;
    void updateColorFromApp();  // Update color picker dari app->customLineColor
    void updatePolygonColorFromApp();  // Update polygon color picker dari app->polygonColor
    void focusWindow();  // Request focus ke SacredGeometry window
    void showWindow();   // Show SacredGeometry window

private:
    ofApp* app;
    float customLineColor[4]; // RGBA color picker (0.0 - 1.0)
    float polygonColor[4]; // RGBA color picker untuk polygon (0.0 - 1.0)
    bool focusRequested = false;  // Flag untuk request focus pada next frame
    bool windowOpen = true;  // Window open state untuk close button
};
