#pragma once

#include "AbstractGuiComponent.h"

// Forward declaration
class ofApp;

/**
 * ObjectTooltip - Manager untuk semua object tooltips
 * Handle tooltips untuk: userDot, customLine, polygon
 * Extends AbstractGuiComponent untuk consistency dengan GUI components
 */
class ObjectTooltip : public AbstractGuiComponent {
public:
	ObjectTooltip(ofApp* app);
	void draw() override;  // Override draw dari AbstractGuiComponent - cek semua hover state dan tampilkan tooltip

private:
	ofApp* app;

	// Tooltip handlers per object type
	void drawUserDotTooltip(int index);
	void drawCustomLineTooltip(int index);
	void drawPolygonTooltip(int index);
};
