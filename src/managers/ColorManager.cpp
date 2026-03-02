#include "ColorManager.h"
#include "../ofApp.h"

// Static constant definition
const ofColor ColorManager::DEFAULT_COLOR = ofColor(0, 0, 255);

//--------------------------------------------------------------
ColorManager::ColorManager(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
ColorManager::~ColorManager() {
}

// ========================================================================
// SETTERS - Update colors
// ========================================================================

//--------------------------------------------------------------
void ColorManager::updateCustomLineColor(ofColor color) {
	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_LINE;
	undoAction.newColor = color;

	// Jika ada customLine yang selected, hanya update yang selected saja
	if (app->selectionManager.hasSelectedLine()) {
		// Copy indices ke local vector untuk menghindari iterator invalidation
		std::vector<int> selectedLineIndices(app->selectionManager.getSelectedLineIndices().begin(),
		                                     app->selectionManager.getSelectedLineIndices().end());
		for (int lineIndex : selectedLineIndices) {
			if (lineIndex >= 0 && lineIndex < app->customLines.size()) {
				// Cek apakah color BENAR-BENAR berubah
				ofColor oldColor = app->customLines[lineIndex].getColor();
				if (oldColor.r == color.r && oldColor.g == color.g &&
				    oldColor.b == color.b && oldColor.a == color.a) {
					continue;  // Skip jika color sama
				}

				// Simpan old color SEBELUM mengubah
				undoAction.colorIndices.push_back(lineIndex);
				undoAction.oldColors.push_back(oldColor);

				// Ubah warna
				app->customLines[lineIndex].setColor(color);
			}
		}
		// Push undo action hanya jika ada yang BERUBAH
		if (!undoAction.colorIndices.empty()) {
			app->pushUndoAction(undoAction);
		}
	}
	// Jika tidak ada yang selected, TIDAK update existing customLines
	// Hanya update global color variable untuk new customLines
	else {
		customLineColor = color;
	}
}

//--------------------------------------------------------------
void ColorManager::updatePolygonColor(ofColor color) {
	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_POLYGON;
	undoAction.newColor = color;

	// Jika ada polygon yang selected, hanya update yang selected saja
	if (app->selectionManager.hasSelectedPolygon()) {
		// Simpan old color SEBELUM mengubah untuk SEMUA selected polygons
		const std::set<int>& indices = app->selectionManager.getSelectedPolygonIndices();
		for (int index : indices) {
			if (index >= 0 && index < app->polygonShapes.size()) {
				// Cek apakah color BENAR-BENAR berubah
				ofColor oldColor = app->polygonShapes[index].getColor();
				if (oldColor.r == color.r && oldColor.g == color.g &&
				    oldColor.b == color.b && oldColor.a == color.a) {
					continue;  // Skip jika color sama
				}

				// Simpan old color SEBELUM mengubah
				undoAction.colorIndices.push_back(index);
				undoAction.oldColors.push_back(oldColor);

				// Ubah warna
				app->polygonShapes[index].setColor(color);
			}
		}
		// Push undo action hanya jika ada yang BERUBAH
		if (!undoAction.colorIndices.empty()) {
			app->pushUndoAction(undoAction);
		}
	}
	// Jika tidak ada yang selected, TIDAK update existing polygons
	// Hanya update global color variable untuk new polygons
	else {
		polygonColor = color;
	}
}

//--------------------------------------------------------------
void ColorManager::updatePolygonAlpha(uint8_t alpha) {
	// Update alpha transparansi untuk semua polygon yang terseleksi
	// RGB tetap, hanya alpha yang berubah

	if (!app->selectionManager.hasSelectedPolygon()) {
		return;  // Tidak ada polygon terseleksi
	}

	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_POLYGON;

	// Update alpha semua selected polygon, RGB tetap
	const std::set<int>& indices = app->selectionManager.getSelectedPolygonIndices();
	for (int polygonIndex : indices) {
		if (polygonIndex >= 0 && polygonIndex < app->polygonShapes.size()) {
			// Simpan old color SEBELUM mengubah
			undoAction.colorIndices.push_back(polygonIndex);
			undoAction.oldColors.push_back(app->polygonShapes[polygonIndex].getColor());

			// Update alpha saja, RGB tetap
			ofColor currentColor = app->polygonShapes[polygonIndex].getColor();
			ofColor newColor = ofColor(currentColor.r, currentColor.g, currentColor.b, alpha);
			app->polygonShapes[polygonIndex].setColor(newColor);

			// Set new color untuk undo info (semua polygons dapat alpha baru yang sama)
			undoAction.newColor = newColor;
		}
	}

	// Push undo action
	if (!undoAction.colorIndices.empty()) {
		app->pushUndoAction(undoAction);
	}
}

//--------------------------------------------------------------
void ColorManager::updateUserDotColor(ofColor color) {
	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_DOT;
	undoAction.newColor = color;

	// Jika ada userDot yang terseleksi, update hanya yang terseleksi
	if (app->selectionManager.hasSelectedUserDot()) {
		// Copy indices ke local vector untuk menghindari iterator invalidation
		std::vector<int> selectedDotIndices(app->selectionManager.getSelectedUserDotIndices().begin(),
		                                     app->selectionManager.getSelectedUserDotIndices().end());
		for (int dotIndex : selectedDotIndices) {
			if (dotIndex >= 0 && dotIndex < app->userDots.size()) {
				if (app->userDots[dotIndex]) {
					// Cek apakah color BENAR-BENAR berubah
					ofColor oldColor = app->userDots[dotIndex]->getColor();
					if (oldColor.r == color.r && oldColor.g == color.g &&
					    oldColor.b == color.b && oldColor.a == color.a) {
						continue;  // Skip jika color sama
					}

					// Simpan old color SEBELUM mengubah
					undoAction.colorIndices.push_back(dotIndex);
					undoAction.oldColors.push_back(oldColor);

					// Ubah warna
					app->userDots[dotIndex]->setColor(color);
				}
			}
		}
		// Push undo action hanya jika ada yang BERUBAH
		if (!undoAction.colorIndices.empty()) {
			app->pushUndoAction(undoAction);
		}
	}
	// Jika tidak ada yang selected, TIDAK update existing userDots
	// Hanya update global color variable untuk new userDots
	else {
		userDotColor = color;
	}
}

// ========================================================================
// RESET OPERATIONS
// ========================================================================

//--------------------------------------------------------------
void ColorManager::resetAllCustomLineColor() {
	// Reset SEMUA customLine ke warna default biru (0, 0, 255)
	ofColor defaultColor = DEFAULT_COLOR;

	// Update variabel global untuk customLine baru
	customLineColor = defaultColor;

	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_LINE;
	undoAction.newColor = defaultColor;

	// FORCE update SEMUA customLines (tidak peduli ada yang selected atau tidak)
	for (size_t i = 0; i < app->customLines.size(); i++) {
		// Simpan old color SEBELUM mengubah
		undoAction.colorIndices.push_back(static_cast<int>(i));
		undoAction.oldColors.push_back(app->customLines[i].getColor());

		// Ubah warna ke default
		app->customLines[i].setColor(defaultColor);
	}

	// Push undo action
	if (!undoAction.colorIndices.empty()) {
		app->pushUndoAction(undoAction);
	}
}

//--------------------------------------------------------------
void ColorManager::resetSelectedCustomLineColor() {
	// Reset hanya customLine yang terseleksi ke warna default biru (0, 0, 255)
	ofColor defaultColor = DEFAULT_COLOR;

	// Update variabel global untuk customLine baru
	customLineColor = defaultColor;

	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_LINE;
	undoAction.newColor = defaultColor;

	// Hanya update yang selected
	if (app->selectionManager.hasSelectedLine()) {
		// Copy indices ke local vector untuk menghindari iterator invalidation
		std::vector<int> selectedLineIndices(app->selectionManager.getSelectedLineIndices().begin(),
		                                     app->selectionManager.getSelectedLineIndices().end());
		for (int lineIndex : selectedLineIndices) {
			if (lineIndex >= 0 && lineIndex < app->customLines.size()) {
				// Simpan old color SEBELUM mengubah
				undoAction.colorIndices.push_back(lineIndex);
				undoAction.oldColors.push_back(app->customLines[lineIndex].getColor());

				// Ubah warna ke default
				app->customLines[lineIndex].setColor(defaultColor);
			}
		}
		// Push undo action hanya jika ada yang diubah
		if (!undoAction.colorIndices.empty()) {
			app->pushUndoAction(undoAction);
		}
	}
}

//--------------------------------------------------------------
void ColorManager::resetAllPolygonColor() {
	// Reset SEMUA polygon ke warna default biru (0, 0, 255)
	ofColor defaultColor = DEFAULT_COLOR;

	// Update variabel global untuk polygon baru
	polygonColor = defaultColor;

	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_POLYGON;
	undoAction.newColor = defaultColor;

	// FORCE update SEMUA polygons (tidak peduli ada yang selected atau tidak)
	for (size_t i = 0; i < app->polygonShapes.size(); i++) {
		// Simpan old color SEBELUM mengubah
		undoAction.colorIndices.push_back(static_cast<int>(i));
		undoAction.oldColors.push_back(app->polygonShapes[i].getColor());

		// Ubah warna ke default
		app->polygonShapes[i].setColor(defaultColor);
	}

	// Push undo action
	if (!undoAction.colorIndices.empty()) {
		app->pushUndoAction(undoAction);
	}
}

//--------------------------------------------------------------
void ColorManager::resetSelectedPolygonColor() {
	// Reset hanya polygon yang terseleksi ke warna default biru (0, 0, 255)
	ofColor defaultColor = DEFAULT_COLOR;

	// Update variabel global untuk polygon baru
	polygonColor = defaultColor;

	// Siapkan undo action
	UndoAction undoAction;
	undoAction.type = CHANGE_COLOR_POLYGON;
	undoAction.newColor = defaultColor;

	// Hanya update yang selected
	if (app->selectionManager.hasSelectedPolygon()) {
		const std::set<int>& indices = app->selectionManager.getSelectedPolygonIndices();
		for (int polygonIndex : indices) {
			if (polygonIndex >= 0 && polygonIndex < app->polygonShapes.size()) {
				// Simpan old color SEBELUM mengubah
				undoAction.colorIndices.push_back(polygonIndex);
				undoAction.oldColors.push_back(app->polygonShapes[polygonIndex].getColor());

				// Ubah warna ke default
				app->polygonShapes[polygonIndex].setColor(defaultColor);
			}
		}
		// Push undo action
		if (!undoAction.colorIndices.empty()) {
			app->pushUndoAction(undoAction);
		}
	}
}

// ========================================================================
// COPY/PASTE OPERATIONS
// ========================================================================

//--------------------------------------------------------------
void ColorManager::copyDotColor() {
	// Validasi: Hanya 1 selected DAN tidak ada mixed type selection
	bool canCopyColor = false;

	if (app->selectionManager.getSelectedUserDotCount() == 1) {
		// Cek apakah ada mixed type selection
		// Copy dari DOT: pastikan tidak ada line atau polygon yang terseleksi
		canCopyColor = !app->selectionManager.hasSelectedLine() && !app->selectionManager.hasSelectedPolygon();
	}

	if (canCopyColor) {
		// Copy color dari selected dot
		int firstIndex = app->selectionManager.getLastSelectedUserDotIndex();
		if (firstIndex >= 0 && firstIndex < app->userDots.size()) {
			if (app->userDots[firstIndex]) {
				clipboardColor = app->userDots[firstIndex]->getColor();
				hasClipboardColorFlag = true;
			}
		}
	}
}

//--------------------------------------------------------------
void ColorManager::copyPolygonColor() {
	// Validasi: Hanya 1 selected DAN tidak ada mixed type selection
	bool canCopyColor = false;

	if (app->selectionManager.getSelectedPolygonCount() == 1) {
		// Cek apakah ada mixed type selection
		// Copy dari POLYGON: pastikan tidak ada dot atau line yang terseleksi
		canCopyColor = !app->selectionManager.hasSelectedUserDot() && !app->selectionManager.hasSelectedLine();
	}

	if (canCopyColor) {
		// Copy color dari selected polygon
		int firstIndex = app->selectionManager.getLastSelectedPolygonIndex();
		if (firstIndex >= 0 && firstIndex < app->polygonShapes.size()) {
			clipboardColor = app->polygonShapes[firstIndex].getColor();
			hasClipboardColorFlag = true;
		}
	}
}

//--------------------------------------------------------------
void ColorManager::copyLineColor() {
	// Validasi: Hanya 1 selected DAN tidak ada mixed type selection
	bool canCopyColor = false;

	if (app->selectionManager.getSelectedLineCount() == 1) {
		// Cek apakah ada mixed type selection
		// Copy dari CUSTOMLINE: pastikan tidak ada dot atau polygon yang terseleksi
		canCopyColor = !app->selectionManager.hasSelectedUserDot() && !app->selectionManager.hasSelectedPolygon();
	}

	if (canCopyColor) {
		// Copy color dari selected line
		int firstIndex = app->selectionManager.getLastSelectedLineIndex();
		if (firstIndex >= 0 && firstIndex < app->customLines.size()) {
			clipboardColor = app->customLines[firstIndex].getColor();
			hasClipboardColorFlag = true;
		}
	}
}

//--------------------------------------------------------------
void ColorManager::pasteColorToDot() {
	// Paste color boleh multi-type (tidak perlu validasi mixed type)
	// Validasi: Harus ada object yang terseleksi DAN ada color di clipboard
	bool canPaste = (app->selectionManager.getSelectedUserDotCount() >= 1) && hasClipboardColorFlag;

	if (canPaste) {
		// Paste color ke semua selected userDots
		updateUserDotColor(clipboardColor);
	}
}

//--------------------------------------------------------------
void ColorManager::pasteColorToPolygon() {
	// Paste color boleh multi-type (tidak perlu validasi mixed type)
	// Validasi: Harus ada object yang terseleksi DAN ada color di clipboard
	bool canPaste = (app->selectionManager.getSelectedPolygonCount() >= 1) && hasClipboardColorFlag;

	if (canPaste) {
		// Paste color ke semua selected polygons
		updatePolygonColor(clipboardColor);
	}
}

//--------------------------------------------------------------
void ColorManager::pasteColorToLine() {
	// Paste color boleh multi-type (tidak perlu validasi mixed type)
	// Validasi: Harus ada object yang terseleksi DAN ada color di clipboard
	bool canPaste = (app->selectionManager.getSelectedLineCount() >= 1) && hasClipboardColorFlag;

	if (canPaste) {
		// Paste color ke semua selected customLines
		updateCustomLineColor(clipboardColor);
	}
}

// ========================================================================
// SYNC OPERATIONS
// ========================================================================

//--------------------------------------------------------------
void ColorManager::syncColorPickersFromSelection() {
	// Sync SEMUA global color variables ke targetColor yang SAMA
	// Priority: userDot > polygon > customLine

	ofColor targetColor;

	// 1. Cek userDot dulu (priority tertinggi)
	if (app->selectionManager.hasSelectedUserDot()) {
		int firstIndex = app->selectionManager.getLastSelectedUserDotIndex();
		if (firstIndex >= 0 && firstIndex < app->userDots.size() && app->userDots[firstIndex]) {
			targetColor = app->userDots[firstIndex]->getColor();
		}
	}
	// 2. Cek polygon
	else if (app->selectionManager.hasSelectedPolygon()) {
		int firstIndex = app->selectionManager.getLastSelectedPolygonIndex();
		if (firstIndex >= 0 && firstIndex < app->polygonShapes.size()) {
			targetColor = app->polygonShapes[firstIndex].getColor();
		}
	}
	// 3. Cek customLine
	else if (app->selectionManager.hasSelectedLine()) {
		int firstIndex = app->selectionManager.getLastSelectedLineIndex();
		if (firstIndex >= 0 && firstIndex < app->customLines.size()) {
			targetColor = app->customLines[firstIndex].getColor();
		}
	}
	else {
		return;  // Tidak ada yang selected, jangan update apa-apa
	}

	// Sync SEMUA global color variables ke targetColor yang SAMA
	userDotColor = targetColor;
	polygonColor = targetColor;
	customLineColor = targetColor;
}

//--------------------------------------------------------------
void ColorManager::resetAllColorPickers() {
	// Reset SEMUA global color variables ke warna biru default
	ofColor defaultColor = DEFAULT_COLOR;

	customLineColor = defaultColor;
	polygonColor = defaultColor;
	userDotColor = defaultColor;
}

// ========================================================================
// HELPER METHODS
// ========================================================================

//--------------------------------------------------------------
void ColorManager::pushUndoAction(int type, ofColor newColor) {
	UndoAction undoAction;
	undoAction.type = static_cast<UndoActionType>(type);
	undoAction.newColor = newColor;
	app->pushUndoAction(undoAction);
}
