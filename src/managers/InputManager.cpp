#include "InputManager.h"
#include "../ofApp.h"
#include "../operation/gui/SacredGeometry.h"
#include "../operation/gui/Playground.h"
#include "../operation/gui/UserCustom.h"
#include "../operation/gui/ContextMenu.h"
#include "../shape/CustomLine.h"
#include "../utils/GeometryUtils.h"
#include "../undo/UndoAction.h"

//--------------------------------------------------------------
InputManager::InputManager(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
InputManager::~InputManager() {
}

// ========================================================================
// MOUSE EVENT HANDLERS
// ========================================================================

//--------------------------------------------------------------
void InputManager::handleMousePressed(int x, int y, int button) {
	// Forward ke ImGui DULU (sebelum logic lain!)
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, true);

	// Cek apakah ImGui mau capture mouse (termasuk drag window)
	if (io.WantCaptureMouse) {
		return; // ImGui handle, jangan process di OF
	}

	// Klik kanan untuk context menu (DOT atau POLYGON)
	if (button == 2) {
		vec2 adjustedMousePos = GeometryUtils::screenToWorld(vec2(x, y), app->canvasTranslation, app->canvasZoom, app->canvasRotation);

		// CEK 1: Klik kanan pada DOT ORIGINAL (dari template)
		vector<DotInfo> allDots = app->getAllDots();
		vector<DotInfo> templateDots;
		for (const auto& dot : allDots) {
			if (dot.shapeType != "Dot") {  // "Dot" = userDot, exclude
				templateDots.push_back(dot);
			}
		}

		for (const auto& dot : templateDots) {
			if (GeometryUtils::isMouseOverDot(adjustedMousePos, dot.position, app->threshold)) {
				app->contextMenu->setHoveredDotPos(dot.position);
				app->contextMenu->setIsUserDotContext(false);  // Flag bahwa ini dari original dot
				app->contextMenu->showWindow(ContextMenu::DOT_CONTEXT, vec2(x, y));
				app->imguiVisible = true;
				return;
			}
		}

		// CEK 1.5: Klik kanan pada USERDOT (duplicate dot) yang terseleksi
		for (int i = 0; i < app->userDots.size(); i++) {
			if (app->userDots[i]) {
				vec2 dotPos = app->userDots[i]->getPosition();
				float dist = glm::length(adjustedMousePos - dotPos);
				if (dist < 15.0f) {
					// Hanya tampilkan context menu jika userDot terseleksi
					if (app->selectionManager.isUserDotSelected(i)) {
						app->contextMenu->setHoveredDotPos(dotPos);
						app->contextMenu->setIsUserDotContext(true);  // Flag bahwa ini dari userDot
						app->contextMenu->showWindow(ContextMenu::DOT_CONTEXT, vec2(x, y));
						app->imguiVisible = true;
						return;
					}
				}
			}
		}

		// CEK 2: Klik kanan pada POLYGON yang terseleksi
		for (int i = 0; i < app->polygonShapes.size(); i++) {
			if (app->polygonShapes[i].containsPoint(adjustedMousePos)) {
				// Hanya tampilkan context menu jika polygon terseleksi
				if (app->selectionManager.isPolygonSelected(i)) {
					app->contextMenu->setHoveredPolygonIndex(i);
					app->contextMenu->showWindow(ContextMenu::POLYGON_CONTEXT, vec2(x, y));
					app->imguiVisible = true;
				}
				return;
			}
		}

		// CEK 3: Klik kanan pada CUSTOMLINE yang terseleksi
		int clickedLineIndex = GeometryUtils::getLineIndexAtPosition(adjustedMousePos, app->customLines, app->mouseLineWidth);
		if (clickedLineIndex >= 0 && app->selectionManager.isLineSelected(clickedLineIndex)) {
			// Tampilkan context menu untuk semua customLine (original dan DcustomLine)
			app->contextMenu->setHoveredLineIndex(clickedLineIndex);
			app->contextMenu->showWindow(ContextMenu::CUSTOMLINE_CONTEXT, vec2(x, y));
			app->imguiVisible = true;
			return;
		}

		// Tidak klik di dot, polygon, atau customLine yang terseleksi → tidak munculkan context menu
		return;
	}

	// Klik kiri tutup context menu
	if (button == 0 && app->contextMenu->isVisible()) {
		app->contextMenu->hideWindow();
		return;
	}

	// CTRL+Click untuk multi-select (userDot PRIORITY lebih tinggi dari customLine)
	if (button == 0 && app->isCtrlPressed) {
		vec2 adjustedMousePos = GeometryUtils::screenToWorld(vec2(x, y), app->canvasTranslation, app->canvasZoom, app->canvasRotation);

		// CEK USER DOTS DULU (priority tertinggi)
		for (int i = 0; i < app->userDots.size(); i++) {
			if (app->userDots[i]) {
				vec2 dotPos = app->userDots[i]->getPosition();
				float dist = glm::length(adjustedMousePos - dotPos);
				if (dist < 15.0f) {
					// Toggle selection userDot
					app->selectionManager.toggleUserDotSelection(i);
					app->syncColorFromSelectedObjects();  // Sync global color dari selected object
					return; // Jangan lanjut ke logic lain
				}
			}
		}

		// Kalau tidak klik userDot, cek polygon
		for (int i = 0; i < app->polygonShapes.size(); i++) {
			if (app->polygonShapes[i].containsPoint(adjustedMousePos)) {
				// Toggle selection polygon
				if (app->selectionManager.isPolygonSelected(i)) {
					app->selectionManager.deselectPolygon(i); // Deselect
				} else {
					app->selectionManager.selectPolygon(i); // Select
				}
				app->syncColorFromSelectedObjects();  // Sync global color dari selected object
				return; // Jangan lanjut ke logic lain
			}
		}

		// Kalau tidak klik polygon, cek customLine
		int clickedLineIndex = GeometryUtils::getLineIndexAtPosition(adjustedMousePos, app->customLines, app->mouseLineWidth);
		if (clickedLineIndex >= 0) {
			// Toggle selection customLine
			app->selectionManager.toggleLineSelection(clickedLineIndex);
			app->syncColorFromSelectedObjects();  // Sync global color dari selected object
		}
		return; // Jangan lanjut ke logic drag
	}

	// Logic untuk interactive line creation & line selection
	if (button == 0) {
		// Adjust mouse position dengan inverse transform
		vec2 adjustedMousePos = GeometryUtils::screenToWorld(vec2(x, y), app->canvasTranslation, app->canvasZoom, app->canvasRotation);

		// CEK USER DOTS (normal click tanpa CTRL)
		bool clickedOnUserDot = false;
		for (int i = 0; i < app->userDots.size(); i++) {
			if (app->userDots[i]) {
				vec2 dotPos = app->userDots[i]->getPosition();
				// Gunakan threshold yang lebih besar untuk userDot (15px) agar mudah diklik
				float dist = glm::length(adjustedMousePos - dotPos);
				if (dist < 15.0f) {
					// Single select (hapus yang lama, select yang baru)
					app->selectionManager.clearUserDotSelection();
					app->selectionManager.selectUserDot(i);
					clickedOnUserDot = true;
					app->syncColorFromSelectedObjects();  // Sync global color dari selected userDot
					// JANGAN return, biarkan lanjut ke line creation
				}
			}
		}

		// Jika tidak klik userDot, deselect semua userDot
		if (!clickedOnUserDot) {
			app->selectionManager.clearUserDotSelection();
		}

		vector<DotInfo> dots = app->getAllDots();

		// Check jika klik di atas dot - HANYA bisa mulai dari dot
		bool clickedOnDot = false;
		for (auto &dot : dots) {
			if (GeometryUtils::isMouseOverDot(adjustedMousePos, dot.position, app->threshold)) {
				app->drawState = ofApp::DRAGGING;
				app->currentPolylinePoints.clear();
				app->currentPolylinePoints.push_back(dot.position); // Start dari dot
				app->startDotPos = dot.position;
				app->mousePos = adjustedMousePos;
				clickedOnDot = true;

				// Deselect line yang mungkin terpilih
				app->selectionManager.clearLineSelection();

				return;
			}
		}

		// Kalau tidak klik di dot, cek apakah klik di garis untuk SELECT
		if (!clickedOnDot) {
			// CEK POLYGON DULU (sebelum cek garis)
			bool clickedOnPolygon = false;
			for (int i = 0; i < app->polygonShapes.size(); i++) {
				if (app->polygonShapes[i].containsPoint(adjustedMousePos)) {
					// Select polygon ini (single select: hapus yang lama, select yang baru)
					app->selectionManager.clearPolygonSelection();
					app->selectionManager.selectPolygon(i);
					app->selectionManager.clearLineSelection(); // Deselect semua lines
					clickedOnPolygon = true;
					app->syncColorFromSelectedObjects();  // Sync global color dari selected object
					break;
				}
			}

			// Kalau tidak klik di polygon, cek garis
			if (!clickedOnPolygon) {
				int lineIndex = GeometryUtils::getLineIndexAtPosition(adjustedMousePos, app->customLines, app->mouseLineWidth);
				if (lineIndex >= 0) {
					// Select garis ini (single select: hapus yang lama, select yang baru)
					app->selectionManager.clearLineSelection();
					app->selectionManager.selectLine(lineIndex);
					app->selectionManager.clearPolygonSelection(); // Deselect polygon
					app->syncColorFromSelectedObjects();  // Sync global color dari selected object
				} else {
					// Klik di tempat kosong → deselect semua
					app->selectionManager.clearLineSelection();
					app->selectionManager.clearPolygonSelection(); // Deselect polygon
					// Tidak perlu syncColorFromSelectedObjects() karena tidak ada yang selected
				}
			}
		}
	}
}

//--------------------------------------------------------------
void InputManager::handleMouseReleased(int x, int y, int button) {
	// Forward ke ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, false);

	// Biarkan ImGui handle semua

	if (app->drawState != ofApp::DRAGGING) {
		return;
	}

	// Adjust mouse position dengan inverse transform
	vec2 releasePos = GeometryUtils::screenToWorld(vec2(x, y), app->canvasTranslation, app->canvasZoom, app->canvasRotation);
	vector<DotInfo> dots = app->getAllDots();

	// Check jika release di atas valid dot - HANYA bisa selesai di dot
	for (auto &dot : dots) {
		if (GeometryUtils::isMouseOverDot(releasePos, dot.position, app->threshold)) {
			// Cek apakah line sudah ada (duplicate check)
			if (!GeometryUtils::lineExists(app->startDotPos, dot.position, app->customLines)) {
				// Cek apakah start != end (bukan garis panjang 0)
				if (glm::length(app->startDotPos - dot.position) > 1.0f) {
					// Update end point ke posisi dot yang dilepas
					if (app->currentPolylinePoints.size() < 2) {
						app->currentPolylinePoints.push_back(dot.position);
					} else {
						app->currentPolylinePoints[1] = dot.position;
					}

					// Simpan polyline dengan 2 points (start dan end)
					std::string newLabel = "customLine" + std::to_string(app->customLines.size());
					CustomLine newLine(app->currentPolylinePoints, app->colorManager->getCustomLineColor(),
						app->mouseLineWidth, newLabel);
					app->customLines.push_back(newLine);

					// Push undo action
					UndoAction undoAction;
					undoAction.type = CREATE_LINE;
					undoAction.isCreate = true;
					app->pushUndoAction(undoAction);
				}
			}
			break;
		}
	}

	// Clear dan reset state
	app->currentPolylinePoints.clear();
	app->drawState = ofApp::IDLE;
}

//--------------------------------------------------------------
void InputManager::handleMouseDragged(int x, int y, int button) {
	// Forward ke ImGui DULU (untuk drag window)
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(x, y);

	// Cek apakah ImGui mau capture mouse
	if (io.WantCaptureMouse) {
		return; // ImGui handle, jangan process di OF
	}

	if (app->drawState == ofApp::DRAGGING) {
		// Adjust mouse position dengan inverse transform
		vec2 currentPos = GeometryUtils::screenToWorld(vec2(x, y), app->canvasTranslation, app->canvasZoom, app->canvasRotation);
		app->mousePos = currentPos;

		// Untuk garis lurus, kita hanya perlu 2 titik: start dan end
		// currentPolylinePoints[0] = start (diset di mousePressed)
		// currentPolylinePoints[1] = end (diupdate setiap drag)
		if (app->currentPolylinePoints.size() < 2) {
			app->currentPolylinePoints.push_back(currentPos); // Add end point
		} else {
			app->currentPolylinePoints[1] = currentPos; // Update end point
		}
	}
}

//--------------------------------------------------------------
void InputManager::handleMouseMoved(int x, int y) {
	// Forward ke ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(x, y);

	// Update mouse position dengan inverse transform (screen to world)
	app->mousePos = GeometryUtils::screenToWorld(vec2(x, y), app->canvasTranslation, app->canvasZoom, app->canvasRotation);
}

//--------------------------------------------------------------
void InputManager::handleMouseScrolled(int x, int y, float scrollX, float scrollY) {
	// Forward ke ImGui DULU
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(scrollX, scrollY);

	// Cek apakah ImGui mau capture mouse
	if (io.WantCaptureMouse) {
		return; // ImGui handle, jangan process di OF
	}

	// Handle scroll userDot (vertical untuk above/below, horizontal untuk left)
	if (app->selectionManager.hasSelectedUserDot()) {
		float scrollSpeed = 2.0f;  // Kecepatan scroll (lebih presise)

		// Scroll semua selected dots
		const std::set<int>& indices = app->selectionManager.getSelectedUserDotIndices();
		for (int index : indices) {
			if (index >= 0 && index < app->userDots.size()) {
				vec2 oldPos = app->userDots[index]->getPosition();  // Simpan posisi lama
				vec2 lowerBound = app->userDots[index]->getLowerBound();

				vec2 newPos = oldPos;

				// Cek apakah dot ini di sumbu Y (above/below) atau sumbu X (left)
				// dengan membandingkan posisi X dengan lowerBound X
				bool isHorizontalDot = (oldPos.x != lowerBound.x);

				if (isHorizontalDot) {
					// DOT LEFT/RIGHT: scroll di sumbu X
					// Scroll ke depan (scrollY positif) = X minus (ke kiri, makin X minus)
					// Scroll ke belakang (scrollY negatif) = X positif (ke kanan)
					float newX = oldPos.x - scrollY * scrollSpeed;

					// Tentukan batas berdasarkan posisi userDot relative terhadap dot original
					if (oldPos.x < lowerBound.x) {
						// DOT LEFT: batas kanan = dot original (tidak boleh ke kanan melewati dot original)
						float maxX = lowerBound.x - 1.0f;  // Jarak minimum 1 pixel dari dot original
						if (newX > maxX) {
							newX = maxX;  // Clamp ke batas kanan
						}
					} else if (oldPos.x > lowerBound.x) {
						// DOT RIGHT: batas kiri = dot original (tidak boleh ke kiri melewati dot original)
						float minX = lowerBound.x + 1.0f;  // Jarak minimum 1 pixel dari dot original
						if (newX < minX) {
							newX = minX;  // Clamp ke batas kiri
						}
					}

					newPos.x = newX;
				} else {
					// DOT ABOVE/BELOW: scroll di sumbu Y
					// Scroll maju (scrollY positif) = Y minus (ke atas, Y negatif)
					// Scroll mundur (scrollY negatif) = Y positif (ke bawah, Y positif)
					float newY = oldPos.y - scrollY * scrollSpeed;

					// Tentukan batas berdasarkan posisi userDot relative terhadap dot original
					if (oldPos.y < lowerBound.y) {
						// DOT ABOVE: batas bawah = dot original (tidak boleh ke bawah melewati dot original)
						float maxY = lowerBound.y - 1.0f;  // Jarak minimum 1 pixel dari dot original
						if (newY > maxY) {
							newY = maxY;  // Clamp ke batas bawah
						}
					} else if (oldPos.y > lowerBound.y) {
						// DOT BELOW: batas atas = dot original (tidak boleh ke atas melewati dot original)
						float minY = lowerBound.y + 1.0f;  // Jarak minimum 1 pixel dari dot original
						if (newY < minY) {
							newY = minY;  // Clamp ke batas atas
						}
					}

					newPos.y = newY;
				}

				// Update posisi dot
				app->userDots[index]->setPosition(newPos);

				// Update semua customLines yang terhubung ke userDot ini
				for (auto& line : app->customLines) {
					vector<vec2> points = line.getPoints();
					bool lineUpdated = false;

					// Cek setiap point di customLine
					for (size_t i = 0; i < points.size(); i++) {
						// Jika point sama dengan posisi lama (dengan toleransi kecil untuk floating point)
						if (glm::length(points[i] - oldPos) < 0.1f) {
							points[i] = newPos;  // Update ke posisi baru
							lineUpdated = true;
						}
					}

					// Jika ada yang diupdate, set kembali points ke customLine
					if (lineUpdated) {
						line.setPoints(points);
					}
				}
			}
		}

		return;  // Jangan lanjut ke logic lain jika ada userDot terseleksi
	}

	// Handle scroll untuk DcustomLine (duplicate lines) - HANYA jika CTRL ditekan
	if (app->isCtrlPressed && app->selectionManager.hasSelectedLine()) {
		float scrollSpeed = 2.0f;  // Kecepatan scroll

		// Copy indices ke local vector untuk menghindari iterator invalidation
		std::vector<int> selectedLineIndices(app->selectionManager.getSelectedLineIndices().begin(),
			app->selectionManager.getSelectedLineIndices().end());
		// Filter hanya DcustomLine yang terseleksi
		for (int lineIndex : selectedLineIndices) {
			if (lineIndex >= 0 && lineIndex < app->customLines.size()) {
				CustomLine& line = app->customLines[lineIndex];

				// Hanya proses jika ini duplicate line
				if (line.getIsDuplicate()) {
					AxisLock lockState = line.getAxisLock();

					// Skip jika LOCK_BOTH (fully locked)
					if (lockState == AxisLock::LOCK_BOTH) {
						continue;
					}

					// Ambil semua points
					vector<vec2> points = line.getPoints();

					// Geser semua points berdasarkan lock state
					for (vec2& point : points) {
						if (lockState == AxisLock::LOCK_X) {
							// LOCK X: Hanya bisa gerak di Y (scroll atas/bawah)
							// Scroll up (scrollY positif) = Y minus (ke atas)
							// Scroll down (scrollY negatif) = Y positif (ke bawah)
							point.y -= scrollY * scrollSpeed;
						}
						else if (lockState == AxisLock::LOCK_Y) {
							// LOCK Y: Hanya bisa gerak di X (scroll kanan/kiri)
							// Scroll up (scrollY positif) = X minus (ke kiri)
							// Scroll down (scrollY negatif) = X positif (ke kanan)
							point.x -= scrollY * scrollSpeed;
						}
						else if (lockState == AxisLock::NONE) {
							// NONE: Bisa gerak bebas di X dan Y (scroll gerakkan serong)
							// Scroll up (scrollY positif) = X minus, Y minus (kiri atas)
							// Scroll down (scrollY negatif) = X positif, Y positif (kanan bawah)
							point.x -= scrollY * scrollSpeed;
							point.y -= scrollY * scrollSpeed;
						}
						// LOCK_BOTH: Tidak bisa scroll (skip)
					}

					// Update points ke line
					line.setPoints(points);
				}
			}
		}
	}

	// Update curve untuk SEMUA garis yang selected - HANYA jika TANPA CTRL
	if (!app->isCtrlPressed && app->selectionManager.hasSelectedLine()) {
		// Siapkan undo action
		UndoAction undoAction;
		undoAction.type = CHANGE_CURVE;
		undoAction.newCurve = 0.0f;  // Placeholder, tidak dipakai

		float curveSpeed = 2.0f; // Kecepatan perubahan curve
		// Copy indices ke local vector untuk menghindari iterator invalidation
		std::vector<int> selectedLineIndices(app->selectionManager.getSelectedLineIndices().begin(),
			app->selectionManager.getSelectedLineIndices().end());
		for (int lineIndex : selectedLineIndices) {
			if (lineIndex >= 0 && lineIndex < app->customLines.size()) {
				// Simpan old curve SEBELUM mengubah
				undoAction.curveLineIndices.push_back(lineIndex);
				undoAction.oldCurves.push_back(app->customLines[lineIndex].getCurve());

				// Hitung new curve dan update
				float newCurve = app->customLines[lineIndex].getCurve() + scrollY * curveSpeed;
				undoAction.newCurve = newCurve;  // Semua lines dapat curve baru yang sama
				app->customLines[lineIndex].setCurve(newCurve);
			}
		}
		// Push undo action hanya jika ada yang diubah
		if (!undoAction.curveLineIndices.empty()) {
			app->pushUndoAction(undoAction);
		}
	}
}

// ========================================================================
// KEYBOARD EVENT HANDLERS
// ========================================================================

//--------------------------------------------------------------
void InputManager::handleKeyPressed(int key) {
	if (key == OF_KEY_END)
		ofExit();

	// Toggle label visibility dengan ` atau ~
	if (key == '`' || key == '~') {
		// Toggle label visibility - delegate ke template
		if (app->currentTemplate) {
			app->currentTemplate->labelsVisible = !app->currentTemplate->labelsVisible;
			app->currentTemplate->toggleLabels();
		}
	}

	// Toggle dot visibility dengan . atau >
	if (key == '.' || key == '>') {
		// Toggle dot visibility - delegate ke template
		if (app->currentTemplate) {
			app->currentTemplate->dotsVisible = !app->currentTemplate->dotsVisible;
			app->currentTemplate->toggleDots();
		}
	}

	if (key == OF_KEY_DEL) {
		bool isShiftPressed = ofGetKeyPressed(OF_KEY_SHIFT);

		if (app->isCtrlPressed && isShiftPressed) {
			// CTRL+SHIFT+DEL: Clean canvas (hapus semua polygon, custom lines, dan hide template shapes)
			app->cleanCanvas();
			return; // Jangan lanjut ke logic DEL biasa
		} else if (app->isCtrlPressed) {
			// CTRL+DEL: Hapus semua polygon dan semua custom lines
			app->clearCustomLinesAndPolygons();
			return; // Jangan lanjut ke logic hide shapes
		} else {
			// DEL saja: Tidak melakukan apa-apa (hide sudah tidak dipakai lagi)
			// User harus menggunakan Clean Canvas atau Draw Template di SacredGeometry panel
		}
	}

	if (key == OF_KEY_BACKSPACE) {
		// Jangan hapus jika sedang sequential load
		if (app->fileManager.isLoadSequentialMode()) {
			return; // Aborted, sedang loading
		}

		// Jangan hapus jika sedang parallel load dan masih ada polygon animasi
		if (app->fileManager.isLoadParallelMode()) {
			// Cek apakah masih ada polygon yang sedang animasi
			for (const auto &polygon : app->polygonShapes) {
				if (polygon.hasAnimation()) {
					return; // Masih ada polygon yang animasi, tunggu kelar
				}
			}
		}

		// Prioritas 1: Hapus userDot selected dulu (highest priority)
		if (app->selectionManager.hasSelectedUserDot()) {
			app->deleteSelectedUserDot();
			return; // Return agar tidak trigger delete lainnya
		}

		// Prioritas 2: Hapus polygon selected
		if (app->selectionManager.hasSelectedPolygon()) {
			app->deleteSelectedPolygons();
			return; // Return agar tidak trigger delete lainnya
		}

		// Prioritas 3: Hapus customLine selected
		if (app->selectionManager.hasSelectedLine()) {
			// Hapus SEMUA garis yang terselect (support multi-delete)
			// Sort descending agar aman untuk erase
			const std::set<int>& indices = app->selectionManager.getSelectedLineIndices();
			vector<int> toDelete(indices.begin(), indices.end());
			std::sort(toDelete.rbegin(), toDelete.rend()); // Descending

			// Push undo action untuk setiap line yang dihapus (reverse order)
			for (int index : toDelete) {
				UndoAction undoAction;
				undoAction.type = DELETE_LINE;
				undoAction.deletedLine = app->customLines[index];
				undoAction.deletedLineIndex = index;
				app->pushUndoAction(undoAction);
			}

			// Hapus lines
			for (int index : toDelete) {
				app->customLines.erase(app->customLines.begin() + index);
			}
			app->selectionManager.clearLineSelection();
		}
		// Note: CartesianAxes toggle dihapus karena tidak sesuai dengan Draw Only concept
	}

	// SHIFT+) untuk show semua shapes sudah tidak dipakai lagi
	// User harus menggunakan Draw Template di SacredGeometry panel

	if (key == '-' || key == '_') {
		// Kurangi line width secara bertahap
		if (app->currentTemplate) {
			app->currentTemplate->lineWidth -= 0.5f;

			// Batasi minimum line width
			if (app->currentTemplate->lineWidth < 0.f) {
				app->currentTemplate->lineWidth = 0.f;
			}
			app->updateLineWidth();
		}
	}

	if (key == '+' || key == '=') {
		// Tambah line width secara bertahap
		if (app->currentTemplate) {
			app->currentTemplate->lineWidth += 0.5f;

			// Batasi maximum line width
			if (app->currentTemplate->lineWidth > 4.0f) {
				app->currentTemplate->lineWidth = 4.0f;
			}
			app->updateLineWidth();
		}
	}

	// Handle CTRL key ditekan
	if (key == 3682 || key == 3683) { // HANYA 3682/3683
		app->isCtrlPressed = true;
		return; // Jangan lanjut ke logic lain
	}

	// SHIFT+B = Select SEMUA customLines (bisa 'B' atau 'b' karena CAPSLOCK)
	if ((key == 'B' || key == 'b') && ofGetKeyPressed(OF_KEY_SHIFT)) {
		for (int i = 0; i < app->customLines.size(); i++) {
			app->selectionManager.selectLine(i);
		}
		app->syncColorFromSelectedObjects();  // Sync global color dari first selected line
		return;
	}

	// Handle tombol saat CTRL aktif - gunakan switch-case
	if (app->isCtrlPressed) {
		switch (key) {
		case 'z':
		case 'Z':
		case 26: // CTRL+Z (ASCII 26)
			// Cek apakah SHIFT juga ditekan (CTRL+SHIFT+Z untuk Redo)
			if (ofGetKeyPressed(OF_KEY_SHIFT)) {
				app->redo();
			} else {
				app->undo();
			}
			break;

		case 's':
		case 'S':
		case 19: // CTRL+S (ASCII 19)
			// Cek apakah SHIFT juga ditekan
			if (ofGetKeyPressed(OF_KEY_SHIFT)) {
				app->saveWorkspaceAs();
			} else {
				app->saveWorkspace();
			}
			break;

		case 'o':
		case 'O':
		case 15: // CTRL+O (ASCII 15)
			app->openWorkspace();
			break;

		case 'g':
		case 'G':
		case 7: // CTRL+G (ASCII 7)
			app->createInvisiblePolygonFromSelected();
			break;
		}
	}

	// G/g - Toggle ImGui visibility (HANYA tanpa CTRL)
	if ((key == 'g' || key == 'G') && !app->isCtrlPressed) {
		if (!app->imguiVisible) {
			// Show MenuBar only
			app->imguiVisible = true;
			app->showSacredGeometry = false;
			app->showPlayground = false;
			app->showUserCustom = false;
		} else if (!app->showSacredGeometry && !app->showPlayground && !app->showUserCustom) {
			// Show all windows
			app->imguiVisible = true;
			app->showSacredGeometry = true;
			app->showPlayground = true;
			app->showUserCustom = true;

			// Reset windowOpen flags untuk masing-masing window
			for (auto& gui : app->guiComponents) {
				// Cast ke tipe spesifik untuk panggil showWindow()
				if (auto* sacredGeom = dynamic_cast<SacredGeometry*>(gui.get())) {
					sacredGeom->showWindow();
				} else if (auto* playground = dynamic_cast<Playground*>(gui.get())) {
					playground->showWindow();
				} else if (auto* userCustom = dynamic_cast<UserCustom*>(gui.get())) {
					userCustom->showWindow();
				}
			}
		} else {
			// Hide all windows
			app->imguiVisible = false;
			app->showSacredGeometry = false;
			app->showPlayground = false;
			app->showUserCustom = false;
		}
	}
}

//--------------------------------------------------------------
void InputManager::handleKeyReleased(int key) {
	if (key == OF_KEY_CONTROL)
		app->isCtrlPressed = false;
}
