#include "UltralightManager.h"

//--------------------------------------------------------------
UltralightManager::UltralightManager()
	: width(0)
	, height(0)
	, isInitialized(false) {
}

//--------------------------------------------------------------
UltralightManager::~UltralightManager() {
	cleanup();
}

//--------------------------------------------------------------
void UltralightManager::setup(int w, int h) {
	if (isInitialized) return;

	width = w;
	height = h;

	try {
		ofLogNotice("UltralightManager") << "Step 1: Creating Renderer...";
		// Initialize Ultralight
		initUltralight();
		ofLogNotice("UltralightManager") << "Step 2: Creating View...";
		// Create view
		createView(width, height);
		ofLogNotice("UltralightManager") << "Step 3: Allocating texture...";
		// Allocate texture
		texture.allocate(width, height, GL_RGBA);
		ofLogNotice("UltralightManager") << "Step 4: Setup complete!";
		isInitialized = true;
	} catch (const std::exception& e) {
		// Cleanup jika gagal
		view = nullptr;
		renderer = nullptr;

		// Re-throw agar caller tahu
		ofLogError("UltralightManager") << "Setup failed: " << e.what();
		throw;
	}
}

//--------------------------------------------------------------
void UltralightManager::setupPlatform() {
	// Setup Platform singleton dengan default handlers dari AppCore
	auto& platform = ultralight::Platform::instance();

	// Gunakan default implementations dari AppCore
	platform.set_font_loader(ultralight::GetPlatformFontLoader());
	platform.set_file_system(ultralight::GetPlatformFileSystem("."));
	platform.set_logger(ultralight::GetDefaultLogger("ultralight.log"));

	ofLogNotice("UltralightManager") << "Platform setup complete";
}

//--------------------------------------------------------------
void UltralightManager::initUltralight() {
	// Setup Platform handlers dulu
	setupPlatform();

	// Create Ultralight renderer
	renderer = ultralight::Renderer::Create();

	if (!renderer) {
		throw std::runtime_error("Failed to create Ultralight Renderer");
	}

	ofLogNotice("UltralightManager") << "Renderer created successfully";
}

//--------------------------------------------------------------
void UltralightManager::createView(int w, int h) {
	ultralight::ViewConfig viewConfig;
	viewConfig.is_accelerated = false;  // CPU renderer (BitmapSurface)
	view = renderer->CreateView(w, h, viewConfig, nullptr);

	if (!view) {
		throw std::runtime_error("Failed to create Ultralight View");
	}
}

//--------------------------------------------------------------
void UltralightManager::update() {
	if (!isInitialized || !renderer) return;

	// Update Ultralight renderer
	renderer->Update();
	renderer->Render();

	// Update OpenGL texture dari Ultralight surface
	updateTexture();
}

//--------------------------------------------------------------
void UltralightManager::updateTexture() {
	if (!view) return;

	ultralight::Surface* surface = view->surface();
	if (!surface) return;

	// Cast ke BitmapSurface
	ultralight::BitmapSurface* bitmapSurface = static_cast<ultralight::BitmapSurface*>(surface);

	// Get Bitmap
	ultralight::RefPtr<ultralight::Bitmap> bitmap = bitmapSurface->bitmap();
	if (!bitmap) return;

	// Lock pixels
	void* pixels = bitmap->LockPixels();

	int w = bitmap->width();
	int h = bitmap->height();
	unsigned char* src = (unsigned char*)pixels;

	// Cek apakah texture sudah di-allocate dengan ukuran yang benar
	if (texture.getWidth() != w || texture.getHeight() != h) {
		texture.allocate(w, h, GL_RGBA);
	}

	// Convert BGRA ke RGBA (swap R and B)
	ofPixels pix;
	pix.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
	unsigned char* dst = pix.getData();

	for (int i = 0; i < w * h; i++) {
		dst[i * 4 + 0] = src[i * 4 + 2];  // R = B
		dst[i * 4 + 1] = src[i * 4 + 1];  // G = G
		dst[i * 4 + 2] = src[i * 4 + 0];  // B = R
		dst[i * 4 + 3] = src[i * 4 + 3];  // A = A
	}

	texture.loadData(pix);

	bitmap->UnlockPixels();

	// Clear dirty bounds
	surface->ClearDirtyBounds();
}

//--------------------------------------------------------------
void UltralightManager::draw() {
	if (!isInitialized || !view) return;

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	texture.draw(0, 0);
	ofDisableBlendMode();
}

//--------------------------------------------------------------
void UltralightManager::loadHTML(const std::string& html) {
	if (!view) return;
	view->LoadHTML(html.c_str());
}

//--------------------------------------------------------------
void UltralightManager::loadHTMLFile(const std::string& path) {
	if (!view) return;
	string fullPath = ofToDataPath(path, true);
	string fileURL = "file:///" + fullPath;
	view->LoadURL(fileURL.c_str());
}

//--------------------------------------------------------------
void UltralightManager::fireMouseMove(int x, int y) {
	if (!view) return;
	ultralight::MouseEvent event;
	event.type = ultralight::MouseEvent::kType_MouseMoved;
	event.x = x;
	event.y = y;
	view->FireMouseEvent(event);
}

//--------------------------------------------------------------
void UltralightManager::fireMouseDown(int x, int y, int button) {
	if (!view) return;
	ultralight::MouseEvent event;
	event.type = ultralight::MouseEvent::kType_MouseDown;
	event.x = x;
	event.y = y;
	event.button = (ultralight::MouseEvent::Button)button;
	view->FireMouseEvent(event);
}

//--------------------------------------------------------------
void UltralightManager::fireMouseUp(int x, int y, int button) {
	if (!view) return;
	ultralight::MouseEvent event;
	event.type = ultralight::MouseEvent::kType_MouseUp;
	event.x = x;
	event.y = y;
	event.button = (ultralight::MouseEvent::Button)button;
	view->FireMouseEvent(event);
}

//--------------------------------------------------------------
void UltralightManager::fireMouseScroll(int x, int y, int scrollX, int scrollY) {
	if (!view) return;
	ultralight::ScrollEvent event;
	event.type = ultralight::ScrollEvent::kType_ScrollByPixel;
	event.delta_x = scrollX;
	event.delta_y = scrollY;
	view->FireScrollEvent(event);
}

//--------------------------------------------------------------
bool UltralightManager::isMouseOverUI(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}

//--------------------------------------------------------------
void UltralightManager::cleanup() {
	if (!isInitialized) return;

	view = nullptr;
	renderer = nullptr;

	isInitialized = false;
}
