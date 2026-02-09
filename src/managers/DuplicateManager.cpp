#include "DuplicateManager.h"
#include "../ofApp.h"
#include "../operation/gui/ContextMenu.h"
#include "../shape/DotShape.h"
#include "../shape/CustomLine.h"

// Static constant definition
const float DuplicateManager::DEFAULT_OFFSET_DISTANCE = 7.0f;

//--------------------------------------------------------------
DuplicateManager::DuplicateManager(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
DuplicateManager::~DuplicateManager() {
}

// ========================================================================
// SETTERS - Update configuration
// ========================================================================

//--------------------------------------------------------------
void DuplicateManager::setOffsetDistance(float distance) {
	offsetDistance = distance;
}

// ========================================================================
// DOT DUPLICATION OPERATIONS
// ========================================================================

//--------------------------------------------------------------
void DuplicateManager::duplicateDotAbove() {
	// Check if there's a hovered dot
	if (!app->contextMenu->getHasHoveredDot()) {
		return;  // No valid dot
	}

	vec2 dotPos = app->contextMenu->getHoveredDotPos();

	// Calculate new position: offset upward (Y negative)
	vec2 newDotPos = dotPos + vec2(0, -offsetDistance);

	// Create DotShape with userDotRadius (from User Custom slider)
	auto dotShape = std::make_unique<DotShape>(newDotPos, "Dot", app->userDotRadius);
	dotShape->progress = 1.0f;  // Instant appearance (no animation)
	dotShape->setColor(app->colorManager->getUserDotColor());  // Set color from userDotColor

	// Set lower bound to parent dot position
	dotShape->setLowerBound(dotPos);

	// Add to userDots vector
	app->userDots.push_back(std::move(dotShape));

	// Push undo action
	this->pushUndoAction();

	// Reset hovered state
	app->contextMenu->resetHoveredDotState();
}

//--------------------------------------------------------------
void DuplicateManager::duplicateDotBelow() {
	// Check if there's a hovered dot
	if (!app->contextMenu->getHasHoveredDot()) {
		return;  // No valid dot
	}

	vec2 dotPos = app->contextMenu->getHoveredDotPos();

	// Calculate new position: offset downward (Y positive)
	vec2 newDotPos = dotPos + vec2(0, offsetDistance);

	// Create DotShape with userDotRadius (from User Custom slider)
	auto dotShape = std::make_unique<DotShape>(newDotPos, "Dot", app->userDotRadius);
	dotShape->progress = 1.0f;  // Instant appearance (no animation)
	dotShape->setColor(app->colorManager->getUserDotColor());  // Set color from userDotColor

	// Set lower bound to parent dot position
	dotShape->setLowerBound(dotPos);

	// Add to userDots vector
	app->userDots.push_back(std::move(dotShape));

	// Push undo action
	this->pushUndoAction();

	// Reset hovered state
	app->contextMenu->resetHoveredDotState();
}

//--------------------------------------------------------------
void DuplicateManager::duplicateDotLeft() {
	// Check if there's a hovered dot
	if (!app->contextMenu->getHasHoveredDot()) {
		return;  // No valid dot
	}

	vec2 dotPos = app->contextMenu->getHoveredDotPos();

	// Calculate new position: offset leftward (X negative)
	vec2 newDotPos = dotPos + vec2(-offsetDistance, 0);

	// Create DotShape with userDotRadius (from User Custom slider)
	auto dotShape = std::make_unique<DotShape>(newDotPos, "Dot", app->userDotRadius);
	dotShape->progress = 1.0f;  // Instant appearance (no animation)
	dotShape->setColor(app->colorManager->getUserDotColor());  // Set color from userDotColor

	// Set lower bound to parent dot position
	dotShape->setLowerBound(dotPos);

	// Add to userDots vector
	app->userDots.push_back(std::move(dotShape));

	// Push undo action
	this->pushUndoAction();

	// Reset hovered state
	app->contextMenu->resetHoveredDotState();
}

//--------------------------------------------------------------
void DuplicateManager::duplicateDotRight() {
	// Check if there's a hovered dot
	if (!app->contextMenu->getHasHoveredDot()) {
		return;  // No valid dot
	}

	vec2 dotPos = app->contextMenu->getHoveredDotPos();

	// Calculate new position: offset rightward (X positive)
	vec2 newDotPos = dotPos + vec2(offsetDistance, 0);

	// Create DotShape with userDotRadius (from User Custom slider)
	auto dotShape = std::make_unique<DotShape>(newDotPos, "Dot", app->userDotRadius);
	dotShape->progress = 1.0f;  // Instant appearance (no animation)
	dotShape->setColor(app->colorManager->getUserDotColor());  // Set color from userDotColor

	// Set lower bound to parent dot position
	dotShape->setLowerBound(dotPos);

	// Add to userDots vector
	app->userDots.push_back(std::move(dotShape));

	// Push undo action
	this->pushUndoAction();

	// Reset hovered state
	app->contextMenu->resetHoveredDotState();
}

// ========================================================================
// LINE DUPLICATION OPERATIONS
// ========================================================================

//--------------------------------------------------------------
void DuplicateManager::duplicateLineR180() {
	// Check if there are selected lines
	if (!app->selectionManager.hasSelectedLine()) {
		return;  // No selected lines
	}

	// 1. Calculate global center point from all selected lines
	vec2 globalCenter = vec2(0, 0);
	int totalPoints = 0;

	const std::set<int>& indices = app->selectionManager.getSelectedLineIndices();
	for (int index : indices) {
		if (index >= 0 && index < app->customLines.size()) {
			const std::vector<vec2>& points = app->customLines[index].getPoints();
			for (const vec2& point : points) {
				globalCenter += point;
				totalPoints++;
			}
		}
	}

	if (totalPoints == 0) {
		return;  // No valid points
	}

	globalCenter /= totalPoints;  // Average of all points

	// Save initial size before duplication
	size_t oldSize = app->customLines.size();

	// 2. For each selected line, create duplicate with 180° rotation
	for (int index : indices) {
		if (index >= 0 && index < app->customLines.size()) {
			const CustomLine& originalLine = app->customLines[index];

			// Copy all properties
			std::vector<vec2> originalPoints = originalLine.getPoints();
			ofColor lineColor = originalLine.getColor();
			float lineWidth = originalLine.getLineWidth();
			float curve = originalLine.getCurve();
			std::string originalLabel = originalLine.getLabel();

			// Rotate all points 180° around global center
			std::vector<vec2> rotatedPoints;
			for (const vec2& point : originalPoints) {
				vec2 offset = point - globalCenter;
				vec2 rotatedPoint = globalCenter - offset;  // 180° rotation
				rotatedPoints.push_back(rotatedPoint);
			}

			// Create label with "D" prefix
			std::string newLabel = "D" + originalLabel;

			// Create new CustomLine
			CustomLine newLine(rotatedPoints, lineColor, lineWidth, newLabel);
			newLine.setCurve(curve);
			newLine.setProgress(1.0f);  // Instant appearance (no animation)
			newLine.setIsDuplicate(true);  // Mark as duplicate line
			newLine.setAxisLock(AxisLock::NONE);  // Can be moved with CTRL+Scroll

			// Add to customLines
			app->customLines.push_back(newLine);
		}
	}

	// 3. Push undo action for each duplicated line
	for (int index : indices) {
		UndoAction undoAction;
		undoAction.type = CREATE_LINE;
		undoAction.isCreate = true;
		app->pushUndoAction(undoAction);
	}

	// 4. Clear original selection, then select all new duplicated lines
	app->selectionManager.clearLineSelection();

	// Select all newly created DcustomLines
	for (size_t i = oldSize; i < app->customLines.size(); i++) {
		app->selectionManager.selectLine(static_cast<int>(i));
	}

	// Sync global color from first selected DcustomLine
	app->syncColorFromSelectedObjects();
}

// ========================================================================
// HELPER METHODS
// ========================================================================

//--------------------------------------------------------------
void DuplicateManager::pushUndoAction() {
	UndoAction undoAction;
	undoAction.type = CREATE_DOT;
	undoAction.isCreate = true;
	app->pushUndoAction(undoAction);
}
