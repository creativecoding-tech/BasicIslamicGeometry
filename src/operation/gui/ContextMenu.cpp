#include "ContextMenu.h"
#include "../../ofApp.h"

//--------------------------------------------------------------
ContextMenu::ContextMenu(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
void ContextMenu::showWindow(ContextMenuType type, vec2 position) {
	currentType = type;
	contextMenuPos = position;
	showContextMenu = true;
}

//--------------------------------------------------------------
void ContextMenu::hideWindow() {
	showContextMenu = false;
	currentType = NONE;
	hoveredDotPos = vec2(0, 0);
	hasHoveredDot = false;  // Reset flag
	hoveredPolygonIndex = -1;
	hoveredLineIndex = -1;
	isUserDotContext = false;  // Reset flag
}

//--------------------------------------------------------------
void ContextMenu::draw() {
	if (!showContextMenu) {
		return;
	}

	ImGui::SetNextWindowPos(ImVec2(contextMenuPos.x, contextMenuPos.y));

	if (ImGui::Begin("ContextMenu", &showContextMenu,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize)) {

		// Context menu untuk DOT (Duplicate Dot, Copy/Paste Color)
		if (currentType == DOT_CONTEXT) {
			if (isUserDotContext) {
				// Klik kanan pada userDot (duplicate dot) → hanya Copy/Paste Color
				int selectedDotCount = app->selectedUserDotIndices.size();
				showCopyPasteColorMenus(selectedDotCount, DOT_CONTEXT);
			} else {
				// Klik kanan pada original dot → hanya Duplicate Dot
				if (ImGui::MenuItem("Duplicate Dot Above")) {
					app->duplicateDotAbove();
					showContextMenu = false;
				}
				if (ImGui::MenuItem("Duplicate Dot Below")) {
					app->duplicateDotBelow();
					showContextMenu = false;
				}
				if (ImGui::MenuItem("Duplicate Dot Left")) {
					app->duplicateDotLeft();
					showContextMenu = false;
				}
				if (ImGui::MenuItem("Duplicate Dot Right")) {
					app->duplicateDotRight();
					showContextMenu = false;
				}
			}
		}
		// Context menu untuk POLYGON (Duplicate Polygon, Copy/Paste Color)
		else if (currentType == POLYGON_CONTEXT) {
			if (ImGui::MenuItem("Duplicate Polygon Above")) {
				// TODO: Implement event untuk duplicate polygon above
				showContextMenu = false;
			}
			if (ImGui::MenuItem("Duplicate Polygon Below")) {
				// TODO: Implement event untuk duplicate polygon below
				showContextMenu = false;
			}
			if (ImGui::MenuItem("Duplicate Polygon Left")) {
				// TODO: Implement event untuk duplicate polygon left
				showContextMenu = false;
			}
			if (ImGui::MenuItem("Duplicate Polygon Right")) {
				// TODO: Implement event untuk duplicate polygon right
				showContextMenu = false;
			}

			ImGui::Separator();

			// Copy/Paste Color untuk POLYGON
			int selectedPolygonCount = app->selectedPolygonIndices.size();
			showCopyPasteColorMenus(selectedPolygonCount, POLYGON_CONTEXT);
		}
		// Context menu untuk CUSTOMLINE (Create Polygon, Copy/Paste Color, Lock Axis untuk DcustomLine)
		else if (currentType == CUSTOMLINE_CONTEXT) {
			// ===== MENU 1: Create Polygon =====
			int selectedCount = app->selectedLineIndices.size();

			if (selectedCount > 1) {
				// Enable hanya jika > 1 selected line
				if (ImGui::MenuItem("Create Polygon")) {
					app->createInvisiblePolygonFromSelected();
					showContextMenu = false;
				}
			} else {
				// Disabled jika ≤1 selected line
				ImGui::BeginDisabled();
				ImGui::MenuItem("Create Polygon");
				ImGui::EndDisabled();
			}

			ImGui::Separator();

			// Copy/Paste Color untuk CUSTOMLINE
			showCopyPasteColorMenus(selectedCount, CUSTOMLINE_CONTEXT);

			ImGui::Separator();

			// ===== MENU 4: Lock/Unlock Axis (KHUSUS untuk DcustomLine) =====
			// Cek apakah line yang di-klik adalah duplicate line
			if (hoveredLineIndex >= 0 && hoveredLineIndex < app->customLines.size()) {
				const CustomLine& line = app->customLines[hoveredLineIndex];

				// Cek apakah ini duplicate line (hasil dari duplicateLineR180)
				if (line.getIsDuplicate()) {
					ImGui::Separator();
					ImGui::SeparatorText("DcustomLine Axis Lock");

					// Hitung jumlah DcustomLine yang terseleksi
					int selectedDLineCount = 0;
					for (int index : app->selectedLineIndices) {
						if (index >= 0 && index < app->customLines.size()) {
							if (app->customLines[index].getIsDuplicate()) {
								selectedDLineCount++;
							}
						}
					}

					// Jika hanya 1 DcustomLine terseleksi → Show per-line menu
					if (selectedDLineCount == 1) {
						AxisLock currentLock = line.getAxisLock();

						// Menu 1: Lock/Unlock X Axis
						bool lockX = (currentLock == AxisLock::LOCK_X || currentLock == AxisLock::LOCK_BOTH);
						const char* labelX = lockX ? "Unlock X Axis" : "Lock X Axis";
						if (ImGui::MenuItem(labelX)) {
							// Toggle lock X
							if (lockX) {
								// Unlock X
								if (currentLock == AxisLock::LOCK_BOTH) {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::LOCK_Y);
								} else {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::NONE);
								}
							} else {
								// Lock X axis
								if (currentLock == AxisLock::LOCK_Y) {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::LOCK_BOTH);
								} else {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::LOCK_X);
								}
							}
							showContextMenu = false;
						}

						// Menu 2: Lock/Unlock Y Axis
						bool lockY = (currentLock == AxisLock::LOCK_Y || currentLock == AxisLock::LOCK_BOTH);
						const char* labelY = lockY ? "Unlock Y Axis" : "Lock Y Axis";
						if (ImGui::MenuItem(labelY)) {
							// Toggle lock Y
							if (lockY) {
								// Unlock Y
								if (currentLock == AxisLock::LOCK_BOTH) {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::LOCK_X);
								} else {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::NONE);
								}
							} else {
								// Lock Y axis
								if (currentLock == AxisLock::LOCK_X) {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::LOCK_BOTH);
								} else {
									app->customLines[hoveredLineIndex].setAxisLock(AxisLock::LOCK_Y);
								}
							}
							showContextMenu = false;
						}
					}
					// Jika >1 DcustomLine terseleksi → Show bulk menu
					else {
						ImGui::SeparatorText("Bulk Operation");

						if (ImGui::MenuItem("Unlock All")) {
							for (int index : app->selectedLineIndices) {
								if (index >= 0 && index < app->customLines.size()) {
									if (app->customLines[index].getIsDuplicate()) {
										app->customLines[index].setAxisLock(AxisLock::NONE);
									}
								}
							}
							showContextMenu = false;
						}

						if (ImGui::MenuItem("Lock All")) {
							for (int index : app->selectedLineIndices) {
								if (index >= 0 && index < app->customLines.size()) {
									if (app->customLines[index].getIsDuplicate()) {
										app->customLines[index].setAxisLock(AxisLock::LOCK_BOTH);
									}
								}
							}
							showContextMenu = false;
						}

						if (ImGui::MenuItem("Lock X All")) {
							for (int index : app->selectedLineIndices) {
								if (index >= 0 && index < app->customLines.size()) {
									if (app->customLines[index].getIsDuplicate()) {
										app->customLines[index].setAxisLock(AxisLock::LOCK_X);
									}
								}
							}
							showContextMenu = false;
						}

						if (ImGui::MenuItem("Lock Y All")) {
							for (int index : app->selectedLineIndices) {
								if (index >= 0 && index < app->customLines.size()) {
									if (app->customLines[index].getIsDuplicate()) {
										app->customLines[index].setAxisLock(AxisLock::LOCK_Y);
									}
								}
							}
							showContextMenu = false;
						}

						if (ImGui::MenuItem("Unlock X All")) {
							for (int index : app->selectedLineIndices) {
								if (index >= 0 && index < app->customLines.size()) {
									if (app->customLines[index].getIsDuplicate()) {
										AxisLock currentLock = app->customLines[index].getAxisLock();
										// Unlock X: LOCK_BOTH → LOCK_Y, LOCK_X → NONE
										if (currentLock == AxisLock::LOCK_BOTH) {
											app->customLines[index].setAxisLock(AxisLock::LOCK_Y);
										} else if (currentLock == AxisLock::LOCK_X) {
											app->customLines[index].setAxisLock(AxisLock::NONE);
										}
									}
								}
							}
							showContextMenu = false;
						}

						if (ImGui::MenuItem("Unlock Y All")) {
							for (int index : app->selectedLineIndices) {
								if (index >= 0 && index < app->customLines.size()) {
									if (app->customLines[index].getIsDuplicate()) {
										AxisLock currentLock = app->customLines[index].getAxisLock();
										// Unlock Y: LOCK_BOTH → LOCK_X, LOCK_Y → NONE
										if (currentLock == AxisLock::LOCK_BOTH) {
											app->customLines[index].setAxisLock(AxisLock::LOCK_X);
										} else if (currentLock == AxisLock::LOCK_Y) {
											app->customLines[index].setAxisLock(AxisLock::NONE);
										}
									}
								}
							}
							showContextMenu = false;
						}
					}
				}
			}
		}
	}

	ImGui::End();

	// Jika window ditutup (ESC atau klik di luar), reset state
	if (!showContextMenu) {
		currentType = NONE;
		hoveredDotPos = vec2(0, 0);
		hasHoveredDot = false;  // Reset flag
		hoveredPolygonIndex = -1;
		hoveredLineIndex = -1;
		isUserDotContext = false;  // Reset flag
	}
}

//--------------------------------------------------------------
void ContextMenu::showCopyPasteColorMenus(int selectedCount, ContextMenuType type) {
	// ===== MENU 1: Copy Color =====
	// Validasi: Hanya 1 selected DAN tidak ada mixed type selection
	bool canCopyColor = false;

	if (selectedCount == 1) {
		// Cek apakah ada mixed type selection
		if (type == DOT_CONTEXT) {
			// Copy dari DOT: pastikan tidak ada line atau polygon yang terseleksi
			canCopyColor = app->selectedLineIndices.empty() && app->selectedPolygonIndices.empty();
		}
		else if (type == POLYGON_CONTEXT) {
			// Copy dari POLYGON: pastikan tidak ada dot atau line yang terseleksi
			canCopyColor = app->selectedUserDotIndices.empty() && app->selectedLineIndices.empty();
		}
		else if (type == CUSTOMLINE_CONTEXT) {
			// Copy dari CUSTOMLINE: pastikan tidak ada dot atau polygon yang terseleksi
			canCopyColor = app->selectedUserDotIndices.empty() && app->selectedPolygonIndices.empty();
		}
	}

	if (canCopyColor) {
		// Enable hanya jika tepat 1 selected DAN tidak ada mixed type
		if (ImGui::MenuItem("Copy Color")) {
			// Copy color berdasarkan type
			if (type == DOT_CONTEXT) {
				app->copyDotColor();
			} else if (type == POLYGON_CONTEXT) {
				app->copyPolygonColor();
			} else if (type == CUSTOMLINE_CONTEXT) {
				app->copyLineColor();
			}
			showContextMenu = false;
		}
	} else {
		// Disabled jika bukan tepat 1 selected atau ada mixed type
		ImGui::BeginDisabled();
		ImGui::MenuItem("Copy Color");
		ImGui::EndDisabled();
	}

	// ===== MENU 2: Paste Color =====
	// Paste color boleh multi-type (tidak perlu validasi mixed type)
	// Validasi: Harus ada object yang terseleksi DAN ada color di clipboard
	bool canPaste = (selectedCount >= 1) && app->hasClipboardColor;

	if (canPaste) {
		// Enable jika ada minimal 1 selected DAN ada color di clipboard
		if (ImGui::MenuItem("Paste Color")) {
			// Paste color berdasarkan type
			if (type == DOT_CONTEXT) {
				app->pasteColorToDot();
			} else if (type == POLYGON_CONTEXT) {
				app->pasteColorToPolygon();
			} else if (type == CUSTOMLINE_CONTEXT) {
				app->pasteColorToLine();
			}
			showContextMenu = false;
		}
	} else {
		// Disabled jika tidak ada selected atau tidak ada color di clipboard
		ImGui::BeginDisabled();
		ImGui::MenuItem("Paste Color");
		ImGui::EndDisabled();
	}
}
