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
	hoveredPolygonIndex = -1;
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

		// Context menu untuk DOT (Duplicate Dot)
		if (currentType == DOT_CONTEXT) {
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
		// Context menu untuk POLYGON (Duplicate Polygon)
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
		}
		// Context menu untuk CUSTOMLINE (Duplicate Line)
		else if (currentType == CUSTOMLINE_CONTEXT) {
			if (ImGui::MenuItem("Duplicate Line Above")) {
				// TODO: Implement event untuk duplicate line above
				showContextMenu = false;
			}
			if (ImGui::MenuItem("Duplicate Line Below")) {
				// TODO: Implement event untuk duplicate line below
				showContextMenu = false;
			}
			if (ImGui::MenuItem("Duplicate Line Left")) {
				// TODO: Implement event untuk duplicate line left
				showContextMenu = false;
			}
			if (ImGui::MenuItem("Duplicate Line Right")) {
				// TODO: Implement event untuk duplicate line right
				showContextMenu = false;
			}
		}
	}

	ImGui::End();

	// Jika window ditutup (ESC atau klik di luar), reset state
	if (!showContextMenu) {
		currentType = NONE;
		hoveredDotPos = vec2(0, 0);
		hoveredPolygonIndex = -1;
	}
}
