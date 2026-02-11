#pragma once

#include "ofMain.h"
#include <vector>
#include "../shape/CustomLine.h"

// Forward declarations
class ofApp;

using glm::vec2;

/**
 * GeometryUtils
 *
 * Namespace containing helper functions for geometric operations.
 * Provides reusable utilities for coordinate transformation, distance calculations,
 * line detection, and point operations.
 *
 * Features:
 * - Coordinate transformation (screen ↔ world)
 * - Line operations (distance, detection, validation)
 * - Point operations (distance, dot detection)
 * - Data collection (all dots from template)
 */
namespace GeometryUtils {

	// ========================================================================
	// COORDINATE TRANSFORMATION
	// ========================================================================

	/**
	 * Convert screen coordinate to world coordinate
	 * Applies inverse transformation: translate → rotate → scale → pan
	 *
	 * @param screenPos Screen position (pixel coordinates)
	 * @param pan Pan offset
	 * @param zoom Zoom level
	 * @param rotation Rotation angle (radians)
	 * @return World position after inverse transformation
	 */
	vec2 screenToWorld(vec2 screenPos, vec2 pan, float zoom, float rotation);

	// ========================================================================
	// LINE OPERATIONS
	// ========================================================================

	/**
	 * Calculate distance from point to line segment
	 * Uses formula: distance = |cross(product)| / lineLength
	 *
	 * @param point Point to measure distance from
	 * @param lineStart Start point of line
	 * @param lineEnd End point of line
	 * @param curve Line curve amount (default: 0.0 for straight line)
	 * @return Distance from point to line (in world coordinates)
	 */
	float distanceToLine(vec2 point, vec2 lineStart, vec2 lineEnd, float curve = 0.0f);

	/**
	 * Check if mouse position is over a line
	 *
	 * @param mousePos Mouse position in world coordinates
	 * @param lineStart Start point of line
	 * @param lineEnd End point of line
	 * @param lineWidth Line width threshold
	 * @return true if mouse is over the line
	 */
	bool isMouseOverLine(vec2 mousePos, vec2 lineStart, vec2 lineEnd, float lineWidth);

	/**
	 * Get index of line at given mouse position
	 *
	 * @param mousePos Mouse position in world coordinates
	 * @param lines Vector of custom lines to search
	 * @param lineWidth Threshold for line detection
	 * @return Index of line at position, or -1 if no line found
	 */
	int getLineIndexAtPosition(vec2 mousePos, const std::vector<CustomLine>& lines, float lineWidth);

	/**
	 * Check if a line with given start and end points already exists
	 * Prevents duplicate line creation
	 *
	 * @param from Start point of line
	 * @param to End point of line
	 * @param lines Vector of custom lines to check
	 * @return true if line already exists (in either direction)
	 */
	bool lineExists(vec2 from, vec2 to, const std::vector<CustomLine>& lines);

	// ========================================================================
	// POINT OPERATIONS
	// ========================================================================

	/**
	 * Check if mouse position is over a dot
	 *
	 * @param mousePos Mouse position in world coordinates
	 * @param dotPos Position of the dot
	 * @param threshold Distance threshold for dot detection
	 * @return true if mouse is over the dot
	 */
	bool isMouseOverDot(vec2 mousePos, vec2 dotPos, float threshold);

	/**
	 * Calculate distance between two points
	 *
	 * @param p1 First point
	 * @param p2 Second point
	 * @return Euclidean distance between points
	 */
	float distanceToPoint(vec2 p1, vec2 p2);

} // namespace GeometryUtils
