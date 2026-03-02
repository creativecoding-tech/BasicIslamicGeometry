#pragma once

#include "ofMain.h"
#include <string>

class ofApp;

class FileOperationManager {
public:
  FileOperationManager(ofApp *app);

  // File operations
  void saveWorkspace();   // Save workspace to file
  void saveWorkspaceAs(); // Save workspace to custom location

  // Real save executions called after or during confirmations
  void executeSaveWorkspace(bool filterTessellation);
  void executeSaveWorkspaceAs(bool filterTessellation);

  // Helper to check for tessellated geometry
  bool hasTessellatedGeometry() const;
  void openWorkspace();    // Open file dialog dengan validasi .nay
  void loadWorkspace();    // Load workspace from file
  void loadWorkspaceSeq(); // Load workspace seq from file
  void closeFile();        // Close opened file (tanpa clean canvas)

  // Peek functions untuk CollapsingHeader
  bool peekFileCustomLinesCount(
      const std::string &filepath,
      int &outCount); // Cek jumlah customLines di file tanpa full load
  bool peekFilePolygonCount(
      const std::string &filepath,
      int &outCount); // Cek jumlah polygons di file tanpa full load

private:
  ofApp *app;
};
