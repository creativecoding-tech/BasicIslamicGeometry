#include "FileManager.h"
#include "../anim/FadeInAnimation.h"

// Konstanta filename
const std::string FileManager::FILENAME = "workspace.nay";

//--------------------------------------------------------------
//--------------------------------------------------------------
void FileManager::saveAll(const std::string &templateName, float globalRadius,
                          const std::vector<CustomLine> &customLines,
                          const std::vector<PolygonShape> &polygons,
                          float currentLineWidth, bool labelsVisible,
                          bool dotsVisible) {
  ofBuffer buffer;

  // ===== HEADER (64 bytes) =====
  // Magic number (4 bytes)
  buffer.append(MAGIC_NUMBER, 4);

  // Version (4 bytes int)
  int version = VERSION;
  buffer.append(reinterpret_cast<const char *>(&version), sizeof(int));

  // Template name length (4 bytes int)
  int nameLen = static_cast<int>(templateName.length());
  buffer.append(reinterpret_cast<const char *>(&nameLen), sizeof(int));

  // Template name (string)
  buffer.append(templateName.c_str(), nameLen);

  // Global radius (4 bytes float)
  buffer.append(reinterpret_cast<const char *>(&globalRadius), sizeof(float));

  // REUSED RESERVED (20 bytes total)
  // Byte 0-3: currentLineWidth (float)
  buffer.append(reinterpret_cast<const char *>(&currentLineWidth),
                sizeof(float));

  // Byte 4: labelsVisible (bool/1 byte)
  buffer.append(reinterpret_cast<const char *>(&labelsVisible), sizeof(bool));

  // Byte 5: dotsVisible (bool/1 byte)
  buffer.append(reinterpret_cast<const char *>(&dotsVisible), sizeof(bool));

  // Byte 6-19: Remaining Reserved (14 bytes)
  char reserved[14] = {0};
  buffer.append(reserved, 14);

  // ===== CUSTOM LINES =====
  int numLines = static_cast<int>(customLines.size());
  buffer.append(reinterpret_cast<const char *>(&numLines), sizeof(int));

  // Simpan customLines dengan NORMALIZED positions (dibagi radius)
  saveCustomLinesNA(buffer, customLines, globalRadius);

  // ===== POLYGONS =====
  int numPolygons = static_cast<int>(polygons.size());
  buffer.append(reinterpret_cast<const char *>(&numPolygons), sizeof(int));

  // Simpan polygons dengan NORMALIZED positions (dibagi radius)
  savePolygonsNA(buffer, polygons, globalRadius);

  // Write ke file
  ofBufferToFile(FILENAME, buffer);
}

//--------------------------------------------------------------
bool FileManager::loadAll(std::string &outTemplateName, float &outGlobalRadius,
                          std::vector<CustomLine> &customLines,
                          std::vector<PolygonShape> &polygons,
                          float &outLineWidth, bool &outLabelsVisible,
                          bool &outDotsVisible) {
  // Cek apakah ada isinya dulu
  if (!customLines.empty()) {
    return false; // Tidak load jika sudah ada isinya
  }

  // Cek apakah file exists
  ofFile file(FILENAME);
  if (!file.exists()) {
    return false;
  }

  // Read file ke buffer
  ofBuffer buffer = ofBufferFromFile(FILENAME);
  char *data = buffer.getData();
  size_t bufferSize = buffer.size();

  // Validasi ukuran minimal
  if (bufferSize < 64) {
    ofLog() << "Error: File terlalu kecil";
    return false;
  }

  size_t offset = 0;

  // ===== VALIDASI HEADER =====
  // Magic number
  if (memcmp(data + offset, MAGIC_NUMBER, 4) != 0) {
    ofLog() << "Error: Invalid file format (wrong magic number)";
    return false;
  }
  offset += 4;

  // Version
  int version = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);
  // Support version 1 and 2
  if (version != 1 && version != 2) {
    ofLog() << "Error: Unsupported version: " << version;
    return false;
  }

  // Template name length
  int nameLen = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Template name
  if (nameLen <= 0 || nameLen > 256) {
    ofLog() << "Error: Invalid template name length";
    return false;
  }
  outTemplateName = std::string(data + offset, nameLen);
  offset += nameLen;

  // Global radius
  outGlobalRadius = *reinterpret_cast<float *>(data + offset);
  offset += sizeof(float);

  // Parse Additional Settings based on version
  if (version == 1) {
    // Version 1: Set default values and skip reserved 20 bytes
    outLineWidth = 4.0f;
    outLabelsVisible = true;
    outDotsVisible = true;
    offset += 20;
  } else if (version == 2) {
    // Version 2: Read from what was previously reserved

    // currentLineWidth (float)
    outLineWidth = *reinterpret_cast<float *>(data + offset);
    offset += sizeof(float);

    // labelsVisible (bool)
    outLabelsVisible = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    // dotsVisible (bool)
    outDotsVisible = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    // Skip remaining reserved (14 bytes)
    offset += 14;
  }

  // ===== LOAD CUSTOM LINES =====
  if (offset >= bufferSize) {
    ofLog() << "Error: Unexpected end of file after header";
    return false;
  }

  int numLines = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  customLines.clear();
  if (!loadCustomLinesNA(buffer, offset, customLines, outGlobalRadius)) {
    ofLog() << "Error: Failed to load custom lines";
    return false;
  }

  // ===== LOAD POLYGONS =====
  if (offset >= bufferSize) {
    ofLog() << "Error: Unexpected end of file after custom lines";
    return false;
  }

  int numPolygons = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  polygons.clear();
  if (!loadPolygonsNA(buffer, offset, polygons, outGlobalRadius)) {
    ofLog() << "Error: Failed to load polygons";
    return false;
  }

  return true;
}

//--------------------------------------------------------------
void FileManager::saveCustomLinesNA(ofBuffer &buffer,
                                    const std::vector<CustomLine> &customLines,
                                    float radius) {
  // Write jumlah line
  int numLines = static_cast<int>(customLines.size());
  buffer.append(reinterpret_cast<const char *>(&numLines), sizeof(int));

  // Write setiap line dengan NORMALIZED positions
  for (const auto &line : customLines) {
    // Write jumlah points
    const vector<vec2> &points = line.getPoints();
    int numPoints = static_cast<int>(points.size());
    buffer.append(reinterpret_cast<const char *>(&numPoints), sizeof(int));

    // Write semua points dengan POSISI NORMALIZED (dibagi radius)
    for (const auto &point : points) {
      vec2 normalizedPoint = point / radius; // Normalize
      buffer.append(reinterpret_cast<const char *>(&normalizedPoint),
                    sizeof(vec2));
    }

    // Write color, lineWidth, dan curve (tidak perlu normalize)
    ofColor color = line.getColor();
    float lineWidth = line.getLineWidth();
    float curve = line.getCurve();
    buffer.append(reinterpret_cast<const char *>(&color), sizeof(ofColor));
    buffer.append(reinterpret_cast<const char *>(&lineWidth), sizeof(float));
    buffer.append(reinterpret_cast<const char *>(&curve), sizeof(float));
  }
}

//--------------------------------------------------------------
bool FileManager::loadCustomLinesNA(ofBuffer &buffer, size_t &offset,
                                    std::vector<CustomLine> &customLines,
                                    float radius) {
  char *data = buffer.getData();
  size_t bufferSize = buffer.size();

  // Read jumlah line
  if (offset + sizeof(int) > bufferSize) {
    return false;
  }
  int numLines = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Read setiap line
  for (int i = 0; i < numLines; i++) {
    CustomLine line;

    // Read jumlah points
    if (offset + sizeof(int) > bufferSize) {
      return false;
    }
    int numPoints = *reinterpret_cast<int *>(data + offset);
    offset += sizeof(int);

    // Read semua points dan DENORMALIZE (kali radius)
    vector<vec2> points;
    for (int j = 0; j < numPoints; j++) {
      if (offset + sizeof(vec2) > bufferSize) {
        return false;
      }
      vec2 normalizedPoint = *reinterpret_cast<vec2 *>(data + offset);
      offset += sizeof(vec2);
      vec2 point = normalizedPoint * radius; // Denormalize
      points.push_back(point);
    }
    line.setPoints(points);

    // Read color
    if (offset + sizeof(ofColor) > bufferSize) {
      return false;
    }
    ofColor color = *reinterpret_cast<ofColor *>(data + offset);
    offset += sizeof(ofColor);
    line.setColor(color);

    // Read lineWidth
    if (offset + sizeof(float) > bufferSize) {
      return false;
    }
    float lineWidth = *reinterpret_cast<float *>(data + offset);
    offset += sizeof(float);
    line.setLineWidth(lineWidth);

    // Read curve
    if (offset + sizeof(float) > bufferSize) {
      return false;
    }
    float curve = *reinterpret_cast<float *>(data + offset);
    offset += sizeof(float);
    line.setCurve(curve);

    // Set progress ke 0.0 untuk parallel animation (semua barengan)
    line.setProgress(0.0f);
    line.setSpeed(0.005f);

    // Add ke vector
    customLines.push_back(line);
  }

  // Enable parallel mode untuk animate semua lines barengan
  loadParallelMode = true;

  return true;
}

//--------------------------------------------------------------
void FileManager::savePolygonsNA(ofBuffer &buffer,
                                 const std::vector<PolygonShape> &polygons,
                                 float radius) {
  // Write jumlah polygon
  int numPolygons = static_cast<int>(polygons.size());
  buffer.append(reinterpret_cast<const char *>(&numPolygons), sizeof(int));

  // Write setiap polygon dengan NORMALIZED positions
  for (const auto &polygon : polygons) {
    // Write jumlah vertices
    const vector<vec2> &vertices = polygon.getVertices();
    int numVertices = static_cast<int>(vertices.size());
    buffer.append(reinterpret_cast<const char *>(&numVertices), sizeof(int));

    // Write semua vertices dengan POSISI NORMALIZED (dibagi radius)
    for (const auto &vertex : vertices) {
      vec2 normalizedVertex = vertex / radius; // Normalize
      buffer.append(reinterpret_cast<const char *>(&normalizedVertex),
                    sizeof(vec2));
    }

    // Write fillColor (tidak perlu normalize)
    ofColor fillColor = polygon.getColor();
    buffer.append(reinterpret_cast<const char *>(&fillColor), sizeof(ofColor));
  }
}

//--------------------------------------------------------------
bool FileManager::loadPolygonsNA(ofBuffer &buffer, size_t &offset,
                                 std::vector<PolygonShape> &polygons,
                                 float radius) {
  char *data = buffer.getData();
  size_t bufferSize = buffer.size();

  // Read jumlah polygon
  if (offset + sizeof(int) > bufferSize) {
    return false;
  }
  int numPolygons = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Read setiap polygon
  for (int i = 0; i < numPolygons; i++) {
    // Read jumlah vertices
    if (offset + sizeof(int) > bufferSize) {
      return false;
    }
    int numVertices = *reinterpret_cast<int *>(data + offset);
    offset += sizeof(int);

    // Read semua vertices dan DENORMALIZE (kali radius)
    vector<vec2> vertices;
    for (int j = 0; j < numVertices; j++) {
      if (offset + sizeof(vec2) > bufferSize) {
        return false;
      }
      vec2 normalizedVertex = *reinterpret_cast<vec2 *>(data + offset);
      offset += sizeof(vec2);
      vec2 vertex = normalizedVertex * radius; // Denormalize
      vertices.push_back(vertex);
    }

    // Read fillColor
    if (offset + sizeof(ofColor) > bufferSize) {
      return false;
    }
    ofColor fillColor = *reinterpret_cast<ofColor *>(data + offset);
    offset += sizeof(ofColor);

    // Buat PolygonShape dengan index
    PolygonShape polygon(vertices, fillColor, i);

    // Tambah FadeInAnimation
    auto fadeIn = std::make_unique<FadeInAnimation>(fillColor.a, 0.003f);
    polygon.setAnimation(std::move(fadeIn));

    // Add ke vector
    polygons.push_back(std::move(polygon));
  }

  return true;
}

//--------------------------------------------------------------
FileManager::FileManager()
    : loadSequentialMode(false), loadParallelMode(false), currentLineIndex(0),
      loadSpeed(0.05f), loadAccumulator(0.0f), currentPolygonIndex(0) {}

//--------------------------------------------------------------
void FileManager::loadAllSequential(std::vector<CustomLine> &customLines,
                                    std::vector<PolygonShape> &polygons) {
  // Cek apakah file .na exists
  ofFile file(FILENAME);
  if (!file.exists()) {
    return;
  }

  // Read file ke buffer
  ofBuffer buffer = ofBufferFromFile(FILENAME);
  char *data = buffer.getData();
  size_t bufferSize = buffer.size();

  // Validasi ukuran minimal
  if (bufferSize < 64) {
    ofLog() << "Error: File terlalu kecil";
    return;
  }

  size_t offset = 0;

  // ===== VALIDASI HEADER =====
  // Magic number
  if (memcmp(data + offset, MAGIC_NUMBER, 4) != 0) {
    ofLog() << "Error: Invalid file format (wrong magic number)";
    return;
  }
  offset += 4;

  // Version
  int version = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);
  if (version != VERSION) {
    ofLog() << "Error: Unsupported version: " << version;
    return;
  }

  // Template name length
  int nameLen = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Skip template name (tidak perlu di-load untuk sequential mode)
  offset += nameLen;

  // Global radius (diperlukan untuk denormalize)
  float globalRadius = *reinterpret_cast<float *>(data + offset);
  offset += sizeof(float);

  // Skip reserved (20 bytes)
  offset += 20;

  // ===== LOAD CUSTOM LINES KE BUFFER =====
  if (offset >= bufferSize) {
    ofLog() << "Error: Unexpected end of file after header";
    return;
  }

  int numLines = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Load customLines jika vectors kosong
  if (customLines.empty()) {
    loadedLinesBuffer.clear();

    for (int i = 0; i < numLines; i++) {
      CustomLine line;

      // Read jumlah points
      int numPoints = *reinterpret_cast<int *>(data + offset);
      offset += sizeof(int);

      // Read semua points dan DENORMALIZE (kali radius)
      vector<vec2> points;
      for (int j = 0; j < numPoints; j++) {
        vec2 normalizedPoint = *reinterpret_cast<vec2 *>(data + offset);
        offset += sizeof(vec2);
        vec2 point = normalizedPoint * globalRadius; // Denormalize
        points.push_back(point);
      }
      line.setPoints(points);

      // Read color
      ofColor color = *reinterpret_cast<ofColor *>(data + offset);
      offset += sizeof(ofColor);
      line.setColor(color);

      // Read lineWidth
      float lineWidth = *reinterpret_cast<float *>(data + offset);
      offset += sizeof(float);
      line.setLineWidth(lineWidth);

      // Read curve
      float curve = *reinterpret_cast<float *>(data + offset);
      offset += sizeof(float);
      line.setCurve(curve);

      // Set initial animation state
      line.setProgress(0.0f);
      line.setSpeed(0.02f);

      // Add ke buffer
      loadedLinesBuffer.push_back(line);
    }
  }

  // ===== LOAD POLYGONS KE BUFFER =====
  if (offset >= bufferSize) {
    ofLog() << "Error: Unexpected end of file after custom lines";
    return;
  }

  int numPolygons = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Load polygons jika vectors kosong
  if (polygons.empty()) {
    loadedPolygonsBuffer.clear();

    for (int i = 0; i < numPolygons; i++) {
      // Read jumlah vertices
      int numVertices = *reinterpret_cast<int *>(data + offset);
      offset += sizeof(int);

      // Read semua vertices dan DENORMALIZE (kali radius)
      vector<vec2> vertices;
      for (int j = 0; j < numVertices; j++) {
        vec2 normalizedVertex = *reinterpret_cast<vec2 *>(data + offset);
        offset += sizeof(vec2);
        vec2 vertex = normalizedVertex * globalRadius; // Denormalize
        vertices.push_back(vertex);
      }

      // Read fillColor
      ofColor fillColor = *reinterpret_cast<ofColor *>(data + offset);
      offset += sizeof(ofColor);

      // Buat PolygonShape dengan index
      PolygonShape polygon(vertices, fillColor, i);

      // Tambah FadeInAnimation
      auto fadeIn = std::make_unique<FadeInAnimation>(fillColor.a, 0.003f);
      polygon.setAnimation(std::move(fadeIn));

      // Add ke buffer
      loadedPolygonsBuffer.push_back(std::move(polygon));
    }
  }

  // Reset index dan mulai mode sequential
  currentLineIndex = 0;
  currentPolygonIndex = 0;
  loadAccumulator = 0.0f;
  loadSequentialMode = true;
}

//--------------------------------------------------------------
void FileManager::clearCustomLines(std::vector<CustomLine> &customLines) {
  customLines.clear();
}

//--------------------------------------------------------------
void FileManager::cancelSequentialLoad() {
  // Clear semua buffer
  loadedLinesBuffer.clear();
  loadedPolygonsBuffer.clear();

  // Reset state
  loadSequentialMode = false;
  loadParallelMode = false;
  currentLineIndex = 0;
  currentPolygonIndex = 0;
  loadAccumulator = 0.0f;
}

//--------------------------------------------------------------
void FileManager::updateSequentialLoad(std::vector<CustomLine> &customLines,
                                       std::vector<PolygonShape> &polygons) {
  // PARALLEL MODE: Update semua lines barengan
  if (loadParallelMode) {
    bool allComplete = true;

    // Update progress semua lines
    for (auto &line : customLines) {
      line.updateProgress();
      if (line.getProgress() < 1.0f) {
        allComplete = false;
      }
    }

    // Selesai parallel mode
    if (allComplete) {
      loadParallelMode = false;
    }

    return;
  }

  // SEQUENTIAL MODE: Update satu per satu (lines + polygons)
  if (!loadSequentialMode) {
    return;
  }

  // === ADD NEW ITEMS (lines + polygons) ===
  // Cek apakah masih ada item yang belum di-load (lines ATAU polygons)
  bool stillHasItems =
      (currentLineIndex < static_cast<int>(loadedLinesBuffer.size())) ||
      (currentPolygonIndex < static_cast<int>(loadedPolygonsBuffer.size()));

  // Add item baru dengan delay
  if (stillHasItems) {
    loadAccumulator += loadSpeed;
  }

  // Cek apakah saatnya add item baru (line atau polygon)
  if (loadAccumulator >= 1.0f) {
    // Prioritas: Add line dulu kalau ada
    if (currentLineIndex < static_cast<int>(loadedLinesBuffer.size())) {
      customLines.push_back(loadedLinesBuffer[currentLineIndex]);
      currentLineIndex++;
      loadAccumulator = 0.0f;
    }
    // Kalau tidak ada line lagi, add polygon
    else if (currentPolygonIndex <
             static_cast<int>(loadedPolygonsBuffer.size())) {
      polygons.push_back(std::move(loadedPolygonsBuffer[currentPolygonIndex]));
      currentPolygonIndex++;
      loadAccumulator = 0.0f;
    }
  }

  // === UPDATE PROGRESS ===
  // Update progress untuk semua lines yang belum complete
  for (auto &line : customLines) {
    line.updateProgress();
  }

  // Update animation untuk semua polygons
  for (auto &polygon : polygons) {
    polygon.update();
  }

  // === CEK SELESAI ===
  // Cek apakah sudah selesai (semua lines & polygons loaded dan semua complete)
  bool allComplete = true;

  // Cek lines
  if (currentLineIndex < static_cast<int>(loadedLinesBuffer.size())) {
    allComplete = false; // Masih ada lines yang belum di-load
  } else {
    // Cek apakah semua lines sudah complete
    for (const auto &line : customLines) {
      if (line.getProgress() < 1.0f) {
        allComplete = false;
        break;
      }
    }
  }

  // Cek polygons
  if (currentPolygonIndex < static_cast<int>(loadedPolygonsBuffer.size())) {
    allComplete = false; // Masih ada polygons yang belum di-load
  }

  if (allComplete) {
    loadSequentialMode = false;   // Selesai
    loadedLinesBuffer.clear();    // Bersihkan buffer lines
    loadedPolygonsBuffer.clear(); // Bersihkan buffer polygons
    loadAccumulator = 0.0f;       // Reset accumulator
  }
}

//--------------------------------------------------------------
bool FileManager::isLoadSequentialMode() const { return loadSequentialMode; }

//--------------------------------------------------------------
bool FileManager::isLoadParallelMode() const { return loadParallelMode; }

//--------------------------------------------------------------
int FileManager::getCurrentLoadIndex() const { return currentLineIndex; }

//--------------------------------------------------------------
int FileManager::getTotalLoadedLines() const {
  return static_cast<int>(loadedLinesBuffer.size());
}

//--------------------------------------------------------------
void FileManager::setLoadSpeed(float speed) { loadSpeed = speed; }

//--------------------------------------------------------------
float FileManager::getLoadSpeed() const { return loadSpeed; }
