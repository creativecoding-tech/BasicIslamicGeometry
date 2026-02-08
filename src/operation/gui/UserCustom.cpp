#include "UserCustom.h"
#include "../../ofApp.h"

UserCustom::UserCustom(ofApp* app) : app(app) {
    // Initialize color picker dari app->customLineColor (default biru)
    customLineColor[0] = app->customLineColor.r / 255.0f; // R
    customLineColor[1] = app->customLineColor.g / 255.0f; // G
    customLineColor[2] = app->customLineColor.b / 255.0f; // B
    customLineColor[3] = app->customLineColor.a / 255.0f; // A

    // Initialize color picker dari app->polygonColor (default biru)
    polygonColor[0] = app->polygonColor.r / 255.0f; // R
    polygonColor[1] = app->polygonColor.g / 255.0f; // G
    polygonColor[2] = app->polygonColor.b / 255.0f; // B
    polygonColor[3] = app->polygonColor.a / 255.0f; // A

    // Initialize color picker dari app->userDotColor (default biru)
    userDotColor[0] = app->userDotColor.r / 255.0f; // R
    userDotColor[1] = app->userDotColor.g / 255.0f; // G
    userDotColor[2] = app->userDotColor.b / 255.0f; // B
    userDotColor[3] = app->userDotColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void UserCustom::updateColorFromApp() {
    // Update color picker values dari app->customLineColor
    customLineColor[0] = app->customLineColor.r / 255.0f; // R
    customLineColor[1] = app->customLineColor.g / 255.0f; // G
    customLineColor[2] = app->customLineColor.b / 255.0f; // B
    customLineColor[3] = app->customLineColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void UserCustom::updatePolygonColorFromApp() {
    // Update polygon color picker values dari app->polygonColor
    polygonColor[0] = app->polygonColor.r / 255.0f; // R
    polygonColor[1] = app->polygonColor.g / 255.0f; // G
    polygonColor[2] = app->polygonColor.b / 255.0f; // B
    polygonColor[3] = app->polygonColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void UserCustom::updateUserDotColorFromApp() {
    // Update userDot color picker values dari app->userDotColor
    userDotColor[0] = app->userDotColor.r / 255.0f; // R
    userDotColor[1] = app->userDotColor.g / 255.0f; // G
    userDotColor[2] = app->userDotColor.b / 255.0f; // B
    userDotColor[3] = app->userDotColor.a / 255.0f; // A
}

//--------------------------------------------------------------
void UserCustom::syncUserDotFromSelection() {
    // Jika ada userDot yang terseleksi, sync radius dan color dengan dot pertama yang terseleksi
    if (app->selectionManager.hasSelectedUserDot()) {
        int firstIndex = app->selectionManager.getLastSelectedUserDotIndex();
        if (firstIndex >= 0 && firstIndex < app->userDots.size()) {
            auto& dot = app->userDots[firstIndex];
            if (dot) {
                // Sync radius
                app->userDotRadius = dot->getRadius();

                // Sync color
                ofColor dotColor = dot->getColor();
                userDotColor[0] = dotColor.r / 255.0f;
                userDotColor[1] = dotColor.g / 255.0f;
                userDotColor[2] = dotColor.b / 255.0f;
                userDotColor[3] = dotColor.a / 255.0f;
            }
        }
    }
    else {
        // Jika tidak ada yang terseleksi, sync dari global color variable
        userDotColor[0] = app->userDotColor.r / 255.0f;
        userDotColor[1] = app->userDotColor.g / 255.0f;
        userDotColor[2] = app->userDotColor.b / 255.0f;
        userDotColor[3] = app->userDotColor.a / 255.0f;
    }
}

//--------------------------------------------------------------
void UserCustom::syncLineColorFromSelection() {
    // Jika ada customLine yang terseleksi, sync color dengan line pertama yang terseleksi
    if (!app->selectedLineIndices.empty()) {
        int firstIndex = *app->selectedLineIndices.begin();
        if (firstIndex >= 0 && firstIndex < app->customLines.size()) {
            ofColor lineColor = app->customLines[firstIndex].getColor();
            customLineColor[0] = lineColor.r / 255.0f;
            customLineColor[1] = lineColor.g / 255.0f;
            customLineColor[2] = lineColor.b / 255.0f;
            customLineColor[3] = lineColor.a / 255.0f;
        }
    }
    else {
        // Jika tidak ada yang terseleksi, sync dari global color variable
        customLineColor[0] = app->customLineColor.r / 255.0f;
        customLineColor[1] = app->customLineColor.g / 255.0f;
        customLineColor[2] = app->customLineColor.b / 255.0f;
        customLineColor[3] = app->customLineColor.a / 255.0f;
    }
}

//--------------------------------------------------------------
void UserCustom::syncPolygonColorFromSelection() {
    // Jika ada polygon yang terseleksi, sync color dengan polygon pertama yang terseleksi
    if (app->selectionManager.hasSelectedPolygon()) {
        int firstIndex = app->selectionManager.getLastSelectedPolygonIndex();
        if (firstIndex >= 0 && firstIndex < app->polygonShapes.size()) {
            ofColor polyColor = app->polygonShapes[firstIndex].getColor();
            polygonColor[0] = polyColor.r / 255.0f;
            polygonColor[1] = polyColor.g / 255.0f;
            polygonColor[2] = polyColor.b / 255.0f;
            polygonColor[3] = polyColor.a / 255.0f;
        }
    }
    else {
        // Jika tidak ada yang terseleksi, sync dari global color variable
        polygonColor[0] = app->polygonColor.r / 255.0f;
        polygonColor[1] = app->polygonColor.g / 255.0f;
        polygonColor[2] = app->polygonColor.b / 255.0f;
        polygonColor[3] = app->polygonColor.a / 255.0f;
    }
}

//--------------------------------------------------------------
void UserCustom::focusWindow() {
    focusRequested = true;
}

//--------------------------------------------------------------
void UserCustom::showWindow() {
    windowOpen = true;
}

//--------------------------------------------------------------
void UserCustom::draw() {
    ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 450), ImGuiCond_FirstUseEver);

    // Focus window jika di-request
    if (focusRequested) {
        ImGui::SetNextWindowFocus();
        focusRequested = false;
    }

    // Begin window dengan close button (windowOpen flag)
    if (ImGui::Begin("User Custom", &windowOpen, ImGuiWindowFlags_None)) {
        ImGui::Text("Dot");

        // Sync radius dan color dengan selected userDot (setiap frame)
        syncUserDotFromSelection();

        if (ImGui::Checkbox("Show/Hide", &app->showUserDot)) {
            // Event handler: toggle userDot visibility
            if (!app->showUserDot) {
                // Unselect semua userDot ketika di-hide
                app->selectionManager.clearUserDotSelection();
            }
        }
        ImGui::SetNextItemWidth(100.0f);
        ImGui::SameLine();
        if (ImGui::SliderFloat("Radius", &app->userDotRadius, 0.0f, 8.0f)) {
            // Event handler: update selected userDots radius saja
            app->updateUserDotRadius(app->userDotRadius);
        }
        if (ImGui::ColorEdit4("D Color", userDotColor)) {
            // Event handler: warna berubah, update selected userDots saja
            ofColor newColor(
                userDotColor[0] * 255,
                userDotColor[1] * 255,
                userDotColor[2] * 255,
                userDotColor[3] * 255
            );
            app->updateUserDotColor(newColor);
        }
        ImGui::Separator();

        ImGui::Text("Line");

        // Info customLine yang terseleksi
        if (!app->selectedLineIndices.empty()) {
            int selectedCount = app->selectedLineIndices.size();

            // Tampilkan label customLine yang terseleksi (dengan auto wrap)
            std::string labels = "Selected: ";
            for (auto it = app->selectedLineIndices.begin(); it != app->selectedLineIndices.end(); ++it) {
                // Ambil label dari CustomLine (bukan hardcoded "customLine" + index)
                if (*it >= 0 && *it < app->customLines.size()) {
                    labels += app->customLines[*it].getLabel();
                } else {
                    labels += "customLine" + std::to_string(*it);  // Fallback kalau index invalid
                }
                // Tambah koma kecuali ini elemen terakhir
                if (std::next(it) != app->selectedLineIndices.end()) {
                    labels += ", ";
                }
            }
            // Gunakan TextWrapped untuk auto wrap ke bawah (hanya untuk label)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
            ImGui::TextWrapped("%s", labels.c_str());
            ImGui::PopStyleColor();

            // Tampilkan total count di baris terpisah
            ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 1.0f), "%d lines selected", selectedCount);
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No line selected");
        }

        // Sync color dengan selected customLine (setiap frame)
        syncLineColorFromSelection();
        // Color picker untuk custom line (bentuk melingkar)
        if (ImGui::ColorPicker4("L Color", customLineColor,
            ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs)) {
            // Event handler: warna berubah, update semua customLines
            ofColor newColor(
                customLineColor[0] * 255,
                customLineColor[1] * 255,
                customLineColor[2] * 255,
                customLineColor[3] * 255
            );
            app->updateCustomLineColor(newColor);
        }

        // Button: Reset All Line Colors & Reset Selected Line Colors (sama lebar)
        float buttonWidth = ImGui::CalcTextSize("Reset Selected Line Colors").x + ImGui::GetStyle().FramePadding.x * 2.0f;

        if (ImGui::Button("Reset All Line Colors", ImVec2(buttonWidth, 0))) {
            app->resetAllCustomLineColor();
            updateColorFromApp();  // Sync colorpicker dengan default color
        }

        // Button: Reset Selected Line Colors (hanya enabled jika ada selection)
        if (!app->selectedLineIndices.empty()) {
            if (ImGui::Button("Reset Selected Line Colors", ImVec2(buttonWidth, 0))) {
                app->resetSelectedCustomLineColor();
            }
        } else {
            // Disabled button kalau tidak ada selection
            ImGui::BeginDisabled();
            ImGui::Button("Reset Selected Line Colors", ImVec2(buttonWidth, 0));
            ImGui::EndDisabled();
        }

        // Button: Duplicate Line R 180° (hanya enabled jika ada >= 2 selected lines)
        if (app->selectedLineIndices.size() >= 2) {
            if (ImGui::Button("Duplicate Line R 180°", ImVec2(buttonWidth, 0))) {
                app->duplicateLineR180();
            }
        } else {
            // Disabled button kalau selected lines < 2
            ImGui::BeginDisabled();
            ImGui::Button("Duplicate Line R 180°", ImVec2(buttonWidth, 0));
            ImGui::EndDisabled();
        }
        ImGui::Separator();
        ImGui::Text("Polygon");

        // Info polygon yang terseleksi
        if (app->selectionManager.hasSelectedPolygon()) {
            int selectedCount = app->selectionManager.getSelectedPolygonCount();

            // Tampilkan label polygon yang terseleksi (dengan auto wrap)
            std::string labels = "Selected: ";
            const std::set<int>& indices = app->selectionManager.getSelectedPolygonIndices();
            for (auto it = indices.begin(); it != indices.end(); ++it) {
                // Ambil label dari PolygonShape (index + "polygon" prefix)
                if (*it >= 0 && *it < app->polygonShapes.size()) {
                    labels += "polygon" + std::to_string(app->polygonShapes[*it].getIndex());
                } else {
                    labels += "polygon" + std::to_string(*it);  // Fallback kalau index invalid
                }
                // Tambah koma kecuali ini elemen terakhir
                if (std::next(it) != indices.end()) {
                    labels += ", ";
                }
            }
            // Gunakan TextWrapped untuk auto wrap ke bawah (hanya untuk label)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
            ImGui::TextWrapped("%s", labels.c_str());
            ImGui::PopStyleColor();

            // Tampilkan total count di baris terpisah
            ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 1.0f), "%d polygons selected", selectedCount);
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No polygon selected");
        }

        // Sync color dengan selected polygon (setiap frame)
        syncPolygonColorFromSelection();
        // Color picker untuk polygon (bentuk melingkar)
        if (ImGui::ColorPicker4("P Color", polygonColor,
            ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs)) {
            // Event handler: warna berubah, update semua polygons
            ofColor newColor(
                polygonColor[0] * 255,
                polygonColor[1] * 255,
                polygonColor[2] * 255,
                polygonColor[3] * 255
            );
            app->updatePolygonColor(newColor);
        }

        // Button: Reset All Polygon Colors & Reset Selected Polygon Colors (sama lebar)
        float polygonButtonWidth = ImGui::CalcTextSize("Reset Selected Polygon Colors").x + ImGui::GetStyle().FramePadding.x * 2.0f;

        if (ImGui::Button("Reset All Polygon Colors", ImVec2(polygonButtonWidth, 0))) {
            app->resetAllPolygonColor();
            updatePolygonColorFromApp();  // Sync colorpicker dengan default color
        }

        // Button: Reset Selected Polygon Colors (hanya enabled jika ada selection)
        if (app->selectionManager.hasSelectedPolygon()) {
            if (ImGui::Button("Reset Selected Polygon Colors", ImVec2(polygonButtonWidth, 0))) {
                app->resetSelectedPolygonColor();
            }
        } else {
            // Disabled button kalau tidak ada selection
            ImGui::BeginDisabled();
            ImGui::Button("Reset Selected Polygon Colors", ImVec2(polygonButtonWidth, 0));
            ImGui::EndDisabled();
        }

        // Slider transparansi (alpha) untuk polygon - hanya untuk selected polygon
        if (app->selectionManager.hasSelectedPolygon()) {
            // Ambil alpha dari polygon pertama yang terseleksi untuk inisialisasi slider
            int firstIndex = app->selectionManager.getLastSelectedPolygonIndex();
            if (firstIndex >= 0 && firstIndex < app->polygonShapes.size()) {
                static int polygonAlpha = 255;  // Nilai slider 0-255 (disimpan antar frame)

                // Ambil alpha dari polygon pertama untuk sinkronisasi slider hanya saat slider tidak aktif
                ofColor firstPolyColor = app->polygonShapes[firstIndex].getColor();
                if (!ImGui::IsItemActive()) {
                    polygonAlpha = firstPolyColor.a;  // Sinkron saat tidak drag slider
                }

                ImGui::SetNextItemWidth(150.0f);
                if (ImGui::SliderInt("Alpha", &polygonAlpha, 0, 255)) {
                    // Update alpha semua selected polygon, RGB tetap
                    app->updatePolygonAlpha((uint8_t)polygonAlpha);
                }
            }
        }
    }
    ImGui::End();

    // Sync window open state ke app
    app->showUserCustom = windowOpen;
}
