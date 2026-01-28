#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class LeftPanel : public AbstractGuiComponent {
public:
    LeftPanel(ofApp* app);
    void draw() override;

private:
    ofApp* app;
};
