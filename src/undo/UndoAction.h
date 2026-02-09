#pragma once

#include "ofMain.h"
#include "../shape/CustomLine.h"
#include "../shape/PolygonShape.h"
#include <vector>

/**
 * UndoAction Types
 *
 * Menentukan jenis operasi yang bisa di-undo/redo
 */
enum UndoActionType {
	CREATE_LINE,          // Create new customLine
	CREATE_POLYGON,       // Create new polygon
	CREATE_DOT,           // Create new userDot
	CHANGE_COLOR_LINE,    // Change customLine color
	CHANGE_COLOR_POLYGON, // Change polygon color
	CHANGE_COLOR_DOT,     // Change userDot color
	DELETE_LINE,          // Delete customLine
	DELETE_POLYGON,       // Delete polygon
	DELETE_DOT,           // Delete userDot
	CHANGE_CURVE          // Change line curve
};

/**
 * UndoAction
 *
 * Menyimpan data yang diperlukan untuk melakukan undo/redo operation.
 * Setiap action (create, delete, change color, change curve) dibungkus dalam UndoAction.
 */
struct UndoAction {
	UndoActionType type;         // Jenis operasi

	// For CREATE actions (semua CREATE_*)
	bool isCreate;                // Flag untuk create operations

	// For CHANGE_COLOR actions (support multi-select)
	std::vector<int> colorIndices;       // Index object yang diubah warnanya
	std::vector<ofColor> oldColors;      // Warna asli sebelum diubah
	ofColor newColor;                    // Warna baru yang diterapkan

	// For DELETE actions
	CustomLine deletedLine;              // CustomLine yang dihapus (line, curve, color, points)
	int deletedLineIndex;                // Index line yang dihapus

	PolygonShape deletedPolygon;         // Polygon yang dihapus (vertices, color)
	int deletedPolygonIndex;             // Index polygon yang dihapus

	vec2 deletedDotPos;                  // Posisi dot yang dihapus (untuk CREATE_DOT undo/redo)
	vec2 deletedDotLowerBound;            // LowerBound dot yang dihapus (untuk DELETE_DOT undo/redo)
	float deletedDotRadius;              // Radius dot yang dihapus (untuk DELETE_DOT undo/redo)
	int deletedDotIndex;                  // Index dot yang dihapus (untuk DELETE_DOT undo/redo)

	// For CHANGE_CURVE (support multi-select)
	std::vector<int> curveLineIndices;   // Index line yang diubah curvenya
	std::vector<float> oldCurves;          // Curve asli sebelum diubah
	float newCurve;                       // Curve baru yang diterapkan
};

/**
 * UndoStack Constants
 *
 * Konstanta untuk undo/redo stack management
 */
namespace UndoStack {
	static const int MAX_UNDO_STEPS = 100;  // Maksimum 100 undo steps
}
