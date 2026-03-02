#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class CanvasSettings : public AbstractGuiComponent {
public:
  CanvasSettings(ofApp *app);
  void draw() override;
  void focusWindow();  // Request focus ke CanvasSettings window
  void showWindow();   // Show CanvasSettings window
  void resetTrails();  // Reset trails ke default
  void resetColors();  // Reset colors ke default

private:
  ofApp *app;
  bool focusRequested = false;  // Flag untuk request focus pada next frame
  bool windowOpen = true;  // Window open state untuk close button
};
