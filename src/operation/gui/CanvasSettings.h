#pragma once

#include "../../imgui/imgui.h"
#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

class CanvasSettings : public AbstractGuiComponent {
public:
  CanvasSettings(ofApp *app);
  virtual void draw() override;

  // Controller flag to open/close this specific window
  bool windowOpen = false;

  void focusWindow(); // Method to focus the window
  void showWindow();  // Method to open the window
  void resetTrails(); // Method to reset trails

private:
  ofApp *app;
  bool focusRequested = false; // Flag for requesting window focus
};
