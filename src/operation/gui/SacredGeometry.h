#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class SacredGeometry : public AbstractGuiComponent {
public:
    SacredGeometry(ofApp* app);
    void draw() override;

private:
    ofApp* app;
};
