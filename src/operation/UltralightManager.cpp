#include "UltralightManager.h"
#include "DialogViewListener.h"

// Static members for SharedRendererManager
ultralight::RefPtr<ultralight::Renderer> SharedRendererManager::renderer = nullptr;
bool SharedRendererManager::isInitialized = false;

//--------------------------------------------------------------
ultralight::RefPtr<ultralight::Renderer> SharedRendererManager::getInstance() {
	if (!isInitialized) {
		// Setup Platform handlers dulu (sekali saja)
		auto& platform = ultralight::Platform::instance();
		platform.set_font_loader(ultralight::GetPlatformFontLoader());
		platform.set_file_system(ultralight::GetPlatformFileSystem("."));
		// Logging di-disable untuk performance (file I/O bikin lambat)
		// platform.set_logger(ultralight::GetDefaultLogger("ultralight.log"));

		// Create singleton Renderer
		renderer = ultralight::Renderer::Create();
		if (!renderer) {
			throw std::runtime_error("Failed to create shared Ultralight Renderer");
		}

		isInitialized = true;
	}

	return renderer;
}

//--------------------------------------------------------------
void SharedRendererManager::cleanup() {
	if (isInitialized && renderer) {
		renderer = nullptr;
		isInitialized = false;
	}
}

// Static instance pointer
UltralightManager* UltralightManager::g_instance = nullptr;

// JavaScript callback functions (file-scope static functions)
// DIPERBAIKI: Null-safe checks untuk mencegah crash saat cleanup/shutdown
static JSValueRef OnDialogCloseCallback(JSContextRef ctx, JSObjectRef function,
                                        JSObjectRef thisObject, size_t argc,
                                        const JSValueRef argv[], JSValueRef* exception) {
	// CRITICAL: Check null pointer SEBELUM akses apapun
	// Ini mencegah crash saat cleanup/shutdown race condition
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("onDialogClose");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnBackCallback(JSContextRef ctx, JSObjectRef function,
                                 JSObjectRef thisObject, size_t argc,
                                 const JSValueRef argv[], JSValueRef* exception) {
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("onBack");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnCreateCallback(JSContextRef ctx, JSObjectRef function,
                                  JSObjectRef thisObject, size_t argc,
                                  const JSValueRef argv[], JSValueRef* exception) {
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("onCreate");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef On2DModeCallback(JSContextRef ctx, JSObjectRef function,
                                    JSObjectRef thisObject, size_t argc,
                                    const JSValueRef argv[], JSValueRef* exception) {
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("on2DMode");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef On3DModeCallback(JSContextRef ctx, JSObjectRef function,
                                    JSObjectRef thisObject, size_t argc,
                                    const JSValueRef argv[], JSValueRef* exception) {
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		UltralightManager::g_instance->jsCallback("on3DMode");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef ToggleLabelsCallback(JSContextRef ctx, JSObjectRef function,
                                     JSObjectRef thisObject, size_t argc,
                                     const JSValueRef argv[], JSValueRef* exception) {
	ofLogNotice("UltralightManager") << "ToggleLabelsCallback called";
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		ofLogNotice("UltralightManager") << "Calling jsCallback with toggleLabels";
		UltralightManager::g_instance->jsCallback("toggleLabels");
	} else {
		ofLogNotice("UltralightManager") << "g_instance or jsCallback is null!";
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef ToggleDotsCallback(JSContextRef ctx, JSObjectRef function,
                                    JSObjectRef thisObject, size_t argc,
                                    const JSValueRef argv[], JSValueRef* exception) {
	ofLogNotice("UltralightManager") << "ToggleDotsCallback called";
	if (UltralightManager::g_instance && UltralightManager::g_instance->jsCallback) {
		ofLogNotice("UltralightManager") << "Calling jsCallback with toggleDots";
		UltralightManager::g_instance->jsCallback("toggleDots");
	} else {
		ofLogNotice("UltralightManager") << "g_instance or jsCallback is null!";
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
		// Initialize Ultralight
		initUltralight();
		// Create view
		createView(width, height);
		// Allocate texture
		texture.allocate(width, height, GL_RGBA);
		isInitialized = true;
	} catch (const std::exception&) {
		// Cleanup jika gagal
		view = nullptr;
		viewListener.reset();

		// Re-throw agar caller tahu (logging dihapus untuk performance)
		throw;
	}
}

//--------------------------------------------------------------
void UltralightManager::setupPlatform() {
	// Platform setup sekarang di-handle oleh SharedRendererManager::getInstance()
	// Tidak perlu setup lagi di sini
}

//--------------------------------------------------------------
void UltralightManager::initUltralight() {
	// Renderer setup sekarang di-handle oleh SharedRendererManager
	// Tidak perlu create renderer sendiri lagi
	// Renderer akan di-share di antara semua UltralightManager instances
}

//--------------------------------------------------------------
void UltralightManager::createView(int w, int h) {
	ultralight::ViewConfig viewConfig;
	viewConfig.is_accelerated = false;  // CPU renderer (BitmapSurface)

	// Get shared renderer (singleton)
	ultralight::RefPtr<ultralight::Renderer> sharedRenderer = SharedRendererManager::getInstance();

	// Create View dengan shared renderer
	view = sharedRenderer->CreateView(w, h, viewConfig, nullptr);

	if (!view) {
		throw std::runtime_error("Failed to create Ultralight View");
	}

	// Create dan set ViewListener dengan callback kalau dibutuhkan
	// Disimpan di member variable agar lifetime-nya jelas dan otomatis di-cleanup
	if (hasJSCallback && jsCallback) {
		viewListener = std::make_unique<DialogViewListener>(jsCallback);
		view->set_view_listener(viewListener.get());
	}
}

//--------------------------------------------------------------
void UltralightManager::update() {
	// Skip update jika tidak initialized atau sedang shutdown
	if (!isInitialized || !view) return;

	// Update shared renderer (semua UltralightManager instances pakai renderer yang sama)
	ultralight::RefPtr<ultralight::Renderer> sharedRenderer = SharedRendererManager::getInstance();
	if (!sharedRenderer) return;

	// Update Ultralight renderer
	sharedRenderer->Update();
	sharedRenderer->Render();

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
void UltralightManager::evaluateJavaScript(const std::string& script) {
	if (!view) {
		ofLogNotice("UltralightManager") << "evaluateJavaScript: view is null!";
		return;
	}
	ultralight::RefPtr<ultralight::JSContext> context = view->LockJSContext();
	if (!context) {
		ofLogNotice("UltralightManager") << "evaluateJavaScript: context is null!";
		return;
	}

	ofLogNotice("UltralightManager") << "evaluateJavaScript: " << script;
	JSContextRef ctx = context->ctx();
	JSStringRef scriptRef = JSStringCreateWithUTF8CString(script.c_str());

	// Evaluate script tanpa mempedulikan return value
	JSEvaluateScript(ctx, scriptRef, nullptr, nullptr, 0, nullptr);

	JSStringRelease(scriptRef);
}

//--------------------------------------------------------------
void UltralightManager::bindJSFunctions() {
	if (!view || !hasJSCallback) {
		ofLogNotice("UltralightManager") << "bindJSFunctions() early return: view=" << (view.get() != nullptr) << " hasJSCallback=" << hasJSCallback;
		return;
	}

	ofLogNotice("UltralightManager") << "bindJSFunctions() called, binding functions...";
	// Set instance pointer supaya static functions bisa akses jsCallback
	g_instance = this;

	// Lock JavaScript context
	ultralight::RefPtr<ultralight::JSContext> context = view->LockJSContext();
	if (!context) return;

	JSContextRef ctx = context->ctx();
	JSObjectRef globalObject = JSContextGetGlobalObject(ctx);

	// Create JavaScript functions dari file-scope static callbacks
	JSStringRef closeName = JSStringCreateWithUTF8CString("onDialogClose");
	JSObjectRef closeFunc = JSObjectMakeFunctionWithCallback(ctx, closeName, &OnDialogCloseCallback);

	JSStringRef backName = JSStringCreateWithUTF8CString("onBack");
	JSObjectRef backFunc = JSObjectMakeFunctionWithCallback(ctx, backName, &OnBackCallback);

	JSStringRef createName = JSStringCreateWithUTF8CString("onCreate");
	JSObjectRef createFunc = JSObjectMakeFunctionWithCallback(ctx, createName, &OnCreateCallback);

	JSStringRef mode2DName = JSStringCreateWithUTF8CString("on2DMode");
	JSObjectRef mode2DFunc = JSObjectMakeFunctionWithCallback(ctx, mode2DName, &On2DModeCallback);

	JSStringRef mode3DName = JSStringCreateWithUTF8CString("on3DMode");
	JSObjectRef mode3DFunc = JSObjectMakeFunctionWithCallback(ctx, mode3DName, &On3DModeCallback);

	JSStringRef toggleLabelsName = JSStringCreateWithUTF8CString("toggleLabels");
	JSObjectRef toggleLabelsFunc = JSObjectMakeFunctionWithCallback(ctx, toggleLabelsName, &ToggleLabelsCallback);

	JSStringRef toggleDotsName = JSStringCreateWithUTF8CString("toggleDots");
	JSObjectRef toggleDotsFunc = JSObjectMakeFunctionWithCallback(ctx, toggleDotsName, &ToggleDotsCallback);

	// Create app object
	JSStringRef appName = JSStringCreateWithUTF8CString("app");
	JSObjectRef appObject = JSObjectMake(ctx, nullptr, nullptr);

	// Set properties to app object
	JSObjectSetProperty(ctx, appObject, closeName, closeFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, backName, backFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, createName, createFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, mode2DName, mode2DFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, mode3DName, mode3DFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, toggleLabelsName, toggleLabelsFunc, kJSPropertyAttributeNone, nullptr);
	JSObjectSetProperty(ctx, appObject, toggleDotsName, toggleDotsFunc, kJSPropertyAttributeNone, nullptr);

	// Set app object to global object (window.app)
	JSObjectSetProperty(ctx, globalObject, appName, appObject, kJSPropertyAttributeNone, nullptr);

	// Cleanup
	JSStringRelease(closeName);
	JSStringRelease(backName);
	JSStringRelease(createName);
	JSStringRelease(mode2DName);
	JSStringRelease(mode3DName);
	JSStringRelease(toggleLabelsName);
	JSStringRelease(toggleDotsName);
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
	// Double-check untuk prevent double-cleanup
	if (!isInitialized) return;

	// STEP 1: Matikan semua callbacks SEBELUM apapun (CRITICAL!)
	// Ini mencegah race condition dari JavaScript callbacks yang mungkin terjadi
	// selama shutdown. Static pointer di-reset ke nullptr agar callback functions
	// tidak mencoba akses object yang sudah di-destroy.
	g_instance = nullptr;
	DialogViewListener::resetInstance();

	// STEP 2: Disconnect listener dari view SEGERA
	// Ini memutus koneksi antara view dan listener sehingga tidak ada callback
	// yang bisa terjadi selama proses shutdown.
	if (view) {
		view->set_view_listener(nullptr);
	}

	// STEP 3: Delete ViewListener object
	// unique_ptr akan otomatis menghapus memory
	viewListener.reset();

	// STEP 4: Release view
	// View di-release, tapi tidak perlu release renderer karena renderer
	// sekarang adalah SHARED singleton yang di-manage oleh SharedRendererManager
	view = nullptr;

	// NOTE: Renderer TIDAK di-release di sini karena:
	// - Renderer sekarang adalah SHARED singleton (SharedRendererManager)
	// - Renderer akan di-cleanup sekali saja di aplikasi shutdown
	// - Multiple UltralightManager instances share 1 renderer yang sama

	// Mark as not initialized
	isInitialized = false;
}
