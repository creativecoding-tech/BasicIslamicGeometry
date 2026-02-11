#include "SelectionInfo.h"
#include "../../ofApp.h"
#include <vector>

SelectionInfo::SelectionInfo(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
SelectionInfo::~SelectionInfo() {
}

//--------------------------------------------------------------
void SelectionInfo::focusWindow() {
	focusRequested = true;
}

//--------------------------------------------------------------
void SelectionInfo::showWindow() {
	windowOpen = true;
}

//--------------------------------------------------------------
void SelectionInfo::draw() {
	ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);

	// Center window di screen
	float screenWidth = ofGetWidth();
	float screenHeight = ofGetHeight();
	float windowWidth = 350.0f;
	float windowHeight = 300.0f;
	ImGui::SetNextWindowPos(ImVec2((screenWidth - windowWidth) / 2.0f, (screenHeight - windowHeight) / 2.0f), ImGuiCond_FirstUseEver);

	// Focus window jika di-request
	if (focusRequested) {
		ImGui::SetNextWindowFocus();
		focusRequested = false;
	}

	// Begin window dengan close button (windowOpen flag)
	if (ImGui::Begin("Selection Info", &windowOpen, ImGuiWindowFlags_None)) {
		// Cek apakah ada selection
		bool hasSelection = (app->selectionManager.hasSelectedUserDot() ||
							app->selectionManager.hasSelectedLine() ||
							app->selectionManager.hasSelectedPolygon());

		if (!hasSelection) {
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No objects selected");
		} else {
			// Tampilkan info untuk selected dots
			if (app->selectionManager.hasSelectedUserDot()) {
				ImGui::Text("Selected Dots: %d", app->selectionManager.getSelectedUserDotCount());

				const std::set<int>& indices = app->selectionManager.getSelectedUserDotIndices();
				for (auto it = indices.begin(); it != indices.end(); ++it) {
					int dotIndex = *it;
					if (dotIndex >= 0 && dotIndex < app->userDots.size()) {
						auto& dot = app->userDots[dotIndex];
						if (dot) {
							float radius = dot->getRadius();
							ofColor color = dot->getColor();
							vec2 dotPos = dot->getPosition();
							vec2 lowerBound = dot->getLowerBound();

							// Header: dot[index]
							ImGui::Bullet();
							ImGui::Text("dot[%d]", dotIndex);

							// Indent properties
							ImGui::Indent();

							// Info: radius
							ImGui::Text("radius: %.1f", radius);

							// Info: color
							ImGui::Text("color: rgba(%d, %d, %d, %d)",
								color.r, color.g, color.b, color.a);

							// Info: offset (horizontal atau vertical)
							if (dotPos.x != lowerBound.x) {
								// Horizontal offset
								float offsetX = dotPos.x - lowerBound.x;
								ImGui::Text("offset = %.1f", offsetX);
							} else {
								// Vertical offset
								float offsetY = dotPos.y - lowerBound.y;
								ImGui::Text("offset = %.1f", offsetY);
							}

							// Unindent
							ImGui::Unindent();
						}
					}
				}
				ImGui::Separator();
			}

			// Tampilkan info untuk selected lines
			if (app->selectionManager.hasSelectedLine()) {
				ImGui::Text("Selected Lines: %d", app->selectionManager.getSelectedLineCount());

				// Copy indices ke local vector untuk menghindari iterator invalidation
				std::vector<int> selectedLineIndices(app->selectionManager.getSelectedLineIndices().begin(),
				                                     app->selectionManager.getSelectedLineIndices().end());
				for (int lineIndex : selectedLineIndices) {
					if (lineIndex >= 0 && lineIndex < app->customLines.size()) {
						CustomLine& line = app->customLines[lineIndex];

						// Header: customLine[index] atau DcustomLine[index]
						ImGui::Bullet();
						if (line.getIsDuplicate()) {
							ImGui::Text("DcustomLine[%d]", lineIndex);
						} else {
							ImGui::Text("customLine[%d]", lineIndex);
						}

						// Indent properties
						ImGui::Indent();

						// Info: curve
						float curve = line.getCurve();
						ImGui::Text("curve: %.1f", curve);

						// Info: color
						ofColor color = line.getColor();
						ImGui::Text("color: rgba(%d, %d, %d, %d)",
							color.r, color.g, color.b, color.a);

						// Info: lock (hanya untuk DcustomLine)
						if (line.getIsDuplicate()) {
							AxisLock lockState = line.getAxisLock();
							const char* lockStr = "";
							switch (lockState) {
								case AxisLock::NONE: lockStr = "NONE"; break;
								case AxisLock::LOCK_X: lockStr = "LOCK_X"; break;
								case AxisLock::LOCK_Y: lockStr = "LOCK_Y"; break;
								case AxisLock::LOCK_BOTH: lockStr = "LOCK_BOTH"; break;
							}
							ImGui::Text("lock: %s", lockStr);
						}

						// Unindent
						ImGui::Unindent();
					}
				}
				ImGui::Separator();
			}

			// Tampilkan info untuk selected polygons
			if (app->selectionManager.hasSelectedPolygon()) {
				ImGui::Text("Selected Polygons: %d", app->selectionManager.getSelectedPolygonCount());

				const std::set<int>& indices = app->selectionManager.getSelectedPolygonIndices();
				for (auto it = indices.begin(); it != indices.end(); ++it) {
					int polyIndex = *it;
					if (polyIndex >= 0 && polyIndex < app->polygonShapes.size()) {
						PolygonShape& poly = app->polygonShapes[polyIndex];

						// Header: polygon[index]
						ImGui::Bullet();
						ImGui::Text("polygon[%d]", poly.getIndex());

						// Indent properties
						ImGui::Indent();

						// Info: vertices count
						int vertexCount = static_cast<int>(poly.getVertices().size());
						ImGui::Text("vertices: %d", vertexCount);

						// Info: color
						ofColor color = poly.getColor();
						ImGui::Text("color: rgba(%d, %d, %d, %d)",
							color.r, color.g, color.b, color.a);

						// Unindent
						ImGui::Unindent();
					}
				}
			}
		}
	}
	ImGui::End();

	// Sync window open state ke app
	app->showSelectionInfo = windowOpen;
}
