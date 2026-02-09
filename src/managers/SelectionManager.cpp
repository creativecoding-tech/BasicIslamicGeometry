#include "SelectionManager.h"

// ========================================================================
// CUSTOM LINE SELECTION
// ========================================================================

void SelectionManager::selectLine(int index) {
	selectedLineIndices.insert(index);
	lastSelectedLineIndex = index;
}

void SelectionManager::deselectLine(int index) {
	selectedLineIndices.erase(index);
	// Update lastSelectedLineIndex if needed
	if (lastSelectedLineIndex == index) {
		lastSelectedLineIndex = selectedLineIndices.empty() ? -1 : *selectedLineIndices.rbegin();
	}
}

void SelectionManager::toggleLineSelection(int index) {
	if (isLineSelected(index)) {
		deselectLine(index);
	} else {
		selectLine(index);
	}
}

void SelectionManager::clearLineSelection() {
	selectedLineIndices.clear();
	lastSelectedLineIndex = -1;
}

bool SelectionManager::isLineSelected(int index) const {
	return selectedLineIndices.find(index) != selectedLineIndices.end();
}

int SelectionManager::getSelectedLineCount() const {
	return static_cast<int>(selectedLineIndices.size());
}

// ========================================================================
// POLYGON SELECTION
// ========================================================================

void SelectionManager::selectPolygon(int index) {
	selectedPolygonIndices.insert(index);
	lastSelectedPolygonIndex = index;
}

void SelectionManager::deselectPolygon(int index) {
	selectedPolygonIndices.erase(index);
	// Update lastSelectedPolygonIndex if needed
	if (lastSelectedPolygonIndex == index) {
		lastSelectedPolygonIndex = selectedPolygonIndices.empty() ? -1 : *selectedPolygonIndices.rbegin();
	}
}

void SelectionManager::togglePolygonSelection(int index) {
	if (isPolygonSelected(index)) {
		deselectPolygon(index);
	} else {
		selectPolygon(index);
	}
}

void SelectionManager::clearPolygonSelection() {
	selectedPolygonIndices.clear();
	lastSelectedPolygonIndex = -1;
}

bool SelectionManager::isPolygonSelected(int index) const {
	return selectedPolygonIndices.find(index) != selectedPolygonIndices.end();
}

int SelectionManager::getSelectedPolygonCount() const {
	return static_cast<int>(selectedPolygonIndices.size());
}

// ========================================================================
// USER DOT SELECTION
// ========================================================================

void SelectionManager::selectUserDot(int index) {
	selectedUserDotIndices.insert(index);
	lastSelectedUserDotIndex = index;
}

void SelectionManager::deselectUserDot(int index) {
	selectedUserDotIndices.erase(index);
	// Update lastSelectedUserDotIndex if needed
	if (lastSelectedUserDotIndex == index) {
		lastSelectedUserDotIndex = selectedUserDotIndices.empty() ? -1 : *selectedUserDotIndices.rbegin();
	}
}

void SelectionManager::toggleUserDotSelection(int index) {
	if (isUserDotSelected(index)) {
		deselectUserDot(index);
	} else {
		selectUserDot(index);
	}
}

void SelectionManager::clearUserDotSelection() {
	selectedUserDotIndices.clear();
	lastSelectedUserDotIndex = -1;
}

bool SelectionManager::isUserDotSelected(int index) const {
	return selectedUserDotIndices.find(index) != selectedUserDotIndices.end();
}

int SelectionManager::getSelectedUserDotCount() const {
	return static_cast<int>(selectedUserDotIndices.size());
}

// ========================================================================
// UTILITY METHODS
// ========================================================================

void SelectionManager::clearAllSelections() {
	clearLineSelection();
	clearPolygonSelection();
	clearUserDotSelection();
}

bool SelectionManager::hasAnySelection() const {
	return hasSelectedLine() || hasSelectedPolygon() || hasSelectedUserDot();
}
