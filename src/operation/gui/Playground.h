#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class Playground : public AbstractGuiComponent {
public:
    Playground(ofApp* app);
    void draw() override;

private:
    ofApp* app;
};
