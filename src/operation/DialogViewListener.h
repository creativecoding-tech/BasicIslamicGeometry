#pragma once

#include "ofMain.h"
#include <Ultralight/Ultralight.h>
#include <JavaScriptCore/JavaScript.h>
#include <functional>

/**
 * ViewListener untuk Dialog UI dengan JavaScript bridge
 *
 * Class ini handle OnDOMReady dan bind C++ functions ke JavaScript
 * sehingga HTML bisa call C++ methods seperti app.onDialogClose()
 */
class DialogViewListener : public ultralight::ViewListener {
public:
	// Callback function type untuk JavaScript calls
	typedef std::function<void(const std::string&)> JSCallback;

	JSCallback jsCallback;  // Make public biar bisa diakses
	static DialogViewListener* g_instance;  // Static instance public untuk static callbacks

	DialogViewListener(JSCallback callback)
		: jsCallback(callback) {
		g_instance = this;
	}

	virtual ~DialogViewListener() {
		g_instance = nullptr;
	}

	// Dipanggil saat DOM ready - bind JavaScript functions di sini
	virtual void OnDOMReady(ultralight::View* caller,
	                        uint64_t frame_id,
	                        bool is_main_frame,
	                        const ultralight::String& url);
};
