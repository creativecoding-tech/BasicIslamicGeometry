#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class MenuBar : public AbstractGuiComponent {
public:
    MenuBar(ofApp* app);
    void draw() override;

private:
    ofApp* app;
};
