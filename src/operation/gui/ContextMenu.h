#pragma once

#include "ofMain.h"
#include "AbstractGuiComponent.h"
#include <set>
using glm::vec2;

/**
 * ContextMenu - Right-click context menu system
 *
 * Menampilkan menu yang berbeda berdasarkan konteks:
 * - DOT_CONTEXT: Menu untuk duplicate dot (Above/Below/Left/Right)
 * - POLYGON_CONTEXT: Menu untuk duplicate polygon (Above/Below/Left/Right)
 * - CUSTOMLINE_CONTEXT: Menu untuk customLine yang terseleksi
 */
class ContextMenu : public AbstractGuiComponent {
public:
	ContextMenu(class ofApp* app);
	virtual ~ContextMenu() = default;

	// Type context menu
	enum ContextMenuType {
		NONE,              // Tidak ada context menu
		DOT_CONTEXT,       // Context menu untuk dot (Duplicate Dot)
		POLYGON_CONTEXT,   // Context menu untuk polygon (Duplicate Polygon)
		CUSTOMLINE_CONTEXT // Context menu untuk customLine yang terseleksi
	};

	// Interface dari AbstractGuiComponent
	void draw() override;

	// Control visibility
	void showWindow(ContextMenuType type, vec2 position);
	void hideWindow();

	// Check apakah context menu sedang visible
	bool isVisible() const { return showContextMenu; }

	// Get current context menu type
	ContextMenuType getCurrentType() const { return currentType; }

	// Set/get polygon index yang di-klik (untuk polygon context)
	void setHoveredPolygonIndex(int index) { hoveredPolygonIndex = index; }
	int getHoveredPolygonIndex() const { return hoveredPolygonIndex; }

	// Set/get dot position yang di-hover (untuk dot context)
	void setHoveredDotPos(vec2 pos) {
		hoveredDotPos = pos;
		hasHoveredDot = true;  // Set flag ketika ada dot yang di-hover
	}
	vec2 getHoveredDotPos() const { return hoveredDotPos; }
	bool getHasHoveredDot() const { return hasHoveredDot; }
	void resetHoveredDotState() {
		hoveredDotPos = vec2(0, 0);
		hasHoveredDot = false;
	}

	// Set/get apakah DOT_CONTEXT berasal dari userDot (duplicate dot) atau original dot
	void setIsUserDotContext(bool isUserDot) { isUserDotContext = isUserDot; }
	bool getIsUserDotContext() const { return isUserDotContext; }

	// Set/get line index yang di-klik (untuk customLine context)
	void setHoveredLineIndex(int index) { hoveredLineIndex = index; }
	int getHoveredLineIndex() const { return hoveredLineIndex; }

private:
	class ofApp* app;

	// Window state
	bool showContextMenu = false;
	vec2 contextMenuPos = vec2(0, 0);

	// Context menu type
	ContextMenuType currentType = NONE;

	// Dot context state
	vec2 hoveredDotPos = vec2(0, 0);
	bool isUserDotContext = false;  // True jika DOT_CONTEXT berasal dari userDot (duplicate dot)
	bool hasHoveredDot = false;  // True jika ada valid dot yang di-hover (untuk cek (0,0) bug)

	// Polygon context state
	int hoveredPolygonIndex = -1;

	// CustomLine context state
	int hoveredLineIndex = -1;

	// Helper function untuk Copy/Paste Color menu
	void showCopyPasteColorMenus(int selectedCount, ContextMenuType type);
};
