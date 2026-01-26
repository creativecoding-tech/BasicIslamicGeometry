#include "DialogViewListener.h"

// Static instance pointer
DialogViewListener* DialogViewListener::g_instance = nullptr;

// JS function callbacks untuk setiap action
static JSValueRef OnDialogCloseCallback(JSContextRef ctx, JSObjectRef function,
                                         JSObjectRef thisObject, size_t argc,
                                         const JSValueRef argv[], JSValueRef* exception) {
	if (DialogViewListener::g_instance && DialogViewListener::g_instance->jsCallback) {
		DialogViewListener::g_instance->jsCallback("onDialogClose");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnNextCallback(JSContextRef ctx, JSObjectRef function,
                                  JSObjectRef thisObject, size_t argc,
                                  const JSValueRef argv[], JSValueRef* exception) {
	if (DialogViewListener::g_instance && DialogViewListener::g_instance->jsCallback) {
		DialogViewListener::g_instance->jsCallback("onNext");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnBackCallback(JSContextRef ctx, JSObjectRef function,
                                  JSObjectRef thisObject, size_t argc,
                                  const JSValueRef argv[], JSValueRef* exception) {
	if (DialogViewListener::g_instance && DialogViewListener::g_instance->jsCallback) {
		DialogViewListener::g_instance->jsCallback("onBack");
	}
	return JSValueMakeUndefined(ctx);
}

static JSValueRef OnCreateCallback(JSContextRef ctx, JSObjectRef function,
                                   JSObjectRef thisObject, size_t argc,
                                   const JSValueRef argv[], JSValueRef* exception) {
	if (DialogViewListener::g_instance && DialogViewListener::g_instance->jsCallback) {
		DialogViewListener::g_instance->jsCallback("onCreate");
	}
	return JSValueMakeUndefined(ctx);
}

//--------------------------------------------------------------
void DialogViewListener::OnDOMReady(ultralight::View* caller,
                                      uint64_t frame_id,
                                      bool is_main_frame,
                                      const ultralight::String& url) {
	if (!is_main_frame) return;  // Hanya handle main frame

	// Lock JavaScript context
	ultralight::RefPtr<ultralight::JSContext> context = caller->LockJSContext();
	if (!context) return;

	JSContextRef ctx = context->ctx();
	JSObjectRef globalObject = JSContextGetGlobalObject(ctx);

	// Create JavaScript functions dengan signature yang benar (3 parameter saja)
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

	// Cleanup
	JSStringRelease(closeName);
	JSStringRelease(nextName);
	JSStringRelease(backName);
	JSStringRelease(createName);
	JSStringRelease(appName);
}
