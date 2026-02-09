#pragma once

#include "ofMain.h"

class AbstractGuiComponent {
public:
    virtual ~AbstractGuiComponent() = default;
    virtual void draw() = 0;  // Pure virtual - WAJIB override
};
