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
    int playMode = -1;  // -1 = belum pilih, 0 = Parallel, 1 = Sequential
    bool autoCleanCanvas = false;
};
