#pragma once

#include "AbstractGuiComponent.h"
#include <ofMain.h>

class ofApp;

class UserCustom : public AbstractGuiComponent {
public:
    UserCustom(ofApp* app);
    virtual ~UserCustom() = default;

    void draw() override;
    void showWindow();
    void focusWindow();
    void updateColorFromApp();  // Update color picker dari app->customLineColor
    void updatePolygonColorFromApp();  // Update polygon color picker dari app->polygonColor
    void updateUserDotColorFromApp();  // Update userDot color picker dari app->userDotColor
    void syncUserDotFromSelection();  // Sync radius & color dari selected userDot
    void syncLineColorFromSelection();  // Sync color dari selected customLine
    void syncPolygonColorFromSelection();  // Sync color dari selected polygon

    bool isOpen() const { return windowOpen; }

private:
    ofApp* app;
    bool windowOpen = true;
    bool focusRequested = false;

    // Color picker values (0-1 range untuk ImGui)
    float customLineColor[4];  // R, G, B, A
    float polygonColor[4];      // R, G, B, A
    float userDotColor[4];      // R, G, B, A untuk userDot
};
