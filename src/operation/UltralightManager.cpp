#include "UltralightManager.h"
#include "DialogViewListener.h"

// Static instance pointer
UltralightManager* UltralightManager::g_instance = nullptr;

// JavaScript callback functions (file-scope static functions)
static JSValueRef OnDialogCloseCallback(JSContextRef ctx, JSObjectRef function,
                                        JSObjectRef thisObject, size_t argc,
                                        const JSValueRef argv[], JSValueRef* exception) {
	ofLogNotice("UltralightManager") << "JavaScript onDialogClose called!";
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("onDialogClose");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnNextCallback(JSContextRef ctx, JSObjectRef function,
                                 JSObjectRef thisObject, size_t argc,
                                 const JSValueRef argv[], JSValueRef* exception) {
	ofLogNotice("UltralightManager") << "JavaScript onNext called!";
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("onNext");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnBackCallback(JSContextRef ctx, JSObjectRef function,
                                 JSObjectRef thisObject, size_t argc,
                                 const JSValueRef argv[], JSValueRef* exception) {
	ofLogNotice("UltralightManager") << "JavaScript onBack called!";
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("onBack");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnCreateCallback(JSContextRef ctx, JSObjectRef function,
                                  JSObjectRef thisObject, size_t argc,
                                  const JSValueRef argv[], JSValueRef* exception) {
	ofLogNotice("UltralightManager") << "JavaScript onCreate called!";
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("onCreate");
	}
	return JSValueMakeUndefined(ctx);
}

//--------------------------------------------------------------
UltralightManager::UltralightManager()
	: width(0)
	, height(0)
	, isInitialized(false)
	, hasJSCallback(false) {
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

	ofLogNotice("UltralightManager") << "hasJSCallback: " << hasJSCallback;

	// Create View dulu (session = nullptr untuk default session)
	view = renderer->CreateView(w, h, viewConfig, nullptr);

	if (!view) {
		throw std::runtime_error("Failed to create Ultralight View");
	}

	// Set ViewListener dengan callback kalau dibutuhkan
	if (hasJSCallback && jsCallback) {
		ofLogNotice("UltralightManager") << "Creating and setting ViewListener...";
		DialogViewListener* dialogListener = new DialogViewListener(jsCallback);
		view->set_view_listener(dialogListener);
		ofLogNotice("UltralightManager") << "ViewListener set successfully!";
	} else {
		ofLogNotice("UltralightManager") << "No JS callback, skipping ViewListener";
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
	if (!view) {
		ofLogError("UltralightManager") << "Cannot load HTML - view is null!";
		return;
	}
	string fullPath = ofToDataPath(path, true);
	string fileURL = "file:///" + fullPath;
	ofLogNotice("UltralightManager") << "Loading HTML file: " << fileURL;
	view->LoadURL(fileURL.c_str());
}

//--------------------------------------------------------------
void UltralightManager::bindJSFunctions() {
	if (!view || !hasJSCallback) {
		ofLogError("UltralightManager") << "Cannot bind JS - view or callback not ready!";
		return;
	}

	ofLogNotice("UltralightManager") << "Binding JavaScript functions...";

	// Set instance pointer supaya static functions bisa akses jsCallback
	g_instance = this;

	// Lock JavaScript context
	ultralight::RefPtr<ultralight::JSContext> context = view->LockJSContext();

	if (!context) {
		ofLogError("UltralightManager") << "Failed to lock JS context!";
		return;
	}

	JSContextRef ctx = context->ctx();
	JSObjectRef globalObject = JSContextGetGlobalObject(ctx);

	// Create JavaScript functions dari file-scope static callbacks
	JSStringRef closeName = JSStringCreateWithUTF8CString("onDialogClose");
	JSObjectRef closeFunc = JSObjectMakeFunctionWithCallback(ctx, closeName, &OnDialogCloseCallback);

	JSStringRef nextName = JSStringCreateWithUTF8CString("onNext");
	JSObjectRef nextFunc = JSObjectMakeFunctionWithCallback(ctx, nextName, &OnNextCallback);

	JSStringRef backName = JSStringCreateWithUTF8CString("onBack");
	JSObjectRef backFunc = JSObjectMakeFunctionWithCallback(ctx, backName, &OnBackCallback);

	JSStringRef createName = JSStringCreateWithUTF8CString("onCreate");
	JSObjectRef createFunc = JSObjectMakeFunctionWithCallback(ctx, createName, &OnCreateCallback);

	// Create app object
	JSStringRef appName = JSStringCreateWithUTF8CString("app");
	JSObjectRef appObject = JSObjectMake(ctx, nullptr, nullptr);

	// Set properties to app object
	JSObjectSetProperty(ctx, appObject, closeName, closeFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, nextName, nextFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, backName, backFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, createName, createFunc, kJSPropertyAttributeNone, nullptr);

	// Set app object to global object (window.app)
	JSObjectSetProperty(ctx, globalObject, appName, appObject, kJSPropertyAttributeNone, nullptr);

	ofLogNotice("UltralightManager") << "JavaScript functions bound successfully!";

	// Cleanup
	JSStringRelease(closeName);
	JSStringRelease(nextName);
	JSStringRelease(backName);
	JSStringRelease(createName);
	JSStringRelease(appName);
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
	ofLogNotice("UltralightManager") << "Mouse clicked at: " << x << ", " << y;
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
