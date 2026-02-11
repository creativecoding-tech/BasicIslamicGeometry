#pragma once

#include <set>

/**
 * SelectionManager
 *
 * Handles all selection logic for CustomLine, Polygon, and UserDot.
 * Responsibilities:
 * - Selection state management (selected indices)
 * - Multi-select support (CTRL+click)
 * - Track last selected index
 * - Utility methods for querying selection state
 */
class SelectionManager {
public:
	SelectionManager() = default;
	~SelectionManager() = default;

	// ========================================================================
	// CUSTOM LINE SELECTION
	// ========================================================================

	/**
	 * Select a CustomLine
	 * @param index Index of the customLine to select
	 */
	void selectLine(int index);

	/**
	 * Deselect a CustomLine
	 * @param index Index of the customLine to deselect
	 */
	void deselectLine(int index);

	/**
	 * Toggle selection (select if not selected, deselect if selected)
	 * @param index Index of the customLine to toggle
	 */
	void toggleLineSelection(int index);

	/**
	 * Clear all CustomLine selection
	 */
	void clearLineSelection();

	/**
	 * Check if a CustomLine is selected
	 * @param index Index of the customLine to check
	 * @return true if selected
	 */
	bool isLineSelected(int index) const;

	/**
	 * Get number of selected CustomLines
	 * @return Number of selected lines
	 */
	int getSelectedLineCount() const;

	/**
	 * Get all selected CustomLine indices
	 * @return Set containing selected line indices
	 */
	const std::set<int>& getSelectedLineIndices() const { return selectedLineIndices; }

	/**
	 * Get last selected line index
	 * @return Index of last clicked line (-1 if none)
	 */
	int getLastSelectedLineIndex() const { return lastSelectedLineIndex; }

	/**
	 * Set last selected line index
	 * @param index Index of last clicked line
	 */
	void setLastSelectedLineIndex(int index) { lastSelectedLineIndex = index; }

	// ========================================================================
	// POLYGON SELECTION
	// ========================================================================

	/**
	 * Select a Polygon
	 * @param index Index of the polygon to select
	 */
	void selectPolygon(int index);

	/**
	 * Deselect a Polygon
	 * @param index Index of the polygon to deselect
	 */
	void deselectPolygon(int index);

	/**
	 * Toggle selection (select if not selected, deselect if selected)
	 * @param index Index of the polygon to toggle
	 */
	void togglePolygonSelection(int index);

	/**
	 * Clear all Polygon selection
	 */
	void clearPolygonSelection();

	/**
	 * Check if a Polygon is selected
	 * @param index Index of the polygon to check
	 * @return true if selected
	 */
	bool isPolygonSelected(int index) const;

	/**
	 * Get number of selected Polygons
	 * @return Number of selected polygons
	 */
	int getSelectedPolygonCount() const;

	/**
	 * Get all selected Polygon indices
	 * @return Set containing selected polygon indices
	 */
	const std::set<int>& getSelectedPolygonIndices() const { return selectedPolygonIndices; }

	/**
	 * Get last selected polygon index
	 * @return Index of last clicked polygon (-1 if none)
	 */
	int getLastSelectedPolygonIndex() const { return lastSelectedPolygonIndex; }

	/**
	 * Set last selected polygon index
	 * @param index Index of last clicked polygon
	 */
	void setLastSelectedPolygonIndex(int index) { lastSelectedPolygonIndex = index; }

	// ========================================================================
	// USER DOT SELECTION
	// ========================================================================

	/**
	 * Select a UserDot
	 * @param index Index of the userDot to select
	 */
	void selectUserDot(int index);

	/**
	 * Deselect a UserDot
	 * @param index Index of the userDot to deselect
	 */
	void deselectUserDot(int index);

	/**
	 * Toggle selection (select if not selected, deselect if selected)
	 * @param index Index of the userDot to toggle
	 */
	void toggleUserDotSelection(int index);

	/**
	 * Clear all UserDot selection
	 */
	void clearUserDotSelection();

	/**
	 * Check if a UserDot is selected
	 * @param index Index of the userDot to check
	 * @return true if selected
	 */
	bool isUserDotSelected(int index) const;

	/**
	 * Get number of selected UserDots
	 * @return Number of selected userDots
	 */
	int getSelectedUserDotCount() const;

	/**
	 * Get all selected UserDot indices
	 * @return Set containing selected userDot indices
	 */
	const std::set<int>& getSelectedUserDotIndices() const { return selectedUserDotIndices; }

	/**
	 * Get last selected userDot index
	 * @return Index of last clicked userDot (-1 if none)
	 */
	int getLastSelectedUserDotIndex() const { return lastSelectedUserDotIndex; }

	/**
	 * Set last selected userDot index
	 * @param index Index of last clicked userDot
	 */
	void setLastSelectedUserDotIndex(int index) { lastSelectedUserDotIndex = index; }

	// ========================================================================
	// UTILITY METHODS
	// ========================================================================

	/**
	 * Clear all selections (lines, polygons, userDots)
	 */
	void clearAllSelections();

	/**
	 * Check if any object is selected
	 * @return true if at least 1 object is selected
	 */
	bool hasAnySelection() const;

	/**
	 * Check if any CustomLine is selected
	 * @return true if at least 1 line is selected
	 */
	bool hasSelectedLine() const { return !selectedLineIndices.empty(); }

	/**
	 * Check if any Polygon is selected
	 * @return true if at least 1 polygon is selected
	 */
	bool hasSelectedPolygon() const { return !selectedPolygonIndices.empty(); }

	/**
	 * Check if any UserDot is selected
	 * @return true if at least 1 userDot is selected
	 */
	bool hasSelectedUserDot() const { return !selectedUserDotIndices.empty(); }

private:
	// CustomLine selection state
	std::set<int> selectedLineIndices;
	int lastSelectedLineIndex = -1;

	// Polygon selection state
	std::set<int> selectedPolygonIndices;
	int lastSelectedPolygonIndex = -1;

	// UserDot selection state
	std::set<int> selectedUserDotIndices;
	int lastSelectedUserDotIndex = -1;
};
