#pragma once

#include "ofMain.h"
#include <memory>
#include <vector>

// Forward declarations
class ofApp;
struct UndoAction;

using glm::vec2;

/**
 * DuplicateManager
 *
 * Manages all duplicate operations for:
 * - UserDot duplication (4 directions: above, below, left, right)
 * - CustomLine duplication (rotate 180°)
 *
 * Features:
 * - Centralized duplicate logic
 * - Offset distance configuration
 * - Automatic undo/redo integration
 * - Helper methods for dot creation
 */
class DuplicateManager {
public:
	/**
	 * Constructor
	 * @param app Pointer to main ofApp instance
	 */
	DuplicateManager(ofApp* app);

	/**
	 * Destructor
	 */
	~DuplicateManager();

	// ========================================================================
	// GETTERS - Retrieve configuration
	// ========================================================================

	/**
	 * Get current offset distance for dot duplication
	 * @return Current offset distance in pixels
	 */
	float getOffsetDistance() const { return offsetDistance; }

	// ========================================================================
	// SETTERS - Update configuration
	// ========================================================================

	/**
	 * Set offset distance for dot duplication
	 * @param distance New offset distance in pixels
	 */
	void setOffsetDistance(float distance);

	// ========================================================================
	// DOT DUPLICATION OPERATIONS
	// ========================================================================

	/**
	 * Duplicate dot upward (Y negative)
	 * Offset: (0, -offsetDistance)
	 */
	void duplicateDotAbove();

	/**
	 * Duplicate dot downward (Y positive)
	 * Offset: (0, offsetDistance)
	 */
	void duplicateDotBelow();

	/**
	 * Duplicate dot leftward (X negative)
	 * Offset: (-offsetDistance, 0)
	 */
	void duplicateDotLeft();

	/**
	 * Duplicate dot rightward (X positive)
	 * Offset: (offsetDistance, 0)
	 */
	void duplicateDotRight();

	// ========================================================================
	// LINE DUPLICATION OPERATIONS
	// ========================================================================

	/**
	 * Duplicate selected customLines with 180° rotation
	 * Rotation performed around global center point of all selected lines
	 *
	 * Process:
	 * 1. Calculate global center from all points in selected lines
	 * 2. For each selected line:
	 *    - Copy all properties (points, color, width, curve, label)
	 *    - Rotate all points 180° around global center
	 *    - Create new CustomLine with "D" prefix on label
	 *    - Set isDuplicate = true, axisLock = NONE
	 * 3. Push undo action for each line
	 * 4. Clear original selection, select all duplicated lines
	 */
	void duplicateLineR180();

private:
	ofApp* app;  // Pointer to main app

	// ========================================================================
	// CONFIGURATION STATE
	// ========================================================================

	float offsetDistance = 7.0f;  // Default offset distance in pixels

	// ========================================================================
	// CONSTANTS
	// ========================================================================

	static const float DEFAULT_OFFSET_DISTANCE;  // 7.0 pixels

	// ========================================================================
	// HELPER METHODS
	// ========================================================================

	/**
	 * Push undo action for dot creation
	 */
	void pushUndoAction();
};
