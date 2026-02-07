#pragma once

#include "ofMain.h"
#include "../../ofApp.h"
#include <string>

using glm::vec2;

class SelectionInfo {
public:
	SelectionInfo(ofApp* app);
	virtual ~SelectionInfo();

	void draw();           // Draw Selection Info window
	void showWindow();     // Show window
	void focusWindow();    // Focus window

private:
	ofApp* app;            // Pointer ke main app

	bool windowOpen = false;   // Window open state
	bool focusRequested = false;  // Focus request flag
};
