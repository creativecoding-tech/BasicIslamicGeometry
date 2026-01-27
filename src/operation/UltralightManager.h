#pragma once

#include "ofMain.h"
#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <AppCore/Platform.h>
#include <Ultralight/RefPtr.h>
#include "DialogViewListener.h"
#include <memory>

/**
 * UltralightManager
 *
 * Manages Ultralight UI Overlay untuk OpenFrameworks applications.
 * Class ini meng-handle semua Ultralight-related operations:
 * - Setup & initialization
 * - Update renderer
 * - Draw UI overlay
 * - Load HTML content
 * - Cleanup resources
 *
 * Usage:
 *   UltralightManager ui;
 *   ui.setup(ofGetWidth(), ofGetHeight());
 *   ui.loadHTML("<button>Click Me</button>");
 *
 *   // Di update()
 *   ui.update();
 *
 *   // Di draw()
 *   ui.draw();
 */
class UltralightManager {
public:
	UltralightManager();
	~UltralightManager();

	/**
	 * Initialize Ultralight SDK dan create view
	 * @param width Lebar window/view
	 * @param height Tinggi window/view
	 */
	void setup(int width, int height);

	/**
	 * Update Ultralight renderer
	 * Harus dipanggil setiap frame di ofApp::update()
	 */
	void update();

	/**
	 * Draw Ultralight UI overlay
	 * Harus dipanggil setiap frame di ofApp::draw() setelah menggambar geometry
	 */
	void draw();

	/**
	 * Cleanup Ultralight resources
	 * Harus dipanggil di ofApp::exit()
	 */
	void cleanup();

	/**
	 * Load HTML dari string
	 * @param html HTML content string
	 */
	void loadHTML(const std::string& html);

	/**
	 * Load HTML dari file
	 * @param path Path ke file HTML (relative ke bin/data/)
	 */
	void loadHTMLFile(const std::string& path);

	/**
	 * Forward mouse movement event ke Ultralight View
	 * @param x Mouse X position
	 * @param y Mouse Y position
	 */
	void fireMouseMove(int x, int y);

	/**
	 * Forward mouse button event ke Ultralight View
	 * @param x Mouse X position
	 * @param y Mouse Y position
	 * @param button Mouse button (0=left, 1=middle, 2=right)
	 */
	void fireMouseDown(int x, int y, int button);

	/**
	 * Forward mouse button release event ke Ultralight View
	 * @param x Mouse X position
	 * @param y Mouse Y position
	 * @param button Mouse button (0=left, 1=middle, 2=right)
	 */
	void fireMouseUp(int x, int y, int button);

	/**
	 * Forward mouse scroll event ke Ultralight View
	 * @param x Mouse X position
	 * @param y Mouse Y position
	 * @param scrollX Horizontal scroll delta
	 * @param scrollY Vertical scroll delta
	 */
	void fireMouseScroll(int x, int y, int scrollX, int scrollY);

	/**
	 * Cek apakah mouse position di dalam area Ultralight UI
	 * @param x Mouse X position
	 * @param y Mouse Y position
	 * @return true jika mouse di dalam area UI
	 */
	bool isMouseOverUI(int x, int y) const;

	/**
	 * Get UI width
	 */
	int getWidth() const { return width; }

	/**
	 * Get UI height
	 */
	int getHeight() const { return height; }

	/**
	 * Set JavaScript callback untuk handle button clicks dari HTML
	 * @param callback Function yang dipanggil ketika JavaScript trigger action
	 */
	void setJSCallback(std::function<void(const std::string&)> callback) {
		jsCallback = callback;
		hasJSCallback = true;
	}

	/**
	 * Bind JavaScript functions ke view (manual binding, tidak lewat OnDOMReady)
	 */
	void bindJSFunctions();

	// Static instance dan jsCallback untuk JavaScript callback access
	static UltralightManager* g_instance;
	std::function<void(const std::string&)> jsCallback;  // Public untuk static callback access

private:
	ultralight::RefPtr<ultralight::View> view;
	ultralight::RefPtr<ultralight::Renderer> renderer;
	std::unique_ptr<DialogViewListener> viewListener;  // Manage listener lifetime properly
	ofTexture texture;
	int width, height;
	bool isInitialized;
	bool hasJSCallback = false;  // Flag untuk track apakah ada JS callback

	/**
	 * Create/update OpenGL texture dari Ultralight surface
	 */
	void updateTexture();

	/**
	 * Initialize Ultralight config dan renderer
	 */
	void initUltralight();

	/**
	 * Setup Platform dengan FileSystem dan FontLoader
	 */
	void setupPlatform();

	/**
	 * Create view dengan ukuran tertentu
	 */
	void createView(int w, int h);
};
