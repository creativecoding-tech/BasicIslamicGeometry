#include "MenuBar.h"
#include "../../ofApp.h"

MenuBar::MenuBar(ofApp* app) : app(app) {}

void MenuBar::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open","CTRL+O ")) {
                app->openWorkspace();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Workspace", "CTRL+S")) {
                app->saveWorkspace();
            }
            if (ImGui::MenuItem("Save As...", "CTRL+SHIFT+S")) {
                app->saveWorkspaceAs();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "END")) {
                ofExit();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {
                app->undo();
            }
            if (ImGui::MenuItem("Redo", "CTRL+SHIFT+Z")) {
                app->redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete All Custom Lines")) {
                app->deleteAllCustomLines();
            }
            if (ImGui::MenuItem("Delete All Polygons")) {
                app->deleteAllPolygons();
            }
            if (ImGui::MenuItem("Delete All Custom Dots")) {
                app->deleteAllUserDots();
            }
            if (ImGui::MenuItem("Delete Lines & Polygons", "CTRL+DEL")) {
                app->clearCustomLinesAndPolygons();
            }
            // Clean Canvas - Disable jika canvas kosong
            bool isCanvasEmpty = (app->currentTemplate && app->currentTemplate->getShapes().empty()) &&
                                app->customLines.empty() &&
                                app->polygonShapes.empty();

            if (ImGui::MenuItem("Clean Canvas", "CTRL+SHIFT+DEL", false, !isCanvasEmpty)) {
                // cleanCanvas() sudah otomatis show confirmation popup
                app->cleanCanvas();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Sacred Gemotery")) {
                app->toggleSacredGeometryWindow();
            }
            if (ImGui::MenuItem("User Custom")) {
                app->toggleUserCustomWindow();
            }
            if (ImGui::MenuItem("Playground")) {
                app->togglePlaygroundWindow();
            }
            if (ImGui::MenuItem("Selection Info")) {
                app->toggleSelectionInfoWindow();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {

            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
