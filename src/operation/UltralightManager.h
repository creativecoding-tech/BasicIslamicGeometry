#pragma once

#include "ofMain.h"
#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <AppCore/Platform.h>
#include <Ultralight/RefPtr.h>

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
	 * Get underlying Ultralight View
	 * Untuk advanced usage (JavaScript callbacks, dll)
	 */
	ultralight::RefPtr<ultralight::View> getView() { return view; }

private:
	ultralight::RefPtr<ultralight::View> view;
	ultralight::RefPtr<ultralight::Renderer> renderer;
	ofTexture texture;
	int width, height;
	bool isInitialized;

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
