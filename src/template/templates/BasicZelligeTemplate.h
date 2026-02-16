#pragma once

#include "../SacredGeometryTemplate.h"

/**
 * BasicZelligeTemplate - Moroccan Islamic geometric pattern (Basic version)
 *
 * Pattern ini terdiri dari:
 * - 5 Circles (A, B, C, D, E) - 1 center + 4 cardinal directions
 * - Cartesian Axes - X dan Y axis
 * - 4 Cross Lines (F, G, H, I) - Diagonal lines
 * - 4 Parallelogram Lines - Connecting outer circles
 * - 4 Rectangle Lines - Intersecting diagonal lines
 * - 8 Octagram Lines - 8-pointed star pattern
 *
 * Total: 26 shapes dengan kompleksitas geometri tinggi
 *
 * Note: Ini adalah "Basic" version. Nanti bisa ada ZelligeTemplate (advanced)
 *       atau variations lainnya.
 */
class BasicZelligeTemplate : public SacredGeometryTemplate {
public:
  std::string getName() override;
  std::string getDescription() override;

  // Setup shapes ke internal vector (inherited from SacredGeometryTemplate)
  void setupShapes() override;

  // Template-specific UI
  bool hasCustomSettings() override;
  void showSettingsUI() override;

  // Wave Animation Helper ⭐ NEW
  void applyWaveAnimationToAllCustomLines(class ofApp *app,
                                          bool enableAnimation = true);
  void applyWaveAnimationToCustomLine(class ofApp *app, class CustomLine *line,
                                      bool enableAnimation = true);

  // Template-specific playback UI
  bool hasPlaybackSettings() override;
  void showPlaybackUI(class ofApp *app) override;

  // radius diambil dari SacredGeometryTemplate::radius (protected)
  // Tidak perlu radius sendiri lagi

private:
  // Setup methods populate this->shapes (inherited protected member)
  void setupCircles();
  void setupCartesianAxes();
  void setupCrossLines();
  void setupParallelograms();
  void setupRectangleLines();
  void setupOctagramLines();

  // Playback state
  int playMode = 0;                  // 0 = Parallel (DEFAULT), 1 = Sequential
  int polygonAnimationMode = 0;      // 0=None, 1=FadeIn, 2=Wobble, 3=Fill
  bool showPlaybackSettings = false; // Control visibility of playback UI

public:
  // Reset draw mode ke default (Parallel)
  void resetDrawMode() {
    playMode = 0;
    showPlaybackSettings = false;
  }

  // Enable playback settings (dipanggil saat file dibuka)
  void enablePlaybackSettings() { showPlaybackSettings = true; }
};
