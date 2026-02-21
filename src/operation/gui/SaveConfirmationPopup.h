#pragma once

#include "AbstractGuiComponent.h"
#include <string>

class ofApp;

class SaveConfirmationPopup : public AbstractGuiComponent {
public:
  SaveConfirmationPopup(ofApp *app);
  void draw() override;

  // Shows the popup and records whether it was triggered by "Save" (false) or
  // "Save As" (true)
  void show(bool isSaveAs = false);

  bool isVisible() const { return showPopup; }

private:
  ofApp *app;
  bool showPopup = false;
  bool needToOpenPopup = false; // Flag trigger popup OpenPopup() di draw()
  bool isSaveAs = false; // Membedakan apakah dipanggil via Save atau Save As
};
