#pragma once

#include "ofMain.h"
#include <memory>

// Forward declarations
class ofApp;

using glm::vec2;

/**
 * InputManager
 *
 * Manages all input handling for mouse and keyboard events.
 * Delegates specific input logic to appropriate handlers.
 *
 * Features:
 * - Centralized input processing
 * - ImGui integration (input forwarding)
 * - Mouse event handlers (press, drag, release, scroll, move)
 * - Keyboard event handlers (press, release)
 */
class InputManager {
public:
	/**
	 * Constructor
	 * @param app Pointer to main ofApp instance
	 */
	InputManager(ofApp* app);

	/**
	 * Destructor
	 */
	~InputManager();

	// ========================================================================
	// MOUSE EVENT HANDLERS
	// ========================================================================

	/**
	 * Handle mouse button press
	 * @param x Mouse X position (screen coordinates)
	 * @param y Mouse Y position (screen coordinates)
	 * @param button Mouse button (0=left, 1=middle, 2=right)
	 */
	void handleMousePressed(int x, int y, int button);

	/**
	 * Handle mouse button release
	 * @param x Mouse X position (screen coordinates)
	 * @param y Mouse Y position (screen coordinates)
	 * @param button Mouse button (0=left, 1=middle, 2=right)
	 */
	void handleMouseReleased(int x, int y, int button);

	/**
	 * Handle mouse drag
	 * @param x Mouse X position (screen coordinates)
	 * @param y Mouse Y position (screen coordinates)
	 * @param button Mouse button (0=left, 1=middle, 2=right)
	 */
	void handleMouseDragged(int x, int y, int button);

	/**
	 * Handle mouse movement
	 * @param x Mouse X position (screen coordinates)
	 * @param y Mouse Y position (screen coordinates)
	 */
	void handleMouseMoved(int x, int y);

	/**
	 * Handle mouse scroll
	 * @param x Mouse X position (screen coordinates)
	 * @param y Mouse Y position (screen coordinates)
	 * @param scrollX Horizontal scroll amount
	 * @param scrollY Vertical scroll amount
	 */
	void handleMouseScrolled(int x, int y, float scrollX, float scrollY);

	// ========================================================================
	// KEYBOARD EVENT HANDLERS
	// ========================================================================

	/**
	 * Handle key press
	 * @param key Key code (OF_KEY_* or ASCII)
	 */
	void handleKeyPressed(int key);

	/**
	 * Handle key release
	 * @param key Key code (OF_KEY_* or ASCII)
	 */
	void handleKeyReleased(int key);

private:
	ofApp* app;  // Pointer to main app
};
