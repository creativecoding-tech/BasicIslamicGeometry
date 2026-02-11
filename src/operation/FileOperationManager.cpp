#include "FileOperationManager.h"
#include "../ofApp.h"
#include "gui/Playground.h"

//--------------------------------------------------------------
FileOperationManager::FileOperationManager(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
void FileOperationManager::saveWorkspace() {
	if (!app->currentTemplate) {
		return;
	}

	// Kalau belum pernah Save As, langsung buka dialog (Save As)
	if (app->lastSavedPath.empty()) {
		saveWorkspaceAs();
		return;
	}

	// Sudah ada Save As sebelumnya, save langsung ke file tersebut
	app->fileManager.saveAll(app->currentTemplate->getName(), app->currentTemplate->radius,
	                      app->customLines, app->polygonShapes, app->userDots, app->currentTemplate->lineWidth,
	                      app->currentTemplate->labelsVisible, app->currentTemplate->dotsVisible,
	                      app->showUserDot, app->lastSavedPath);

	// Update info jumlah customLines dan polygons (untuk CollapsingHeader)
	app->loadedFileCustomLinesCount = static_cast<int>(app->customLines.size());
	app->loadedFilePolygonCount = static_cast<int>(app->polygonShapes.size());

	// Show MenuBar agar popup terlihat
	app->imguiVisible = true;
	app->successPopup->show();
}

//--------------------------------------------------------------
void FileOperationManager::saveWorkspaceAs() {
	if (!app->currentTemplate) {
		return;  // Tidak ada template aktif
	}

	// Buka save dialog untuk pilih lokasi dan nama file
	ofFileDialogResult saveDialog = ofSystemSaveDialog("workspace.nay", "Save Workspace As");

	// Cek apakah user memilih file (tidak cancel)
	if (saveDialog.getPath().empty()) {
		return;  // User cancel
	}

	// Ambil filepath dari dialog
	std::string filepath = saveDialog.getPath();

	// Tambah extension .nay kalau belum ada
	if (filepath.find(".nay") == std::string::npos) {
		filepath += ".nay";
	}

	// Simpan langsung ke filepath yang user pilih
	app->fileManager.saveAll(app->currentTemplate->getName(), app->currentTemplate->radius,
	                      app->customLines, app->polygonShapes, app->userDots, app->currentTemplate->lineWidth,
	                      app->currentTemplate->labelsVisible, app->currentTemplate->dotsVisible,
	                      app->showUserDot, filepath);

	// Simpan path ini sebagai lastSavedPath (CTRL+S selanjutnya akan kesini)
	app->lastSavedPath = filepath;

	// Update info jumlah customLines dan polygons (untuk CollapsingHeader)
	app->loadedFileCustomLinesCount = static_cast<int>(app->customLines.size());
	app->loadedFilePolygonCount = static_cast<int>(app->polygonShapes.size());

	// Show MenuBar agar popup terlihat
	app->imguiVisible = true;
	app->successPopup->show();
}

//--------------------------------------------------------------
void FileOperationManager::openWorkspace() {
	// Buka open file dialog
	ofFileDialogResult openDialog = ofSystemLoadDialog("Open Workspace", false);

	// Cek apakah user memilih file (tidak cancel)
	if (openDialog.getPath().empty()) {
		return;  // User cancel
	}

	// Ambil filepath dari dialog
	std::string filepath = openDialog.getPath();

	// Cek apakah file extension .nay
	if (filepath.find(".nay") == std::string::npos) {
		// File bukan format .nay, tampilkan error popup
		// Show MenuBar agar popup terlihat
		app->imguiVisible = true;
		app->showSacredGeometry = false;
		app->showPlayground = false;
		app->errorPopup->show("Invalid File Format",
		                     "Please select a .nay file format!",
		                     "OK");
		return;
	}

	// Set lastSavedPath ke filepath yang di-open (untuk load dan save)
	app->lastSavedPath = filepath;

	// Cek jumlah customLines di file (untuk CollapsingHeader di playground)
	peekFileCustomLinesCount(filepath, app->loadedFileCustomLinesCount);

	// Cek jumlah polygons di file (untuk CollapsingHeader di playground)
	peekFilePolygonCount(filepath, app->loadedFilePolygonCount);

	// Show dan focus Playground window (file valid, akan diload)
	app->imguiVisible = true;
	app->showPlayground = true;

	// Set windowOpen flag dan focus ke Playground
	for (auto& gui : app->guiComponents) {
		Playground* playground = dynamic_cast<Playground*>(gui.get());
		if (playground) {
			playground->showWindow();
		}
	}
}

//--------------------------------------------------------------
void FileOperationManager::loadWorkspace() {
	// NOTE: Validasi canvas sudah dilakukan di UI level (Playground)
	// Jangan cek lagi di sini agar autoCleanCanvas bisa berfungsi

	// Cek apakah ada file yang di-open (lastSavedPath)
	if (app->lastSavedPath.empty()) return;

	// Load workspace dengan template name, radius, dan ALL settings
	std::string loadedTemplateName;
	float loadedRadius;
	float loadedLineWidth;
	bool loadedLabelsVisible;
	bool loadedDotsVisible;

	if (app->fileManager.loadAll(loadedTemplateName, loadedRadius, app->customLines,
	    app->polygonShapes, app->userDots, loadedLineWidth,
	    loadedLabelsVisible, loadedDotsVisible, app->showUserDot, app->lastSavedPath)) {

		// Switch ke template yang di-load DULU
		app->switchTemplate(loadedTemplateName);

		// Rebuild shapes dengan loaded radius yang benar!
		app->currentTemplate->setRadius(loadedRadius);

		// Update tracking untuk scaling
		app->previousRadius = loadedRadius;  // Reset tracking agar tidak scaling saat load

		// Sync Settings ke template
		app->currentTemplate->lineWidth = loadedLineWidth;
		app->currentTemplate->labelsVisible = loadedLabelsVisible;
		app->currentTemplate->dotsVisible = loadedDotsVisible;

		// Apply settings ke semua shapes yang baru di-load
		const auto& shapes = app->currentTemplate->getShapes();
		for (auto& shape : shapes) {
			shape->setRadius(app->currentTemplate->radius);  // Update radius dengan loadedRadius!
			shape->setLineWidth(app->currentTemplate->lineWidth);

			if (app->currentTemplate->labelsVisible)
				shape->showLabel();
			else
				shape->hideLabel();

			if (app->currentTemplate->dotsVisible)
				shape->showDot();
			else
				shape->hideDot();
		}

		// Reset dots cache agar di-rebuild
		app->dotsCacheDirty = true;

		// Reset canvas transform
		app->canvasTranslation = vec2(0, 0);
		app->canvasRotation = 0.0f;
		app->canvasZoom = 1.0f;

		app->successPopup->show("Workspace Loaded Successfully!",
		                         "File: " + app->lastSavedPath,
		                         "OK");
	} else {
		app->errorPopup->show("Failed to Load Workspace",
		                     "Invalid file format or corrupted data!",
		                     "OK");
	}
}

//--------------------------------------------------------------
void FileOperationManager::loadWorkspaceSeq() {
	// NOTE: Validasi canvas sudah dilakukan di UI level (Playground)
	// Jangan cek lagi di sini agar autoCleanCanvas bisa berfungsi

	// Cek apakah ada file yang di-open (lastSavedPath)
	if (app->lastSavedPath.empty()) return;

	// Load workspace sequential dengan template name, radius, dan ALL settings
	std::string loadedTemplateName;
	float loadedRadius;
	float loadedLineWidth;
	bool loadedLabelsVisible;
	bool loadedDotsVisible;

	app->fileManager.loadAllSequential(loadedTemplateName, loadedRadius,
	                                  loadedLineWidth, loadedLabelsVisible, loadedDotsVisible,
	                                  app->customLines, app->polygonShapes, app->userDots,
	                                  app->showUserDot, app->lastSavedPath);

	// Switch ke template yang di-load DULU
	app->switchTemplate(loadedTemplateName);

	// Rebuild shapes dengan loaded radius yang benar!
	app->currentTemplate->setRadius(loadedRadius);

	// Update tracking untuk scaling
	app->previousRadius = loadedRadius;  // Reset tracking agar tidak scaling saat load

	// Sync Settings ke template
	app->currentTemplate->lineWidth = loadedLineWidth;
	app->currentTemplate->labelsVisible = loadedLabelsVisible;
	app->currentTemplate->dotsVisible = loadedDotsVisible;

	// Apply settings ke semua shapes yang baru di-load
	const auto& shapes = app->currentTemplate->getShapes();
	for (auto& shape : shapes) {
		shape->setRadius(app->currentTemplate->radius);  // Update radius dengan loadedRadius!
		shape->setLineWidth(app->currentTemplate->lineWidth);

		if (app->currentTemplate->labelsVisible)
			shape->showLabel();
		else
			shape->hideLabel();

		if (app->currentTemplate->dotsVisible)
			shape->showDot();
		else
			shape->hideDot();
	}

	// Reset dots cache agar di-rebuild
	app->dotsCacheDirty = true;

	// Reset canvas transform
	app->canvasTranslation = vec2(0, 0);
	app->canvasRotation = 0.0f;
	app->canvasZoom = 1.0f;

	// Sequential load akan mengupdate customLines dan polygons secara bertahap di update()
	// Update state ke SEQUENTIAL_DRAWING untuk enable sequential animation
	app->currentState = ofApp::UpdateState::SEQUENTIAL_DRAWING;

	app->successPopup->show("Sequential Load Started!",
	                         "Drawing shapes one by one...",
	                         "OK");
}

//--------------------------------------------------------------
bool FileOperationManager::peekFileCustomLinesCount(const std::string& filepath, int& outCount) {
	// Baca file header saja untuk mendapatkan jumlah customLines
	ofFile file(filepath);
	if (!file.exists()) {
		outCount = 0;
		return false;
	}

	// Read file ke buffer
	ofBuffer buffer = ofBufferFromFile(filepath);
	char* data = buffer.getData();
	size_t bufferSize = buffer.size();

	if (bufferSize < 64) {
		outCount = 0;
		return false;
	}

	size_t offset = 0;

	// Validasi Magic Number
	if (memcmp(data + offset, "NA01", 4) != 0) {
		outCount = 0;
		return false;
	}
	offset += 4;

	// Skip Version (4 bytes)
	offset += sizeof(int);

	// Skip Template Name Length (4 bytes) + Template Name
	int nameLen = *reinterpret_cast<int*>(data + offset);
	offset += sizeof(int) + nameLen;

	// Skip Global Radius (4 bytes)
	offset += sizeof(float);

	// Skip Additional Settings (20 bytes untuk v1, atau float + 3 bool + 13 bytes untuk v2)
	offset += 20;

	// Baca jumlah Custom Lines
	if (offset + sizeof(int) > bufferSize) {
		outCount = 0;
		return false;
	}

	int numLines = *reinterpret_cast<int*>(data + offset);
	outCount = numLines;

	return true;
}

//--------------------------------------------------------------
bool FileOperationManager::peekFilePolygonCount(const std::string& filepath, int& outCount) {
	// Baca file header saja untuk mendapatkan jumlah polygons
	ofFile file(filepath);
	if (!file.exists()) {
		outCount = 0;
		return false;
	}

	// Read file ke buffer
	ofBuffer buffer = ofBufferFromFile(filepath);
	char* data = buffer.getData();
	size_t bufferSize = buffer.size();

	if (bufferSize < 64) {
		outCount = 0;
		return false;
	}

	size_t offset = 0;

	// Validasi Magic Number
	if (memcmp(data + offset, "NA01", 4) != 0) {
		outCount = 0;
		return false;
	}
	offset += 4;

	// Skip Version (4 bytes)
	offset += sizeof(int);

	// Skip Template Name Length (4 bytes) + Template Name
	int nameLen = *reinterpret_cast<int*>(data + offset);
	offset += sizeof(int) + nameLen;

	// Skip Global Radius (4 bytes)
	offset += sizeof(float);

	// Skip Additional Settings (20 bytes)
	offset += 20;

	// Read jumlah Custom Lines
	if (offset + sizeof(int) > bufferSize) {
		outCount = 0;
		return false;
	}
	int numLines = *reinterpret_cast<int*>(data + offset);
	offset += sizeof(int);

	// Skip semua Custom Lines data
	for (int i = 0; i < numLines; i++) {
		// Skip numPoints (4 bytes)
		if (offset + sizeof(int) > bufferSize) {
			outCount = 0;
			return false;
		}
		int numPoints = *reinterpret_cast<int*>(data + offset);
		offset += sizeof(int);

		// Skip points (numPoints * sizeof(vec2))
		if (offset + numPoints * sizeof(vec2) > bufferSize) {
			outCount = 0;
			return false;
		}
		offset += numPoints * sizeof(vec2);

		// Skip color (4 bytes), lineWidth (4 bytes), curve (4 bytes)
		if (offset + 12 > bufferSize) {
			outCount = 0;
			return false;
		}
		offset += 12;

		// Skip label string (length + data)
		if (offset + sizeof(int) > bufferSize) {
			outCount = 0;
			return false;
		}
		int labelLength = *reinterpret_cast<int*>(data + offset);
		offset += sizeof(int);
		if (offset + labelLength > bufferSize) {
			outCount = 0;
			return false;
		}
		offset += labelLength;

		// Skip isDuplicate (1 byte) dan axisLock (4 bytes)
		if (offset + 5 > bufferSize) {
			outCount = 0;
			return false;
		}
		offset += 5;
	}

	// Baca jumlah Polygons
	if (offset + sizeof(int) > bufferSize) {
		outCount = 0;
		return false;
	}

	int numPolygons = *reinterpret_cast<int*>(data + offset);
	outCount = numPolygons;

	return true;
}
