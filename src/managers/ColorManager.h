#pragma once

#include "ofMain.h"
#include "../undo/UndoAction.h"
#include <memory>
#include <set>
#include <vector>

// Forward declarations
class ofApp;

/**
 * ColorManager
 *
 * Manages all color-related state and operations for:
 * - CustomLine colors
 * - Polygon colors (including alpha/transparency)
 * - UserDot colors
 * - Clipboard color (for copy/paste operations)
 *
 * Features:
 * - Centralized color state management
 * - Color reset operations (all/selected)
 * - Copy/paste color operations
 * - Automatic undo/redo integration
 * - Color picker sync from selection
 */
class ColorManager {
public:
	/**
	 * Constructor
	 * @param app Pointer to main ofApp instance
	 */
	ColorManager(ofApp* app);

	/**
	 * Destructor
	 */
	~ColorManager();

	// ========================================================================
	// GETTERS - Retrieve current color state
	// ========================================================================

	/**
	 * Get current customLine color
	 * @return Current color for new/existing customLines
	 */
	ofColor getCustomLineColor() const { return customLineColor; }

	/**
	 * Get current polygon color
	 * @return Current color for new/existing polygons
	 */
	ofColor getPolygonColor() const { return polygonColor; }

	/**
	 * Get current userDot color
	 * @return Current color for new/existing userDots
	 */
	ofColor getUserDotColor() const { return userDotColor; }

	/**
	 * Get clipboard color
	 * @return Color that was copied (for paste operations)
	 */
	ofColor getClipboardColor() const { return clipboardColor; }

	/**
	 * Check if clipboard has color
	 * @return true if there's a color in clipboard
	 */
	bool hasClipboardColor() const { return hasClipboardColorFlag; }

	// ========================================================================
	// SETTERS - Update colors (with undo support)
	// ========================================================================

	/**
	 * Update customLine color
	 * - If there are selected lines: only update selected lines
	 * - If no selection: only update global color (for new lines)
	 * @param color New color to apply
	 */
	void updateCustomLineColor(ofColor color);

	/**
	 * Update polygon color
	 * - If there are selected polygons: only update selected polygons
	 * - If no selection: only update global color (for new polygons)
	 * @param color New color to apply
	 */
	void updatePolygonColor(ofColor color);

	/**
	 * Update polygon alpha (transparency)
	 * - Only affects selected polygons
	 * - RGB components remain unchanged
	 * @param alpha Alpha value (0-255)
	 */
	void updatePolygonAlpha(uint8_t alpha);

	/**
	 * Update userDot color
	 * - If there are selected dots: only update selected dots
	 * - If no selection: only update global color (for new dots)
	 * @param color New color to apply
	 */
	void updateUserDotColor(ofColor color);

	// ========================================================================
	// RESET OPERATIONS
	// ========================================================================

	/**
	 * Reset ALL customLine colors to default blue (0, 0, 255)
	 * - Forces update on ALL existing lines (ignores selection)
	 */
	void resetAllCustomLineColor();

	/**
	 * Reset selected customLine colors to default blue (0, 0, 255)
	 * - Only updates lines that are currently selected
	 */
	void resetSelectedCustomLineColor();

	/**
	 * Reset ALL polygon colors to default blue (0, 0, 255)
	 * - Forces update on ALL existing polygons (ignores selection)
	 */
	void resetAllPolygonColor();

	/**
	 * Reset selected polygon colors to default blue (0, 0, 255)
	 * - Only updates polygons that are currently selected
	 */
	void resetSelectedPolygonColor();

	// ========================================================================
	// COPY/PASTE OPERATIONS
	// ========================================================================

	/**
	 * Copy color from selected userDot to clipboard
	 * Requires exactly 1 selected dot (no mixed selection)
	 */
	void copyDotColor();

	/**
	 * Copy color from selected polygon to clipboard
	 * Requires exactly 1 selected polygon (no mixed selection)
	 */
	void copyPolygonColor();

	/**
	 * Copy color from selected customLine to clipboard
	 * Requires exactly 1 selected line (no mixed selection)
	 */
	void copyLineColor();

	/**
	 * Paste color from clipboard to selected userDots
	 * Requires: dots are selected AND clipboard has color
	 */
	void pasteColorToDot();

	/**
	 * Paste color from clipboard to selected polygons
	 * Requires: polygons are selected AND clipboard has color
	 */
	void pasteColorToPolygon();

	/**
	 * Paste color from clipboard to selected customLines
	 * Requires: lines are selected AND clipboard has color
	 */
	void pasteColorToLine();

	// ========================================================================
	// SYNC OPERATIONS
	// ========================================================================

	/**
	 * Sync color pickers in UserCustom window from selected objects
	 * Priority: userDot > polygon > customLine
	 * - If dot selected: sync dot color
	 * - Else if polygon selected: sync polygon color
	 * - Else if line selected: sync line color
	 * - Else: use global colors
	 */
	void syncColorPickersFromSelection();

private:
	ofApp* app;  // Pointer to main app

	// ========================================================================
	// COLOR STATE
	// ========================================================================

	// Global colors (used for new objects and when nothing is selected)
	ofColor customLineColor = ofColor(0, 0, 255);  // Default blue
	ofColor polygonColor = ofColor(0, 0, 255);      // Default blue
	ofColor userDotColor = ofColor(0, 0, 255);      // Default blue

	// Clipboard state
	ofColor clipboardColor = ofColor(0, 0, 255);    // Default blue
	bool hasClipboardColorFlag = false;             // true if color is copied

	// ========================================================================
	// CONSTANTS
	// ========================================================================

	static const ofColor DEFAULT_COLOR;  // (0, 0, 255) - Default blue color

	// ========================================================================
	// HELPER METHODS
	// ========================================================================

	/**
	 * Push undo action for color change
	 * @param type Type of color change (CHANGE_COLOR_LINE/POLYGON/DOT)
	 * @param newColor New color that was applied
	 */
	void pushUndoAction(int type, ofColor newColor);
};
