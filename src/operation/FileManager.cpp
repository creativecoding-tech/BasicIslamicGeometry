#include "FileManager.h"
#include "../anim/FadeInAnimation.h"
#include "../anim/FillAnimation.h"
#include "../anim/GradientAnimation.h"
#include "../anim/RotateLeftAnimation.h"
#include "../anim/WobbleAnimation.h"
#include "../anim/WobbleFillAnimation.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
void FileManager::saveAll(
    const std::string &templateName, float globalRadius,
    const std::vector<CustomLine> &customLines,
    const std::vector<PolygonShape> &polygons,
    const std::vector<std::unique_ptr<DotShape>> &userDots,
    float currentLineWidth, bool labelsVisible, bool dotsVisible,
    bool showUserDot, const std::string &filepath) {
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

  // Byte 6: showUserDot (bool/1 byte)
  buffer.append(reinterpret_cast<const char *>(&showUserDot), sizeof(bool));

  // Byte 7-19: Remaining Reserved (13 bytes)
  char reserved[13] = {0};
  buffer.append(reserved, 13);

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

  // ===== USER DOTS =====
  int numDots = static_cast<int>(userDots.size());
  buffer.append(reinterpret_cast<const char *>(&numDots), sizeof(int));

  // Simpan userDots dengan NORMALIZED positions (dibagi radius)
  saveUserDotsNA(buffer, userDots, globalRadius);

  // Write ke file langsung ke filepath
  ofBufferToFile(filepath, buffer);
}

//--------------------------------------------------------------
bool FileManager::loadAll(std::string &outTemplateName, float &outGlobalRadius,
                          std::vector<CustomLine> &customLines,
                          std::vector<PolygonShape> &polygons,
                          std::vector<std::unique_ptr<DotShape>> &userDots,
                          float &outLineWidth, bool &outLabelsVisible,
                          bool &outDotsVisible, bool &outShowUserDot,
                          const std::string &filepath) {
  // Baca dari filepath yang diberikan (wajib di-parameter)
  ofFile file(filepath);
  if (!file.exists()) {
    return false;
  }

  // Read file ke buffer
  ofBuffer buffer = ofBufferFromFile(filepath);
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
  // Support version 1, 2, 3, and 4
  if (version != 1 && version != 2 && version != 3 && version != 4) {
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
    outShowUserDot = true; // Default true
    offset += 20;
  } else if (version >= 2) {
    // Version >= 2: Read from what was previously reserved

    // currentLineWidth (float)
    outLineWidth = *reinterpret_cast<float *>(data + offset);
    offset += sizeof(float);

    // labelsVisible (bool)
    outLabelsVisible = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    // dotsVisible (bool)
    outDotsVisible = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    // showUserDot (bool)
    outShowUserDot = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    // Skip remaining reserved (13 bytes)
    offset += 13;
  }

  // ===== LOAD CUSTOM LINES =====
  if (offset >= bufferSize) {
    ofLog() << "Error: Unexpected end of file after header";
    return false;
  }

  int numLines = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  customLines.clear();
  if (!loadCustomLinesNA(buffer, offset, customLines, outGlobalRadius,
                         version)) {
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
  if (!loadPolygonsNA(buffer, offset, polygons, outGlobalRadius, version)) {
    ofLog() << "Error: Failed to load polygons";
    return false;
  }

  // ===== LOAD USER DOTS =====
  if (offset >= bufferSize) {
    // Backward compatibility: file lama tidak punya userDots
    userDots.clear();
  } else {
    int numDots = *reinterpret_cast<int *>(data + offset);
    offset += sizeof(int);

    userDots.clear();
    if (!loadUserDotsNA(buffer, offset, userDots, outGlobalRadius)) {
      ofLog() << "Error: Failed to load user dots";
      return false;
    }
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

    // Write color, lineWidth, curve, dan label (tidak perlu normalize)
    ofColor color = line.getColor();
    float lineWidth = line.getLineWidth();
    float curve = line.getCurve();
    std::string label = line.getLabel();

    buffer.append(reinterpret_cast<const char *>(&color), sizeof(ofColor));
    buffer.append(reinterpret_cast<const char *>(&lineWidth), sizeof(float));
    buffer.append(reinterpret_cast<const char *>(&curve), sizeof(float));

    // Write label string (length + data)
    int labelLength = static_cast<int>(label.size());
    buffer.append(reinterpret_cast<const char *>(&labelLength), sizeof(int));
    if (labelLength > 0) {
      buffer.append(label.c_str(), labelLength);
    }

    // Write isDuplicate dan axisLock
    bool isDuplicate = line.getIsDuplicate();
    AxisLock axisLock = line.getAxisLock();
    buffer.append(reinterpret_cast<const char *>(&isDuplicate), sizeof(bool));
    buffer.append(reinterpret_cast<const char *>(&axisLock), sizeof(AxisLock));

    // Write isTessellated for version 3
    bool isTessellated = line.isTessellated();
    buffer.append(reinterpret_cast<const char *>(&isTessellated), sizeof(bool));
  }
}

//--------------------------------------------------------------
bool FileManager::loadCustomLinesNA(ofBuffer &buffer, size_t &offset,
                                    std::vector<CustomLine> &customLines,
                                    float radius, int version) {
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

    // Read label string
    if (offset + sizeof(int) > bufferSize) {
      return false;
    }
    int labelLength = *reinterpret_cast<int *>(data + offset);
    offset += sizeof(int);

    std::string label = "";
    if (labelLength > 0) {
      if (offset + labelLength > bufferSize) {
        return false;
      }
      label.assign(data + offset, labelLength);
      offset += labelLength;
    }
    line.setLabel(label);

    // Read isDuplicate
    if (offset + sizeof(bool) > bufferSize) {
      return false;
    }
    bool isDuplicate = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);
    line.setIsDuplicate(isDuplicate);

    // Read axisLock
    if (offset + sizeof(AxisLock) > bufferSize) {
      return false;
    }
    AxisLock axisLock = *reinterpret_cast<AxisLock *>(data + offset);
    offset += sizeof(AxisLock);
    line.setAxisLock(axisLock);

    // Read isTessellated (VERSION >= 3)
    if (version >= 3) {
      if (offset + sizeof(bool) > bufferSize) {
        return false;
      }
      bool isTessellated = *reinterpret_cast<bool *>(data + offset);
      offset += sizeof(bool);
      line.setTessellated(isTessellated);
    } else {
      line.setTessellated(false);
    }

    // Set progress ke 0.0 untuk parallel animation (semua barengan)
    line.setProgress(0.0f);
    line.setSpeed(
        0.18f *
        animationSpeedMultiplier); // Delta time calibrated (0.003f * 60 FPS)

    // Simpan titik asli dan hitung baseRadius untuk mencegah floating point
    // drift saat dimodifikasi slider Radius ⭐ NEW
    line.saveOriginalPoints(radius);

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

    // Write isTessellated for version 3
    bool isTessellated = polygon.isTessellated();
    buffer.append(reinterpret_cast<const char *>(&isTessellated), sizeof(bool));

    // Write sourceTessellation data for version 4 (only if not tessellated
    // itself)
    if (!isTessellated) {
      std::string sourceFile = polygon.getSourceTessellationFile();
      float sourceRadius = polygon.getSourceTessellationRadius();

      int fileLen = static_cast<int>(sourceFile.length());
      buffer.append(reinterpret_cast<const char *>(&fileLen), sizeof(int));
      if (fileLen > 0) {
        buffer.append(sourceFile.c_str(), fileLen);
      }
      buffer.append(reinterpret_cast<const char *>(&sourceRadius),
                    sizeof(float));
    }
  }
}

//--------------------------------------------------------------
bool FileManager::loadPolygonsNA(ofBuffer &buffer, size_t &offset,
                                 std::vector<PolygonShape> &polygons,
                                 float radius, int version) {
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

    // Read isTessellated (VERSION >= 3)
    bool isTessellated = false;
    if (version >= 3) {
      if (offset + sizeof(bool) > bufferSize) {
        return false;
      }
      isTessellated = *reinterpret_cast<bool *>(data + offset);
      offset += sizeof(bool);
    }

    // Read sourceTessellation data (VERSION >= 4)
    std::string sourceFile = "";
    float sourceRadius = 10.0f;
    if (version >= 4 && !isTessellated) {
      if (offset + sizeof(int) > bufferSize) {
        return false;
      }
      int fileLen = *reinterpret_cast<int *>(data + offset);
      offset += sizeof(int);

      if (fileLen > 0) {
        if (offset + fileLen > bufferSize) {
          return false;
        }
        sourceFile.assign(data + offset, fileLen);
        offset += fileLen;
      }

      if (offset + sizeof(float) > bufferSize) {
        return false;
      }
      sourceRadius = *reinterpret_cast<float *>(data + offset);
      offset += sizeof(float);
    }

    // Buat PolygonShape dengan atau tanpa animation tergantung mode
    PolygonShape polygon = createPolygonWithAnimation(vertices, fillColor, i);
    polygon.setLoadedFromFile(true);       // Flag sebagai loaded dari file .nay
    polygon.setTessellated(isTessellated); // Restore tessellation state
    if (version >= 4 && !isTessellated && !sourceFile.empty()) {
      polygon.setSourceTessellation(sourceFile, sourceRadius);
    }

    // Simpan titik asli dan hitung baseRadius untuk mencegah floating point
    // drift saat dimodifikasi slider Radius ⭐ NEW
    polygon.saveOriginalVertices(radius);

    polygons.push_back(std::move(polygon));
  }

  return true;
}

//--------------------------------------------------------------
void FileManager::saveUserDotsNA(
    ofBuffer &buffer, const std::vector<std::unique_ptr<DotShape>> &userDots,
    float radius) {
  // Write jumlah dot
  int numDots = static_cast<int>(userDots.size());
  buffer.append(reinterpret_cast<const char *>(&numDots), sizeof(int));

  // Write setiap dot dengan NORMALIZED positions
  for (const auto &dot : userDots) {
    if (!dot)
      continue;

    // Write position (NORMALIZED)
    vec2 pos = dot->getPosition();
    vec2 normalizedPos = pos / radius; // Normalize
    buffer.append(reinterpret_cast<const char *>(&normalizedPos), sizeof(vec2));

    // Write lower bound (NORMALIZED)
    vec2 lowerBound = dot->getLowerBound();
    vec2 normalizedLowerBound = lowerBound / radius; // Normalize
    buffer.append(reinterpret_cast<const char *>(&normalizedLowerBound),
                  sizeof(vec2));

    // Write radius
    float dotRadius = dot->getRadius();
    buffer.append(reinterpret_cast<const char *>(&dotRadius), sizeof(float));

    // Write color
    ofColor color = dot->getColor();
    buffer.append(reinterpret_cast<const char *>(&color), sizeof(ofColor));
  }
}

//--------------------------------------------------------------
bool FileManager::loadUserDotsNA(
    ofBuffer &buffer, size_t &offset,
    std::vector<std::unique_ptr<DotShape>> &userDots, float radius) {
  char *data = buffer.getData();
  size_t bufferSize = buffer.size();

  // Read jumlah dot
  if (offset + sizeof(int) > bufferSize) {
    return false;
  }
  int numDots = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Read setiap dot
  for (int i = 0; i < numDots; i++) {
    // Read position dan DENORMALIZE (kali radius)
    if (offset + sizeof(vec2) > bufferSize) {
      return false;
    }
    vec2 normalizedPos = *reinterpret_cast<vec2 *>(data + offset);
    offset += sizeof(vec2);
    vec2 pos = normalizedPos * radius; // Denormalize

    // Read lower bound dan DENORMALIZE
    if (offset + sizeof(vec2) > bufferSize) {
      return false;
    }
    vec2 normalizedLowerBound = *reinterpret_cast<vec2 *>(data + offset);
    offset += sizeof(vec2);
    vec2 lowerBound = normalizedLowerBound * radius; // Denormalize

    // Read radius
    if (offset + sizeof(float) > bufferSize) {
      return false;
    }
    float dotRadius = *reinterpret_cast<float *>(data + offset);
    offset += sizeof(float);

    // Read color
    if (offset + sizeof(ofColor) > bufferSize) {
      return false;
    }
    ofColor color = *reinterpret_cast<ofColor *>(data + offset);
    offset += sizeof(ofColor);

    // Buat DotShape
    auto dotShape = std::make_unique<DotShape>(pos, "Dot", dotRadius);
    dotShape->setLowerBound(lowerBound);
    dotShape->setColor(color);
    dotShape->progress = 1.0f;

    // Simpan titik asli dan hitung baseRadius untuk mencegah floating point
    // drift saat dimodifikasi slider Radius ⭐ NEW
    dotShape->saveOriginalPosition(radius);

    userDots.push_back(std::move(dotShape));
  }

  return true;
}

//--------------------------------------------------------------
FileManager::FileManager()
    : loadSequentialMode(false), loadParallelMode(false), currentLineIndex(0),
      loadSpeed(0.05f), loadAccumulator(0.0f), currentPolygonIndex(0),
      polygonAnimationMode(PolygonAnimationMode::NO_ANIMATION),
      animationSpeedMultiplier(1.0f) {}

//--------------------------------------------------------------
void FileManager::loadAllSequential(
    std::string &outTemplateName, float &outGlobalRadius, float &outLineWidth,
    bool &outLabelsVisible, bool &outDotsVisible,
    std::vector<CustomLine> &customLines, std::vector<PolygonShape> &polygons,
    std::vector<std::unique_ptr<DotShape>> &userDots, bool &outShowUserDot,
    const std::string &filepath) {
  // Cek apakah file exists
  ofFile file(filepath);
  if (!file.exists()) {
    return;
  }

  // Read file ke buffer
  ofBuffer buffer = ofBufferFromFile(filepath);
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
  // Support version 1, 2, 3, and 4
  if (version != 1 && version != 2 && version != 3 && version != 4) {
    ofLog() << "Error: Unsupported version: " << version;
    return;
  }

  // Template name length
  int nameLen = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Read template name
  if (nameLen <= 0 || nameLen > 256) {
    ofLog() << "Error: Invalid template name length";
    return;
  }
  outTemplateName = std::string(data + offset, nameLen);
  offset += nameLen;

  // Global radius (diperlukan untuk denormalize)
  outGlobalRadius = *reinterpret_cast<float *>(data + offset);
  offset += sizeof(float);

  // Parse Additional Settings based on version
  if (version == 1) {
    // Version 1: Set default values dan skip reserved 20 bytes
    outLineWidth = 4.0f;
    outLabelsVisible = true;
    outDotsVisible = true;
    outShowUserDot = true; // Default true
    offset += 20;
  } else if (version >= 2) {
    // Version >= 2: Read dari apa yang sebelumnya reserved
    outLineWidth = *reinterpret_cast<float *>(data + offset);
    offset += sizeof(float);

    outLabelsVisible = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    outDotsVisible = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    // showUserDot (bool)
    outShowUserDot = *reinterpret_cast<bool *>(data + offset);
    offset += sizeof(bool);

    // Skip remaining reserved (13 bytes)
    offset += 13;
  }

  // ===== LOAD CUSTOM LINES KE BUFFER =====
  if (offset >= bufferSize) {
    ofLog() << "Error: Unexpected end of file after header";
    return;
  }

  int numLines = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Baca numLines yang KEDUA (dari saveCustomLinesNA)
  int numLines2 = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Validasi: numLines harus sama dengan numLines2
  if (numLines != numLines2) {
    ofLog() << "Warning: numLines mismatch (" << numLines << " vs " << numLines2
            << ")";
  }

  // Load customLines jika vectors kosong
  if (customLines.empty()) {
    loadedLinesBuffer.clear();

    for (int i = 0; i < numLines; i++) {
      CustomLine line;

      // Read jumlah points
      if (offset + sizeof(int) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading numPoints";
        return;
      }
      int numPoints = *reinterpret_cast<int *>(data + offset);
      offset += sizeof(int);

      // Read semua points dan DENORMALIZE (kali radius)
      vector<vec2> points;
      for (int j = 0; j < numPoints; j++) {
        if (offset + sizeof(vec2) > bufferSize) {
          ofLog() << "Error: Unexpected end of file reading point";
          return;
        }
        vec2 normalizedPoint = *reinterpret_cast<vec2 *>(data + offset);
        offset += sizeof(vec2);
        vec2 point = normalizedPoint * outGlobalRadius; // Denormalize
        points.push_back(point);
      }
      line.setPoints(points);

      // Read color
      if (offset + sizeof(ofColor) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading color";
        return;
      }
      ofColor color = *reinterpret_cast<ofColor *>(data + offset);
      offset += sizeof(ofColor);
      line.setColor(color);

      // Read lineWidth
      if (offset + sizeof(float) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading lineWidth";
        return;
      }
      float lineWidth = *reinterpret_cast<float *>(data + offset);
      offset += sizeof(float);
      line.setLineWidth(lineWidth);

      // Read curve
      if (offset + sizeof(float) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading curve";
        return;
      }
      float curve = *reinterpret_cast<float *>(data + offset);
      offset += sizeof(float);
      line.setCurve(curve);

      // Read label string
      if (offset + sizeof(int) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading label length";
        return;
      }
      int labelLength = *reinterpret_cast<int *>(data + offset);
      offset += sizeof(int);

      std::string label = "";
      if (labelLength > 0) {
        if (offset + labelLength > bufferSize) {
          ofLog() << "Error: Unexpected end of file reading label";
          return;
        }
        label.assign(data + offset, labelLength);
        offset += labelLength;
      }
      line.setLabel(label);

      // Read isDuplicate
      if (offset + sizeof(bool) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading isDuplicate";
        return;
      }
      bool isDuplicate = *reinterpret_cast<bool *>(data + offset);
      offset += sizeof(bool);
      line.setIsDuplicate(isDuplicate);

      // Read axisLock
      if (offset + sizeof(AxisLock) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading axisLock";
        return;
      }
      AxisLock axisLock = *reinterpret_cast<AxisLock *>(data + offset);
      offset += sizeof(AxisLock);
      line.setAxisLock(axisLock);

      // Read isTessellated (VERSION >= 3)
      if (version >= 3) {
        if (offset + sizeof(bool) > bufferSize) {
          ofLog() << "Error: Unexpected end of file reading isTessellated";
          return;
        }
        bool isTessellated = *reinterpret_cast<bool *>(data + offset);
        offset += sizeof(bool);
        line.setTessellated(isTessellated);
      } else {
        line.setTessellated(false);
      }

      // Set initial animation state
      line.setProgress(0.0f);
      line.setSpeed(
          1.8f *
          animationSpeedMultiplier); // Delta time calibrated (0.03f * 60 FPS)

      // Add ke buffer HANYA jika shouldLoadCustomLines = true
      if (shouldLoadCustomLines) {
        loadedLinesBuffer.push_back(line);
      }
    }
  }

  // ===== LOAD POLYGONS KE BUFFER =====
  if (offset >= bufferSize) {
    ofLog() << "Error: Unexpected end of file after custom lines";
    return;
  }

  int numPolygons = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Baca numPolygons yang KEDUA (dari savePolygonsNA)
  int numPolygons2 = *reinterpret_cast<int *>(data + offset);
  offset += sizeof(int);

  // Validasi: numPolygons harus sama dengan numPolygons2
  if (numPolygons != numPolygons2) {
    ofLog() << "Warning: numPolygons mismatch (" << numPolygons << " vs "
            << numPolygons2 << ")";
  }

  // Load polygons jika vectors kosong
  if (polygons.empty()) {
    loadedPolygonsBuffer.clear();

    for (int i = 0; i < numPolygons; i++) {
      // Read jumlah vertices
      if (offset + sizeof(int) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading numVertices";
        return;
      }
      int numVertices = *reinterpret_cast<int *>(data + offset);
      offset += sizeof(int);

      // Read semua vertices dan DENORMALIZE (kali radius)
      vector<vec2> vertices;
      for (int j = 0; j < numVertices; j++) {
        if (offset + sizeof(vec2) > bufferSize) {
          ofLog() << "Error: Unexpected end of file reading vertex";
          return;
        }
        vec2 normalizedVertex = *reinterpret_cast<vec2 *>(data + offset);
        offset += sizeof(vec2);
        vec2 vertex = normalizedVertex * outGlobalRadius; // Denormalize
        vertices.push_back(vertex);
      }

      // Read fillColor
      if (offset + sizeof(ofColor) > bufferSize) {
        ofLog() << "Error: Unexpected end of file reading fillColor";
        return;
      }
      ofColor fillColor = *reinterpret_cast<ofColor *>(data + offset);
      offset += sizeof(ofColor);

      // Read isTessellated (VERSION >= 3)
      bool isTessellated = false;
      if (version >= 3) {
        if (offset + sizeof(bool) > bufferSize) {
          ofLog() << "Error: Unexpected end of file reading isTessellated";
          return;
        }
        isTessellated = *reinterpret_cast<bool *>(data + offset);
        offset += sizeof(bool);
      }

      // Read sourceTessellation data (VERSION >= 4)
      std::string sourceFile = "";
      float sourceRadius = 10.0f;
      if (version >= 4 && !isTessellated) {
        if (offset + sizeof(int) > bufferSize) {
          ofLog() << "Error: Unexpected end of file reading "
                     "sourceTessellationFile length";
          return;
        }
        int fileLen = *reinterpret_cast<int *>(data + offset);
        offset += sizeof(int);

        if (fileLen > 0) {
          if (offset + fileLen > bufferSize) {
            ofLog() << "Error: Unexpected end of file reading "
                       "sourceTessellationFile";
            return;
          }
          sourceFile.assign(data + offset, fileLen);
          offset += fileLen;
        }

        if (offset + sizeof(float) > bufferSize) {
          ofLog() << "Error: Unexpected end of file reading "
                     "sourceTessellationRadius";
          return;
        }
        sourceRadius = *reinterpret_cast<float *>(data + offset);
        offset += sizeof(float);
      }

      // Buat PolygonShape dengan atau tanpa animation tergantung mode
      PolygonShape polygon = createPolygonWithAnimation(vertices, fillColor, i);
      polygon.setLoadedFromFile(true); // Flag sebagai loaded dari file .nay
      polygon.setTessellated(isTessellated); // Restore tessellation state
      if (version >= 4 && !isTessellated && !sourceFile.empty()) {
        polygon.setSourceTessellation(sourceFile, sourceRadius);
      }
      loadedPolygonsBuffer.push_back(std::move(polygon));
    }
  }

  // ===== LOAD USER DOTS (langsung, bukan sequential) =====
  if (offset >= bufferSize) {
    // Backward compatibility: file lama tidak punya userDots
    userDots.clear();
  } else {
    int numDots = *reinterpret_cast<int *>(data + offset);
    offset += sizeof(int);

    userDots.clear();
    // Load userDots langsung (tanpa buffer sequential)
    if (!loadUserDotsNA(buffer, offset, userDots, outGlobalRadius)) {
      ofLog() << "Error: Failed to load user dots";
      return;
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
  // PARALLEL MODE: Instantly load ALL items (Parallel/Instant Load)
  if (loadParallelMode) {
    // 1. Move ALL remaining lines from buffer to customLines
    while (currentLineIndex < static_cast<int>(loadedLinesBuffer.size())) {
      customLines.push_back(loadedLinesBuffer[currentLineIndex]);
      currentLineIndex++;
    }

    // 2. Move ALL remaining polygons from buffer to polygons
    while (currentPolygonIndex <
           static_cast<int>(loadedPolygonsBuffer.size())) {
      polygons.push_back(std::move(loadedPolygonsBuffer[currentPolygonIndex]));
      currentPolygonIndex++;
    }

    // 3. Update progress for ALL lines (to ensure they are ready/visible or
    // animating) Note: If we want them to START animating from 0, we just push
    // them. If we want them to finish instantly, we set progress=1.0. User
    // wants "With Polygon Draw" -> Concurrent Animation. So we just push them.
    // animation progress is handled by ofApp update loop.

    // We do NOT finish them instantly here. We just LOAD them instantly.

    // 4. Turn off parallel mode (loading is done)
    loadParallelMode = false;
    return;
  }

  // SEQUENTIAL MODE: Update satu per satu (lines + polygons)
  if (!loadSequentialMode) {
    return;
  }

  // === UPDATE PROGRESS DULU ===
  // Hanya update line terakhir yang sedang animate
  if (!customLines.empty() && currentLineIndex > 0) {
    customLines.back().updateProgress();
  }

  // Update animation untuk semua polygons yang sudah ada
  for (auto &polygon : polygons) {
    polygon.update();
  }

  // === ADD NEW ITEMS (lines + polygons) ===
  // Cek apakah masih ada item yang belum di-load (lines ATAU polygons)
  bool stillHasItems =
      (currentLineIndex < static_cast<int>(loadedLinesBuffer.size())) ||
      (currentPolygonIndex < static_cast<int>(loadedPolygonsBuffer.size()));

  if (stillHasItems) {
    // Cek apakah line terakhir sudah complete sebelum add baru
    bool canAddNew = true;
    if (!customLines.empty() && currentLineIndex > 0) {
      if (customLines.back().getProgress() < 1.0f) {
        canAddNew = false; // Line terakhir belum complete, jangan add baru
      }
    }

    if (canAddNew) {
      // Calculate effective load speed depending on item type
      // Base load speed is 0.05f per frame (approx 3 items per second at 1.0x)
      float currentLoadSpeed = loadSpeed;

      // SPEED SCALING:
      // If we are loading polygons, scale speed by polygonSpeedMultiplier
      // If Parallel Mode is ON, force a very high speed to load everything
      // effectively instantly per frame limit
      if (loadParallelMode) {
        currentLoadSpeed = 1000.0f; // Load almost everything in one go
      } else {
        if (!(currentLineIndex < static_cast<int>(loadedLinesBuffer.size())) &&
            (currentPolygonIndex <
             static_cast<int>(loadedPolygonsBuffer.size()))) {
          // We are loading polygons now
          currentLoadSpeed *= polygonSpeedMultiplier;
        }
      }

      loadAccumulator += currentLoadSpeed;

      // BATCH LOADING LOOP
      // Process multiple items per frame if speed is high enough!
      // This fixes the "slow no animation" issue.
      while (loadAccumulator >= 1.0f) {

        // Safety check to break if we are done with everything
        bool anyAdded = false;

        // Prioritas: Add line dulu kalau ada
        if (currentLineIndex < static_cast<int>(loadedLinesBuffer.size())) {
          customLines.push_back(loadedLinesBuffer[currentLineIndex]);
          currentLineIndex++;
          anyAdded = true;
        }
        // Kalau tidak ada line lagi, add polygon
        else if (currentPolygonIndex <
                 static_cast<int>(loadedPolygonsBuffer.size())) {
          polygons.push_back(
              std::move(loadedPolygonsBuffer[currentPolygonIndex]));
          currentPolygonIndex++;
          anyAdded = true;
        }

        if (!anyAdded) {
          loadAccumulator = 0.0f; // Nothing left to add
          break;
        }

        loadAccumulator -= 1.0f; // Decrement accumulator (keep remainder)
      }
    }
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
int FileManager::getTotalLoadedPolygons() const {
  return static_cast<int>(loadedPolygonsBuffer.size());
}

//--------------------------------------------------------------
int FileManager::getTotalOriginalPolygons() const {
  int count = 0;
  for (const auto &p : loadedPolygonsBuffer) {
    if (!p.isTessellated()) {
      count++;
    }
  }
  return count;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
PolygonShape
FileManager::createPolygonWithAnimation(const std::vector<vec2> &vertices,
                                        ofColor color, int index) {
  // Buat PolygonShape dengan animation berdasarkan polygonAnimationMode

  // Base speeds setup
  // NOTE: polygonSpeedMultiplier scales these base speeds.

  switch (polygonAnimationMode) {
  case PolygonAnimationMode::FADE_IN: {
    // Default speed: 2.0f (approx 0.5s fade) -> Scaled by multiplier
    float speed = 2.0f * polygonSpeedMultiplier;
    auto fadeIn = std::make_unique<FadeInAnimation>(color.a, speed);
    return PolygonShape(vertices, color, index, std::move(fadeIn));
  }
  case PolygonAnimationMode::WOBBLE: {
    // Wobble Animation
    float freq = 1.8f * polygonSpeedMultiplier;
    auto wobble = std::make_unique<WobbleAnimation>(30.0f, 5.0f, freq);
    return PolygonShape(vertices, color, index, std::move(wobble));
  }
  case PolygonAnimationMode::WAVE_FILL: {
    // Fill Animation - speed controls how fast water rises
    // Base speed 0.3f (approx 3.3s fill)
    float speed = 0.3f * polygonSpeedMultiplier;
    auto fill = std::make_unique<FillAnimation>(20.0f, 4.0f, speed);
    return PolygonShape(vertices, color, index, std::move(fill));
  }
  case PolygonAnimationMode::WOBBLE_FILL: {
    // Wobble Fill - speed controls fill rate
    // Base speed 0.6f (approx 1.6s fill)
    float fillSpeed = 0.6f * polygonSpeedMultiplier;
    float freq = 1.8f * polygonSpeedMultiplier;
    auto wobbleFill = std::make_unique<WobbleFillAnimation>(30.0f, 5.0f, freq,
                                                            1.0f, fillSpeed);
    return PolygonShape(vertices, color, index, std::move(wobbleFill));
  }
  case PolygonAnimationMode::GRADIENT: {
    // Gradient Animation - speed and frequency
    // Base speed 0.5f (approx 2s cycle)
    float speed = 0.5f * polygonSpeedMultiplier;
    float freq = 5.0f;
    auto gradient = std::make_unique<GradientAnimation>(speed, freq, 5.0f);
    return PolygonShape(vertices, color, index, std::move(gradient));
  }
  case PolygonAnimationMode::NO_ANIMATION:
  default: {
    return PolygonShape(vertices, color, index);
  }
  }
}

//--------------------------------------------------------------
void FileManager::setLoadSpeed(float speed) { loadSpeed = speed; }

//--------------------------------------------------------------
float FileManager::getLoadSpeed() const { return loadSpeed; }

//--------------------------------------------------------------
void FileManager::setAnimationSpeedMultiplier(float multiplier) {
  animationSpeedMultiplier = multiplier;
}

//--------------------------------------------------------------
float FileManager::getAnimationSpeedMultiplier() const {
  return animationSpeedMultiplier;
}

//--------------------------------------------------------------
void FileManager::setPolygonAnimationMode(PolygonAnimationMode mode) {
  polygonAnimationMode = mode;
}

//--------------------------------------------------------------
PolygonAnimationMode FileManager::getPolygonAnimationMode() const {
  return polygonAnimationMode;
}

//--------------------------------------------------------------
void FileManager::setShouldLoadCustomLines(bool shouldLoad) {
  shouldLoadCustomLines = shouldLoad;
}

//--------------------------------------------------------------
bool FileManager::getShouldLoadCustomLines() const {
  return shouldLoadCustomLines;
}

//--------------------------------------------------------------
void FileManager::setLoadParallelMode(bool enabled) {
  loadParallelMode = enabled;
}

//--------------------------------------------------------------
void FileManager::setPolygonSpeedMultiplier(float multiplier) {
  polygonSpeedMultiplier = multiplier;
}

//--------------------------------------------------------------
float FileManager::getPolygonSpeedMultiplier() const {
  return polygonSpeedMultiplier;
}

//--------------------------------------------------------------
void FileManager::applySpecialPolygonAnimations(
    std::vector<PolygonShape> &polys, const std::vector<int> &specialAnims) {
  // Hanya proses sebanyak min(polys.size(), specialAnims.size())
  size_t count = std::min(polys.size(), specialAnims.size());

  for (size_t i = 0; i < count; ++i) {
    int animMode = specialAnims[i];

    if (animMode == 0) { // 0 = No Animation
      polys[i].setSpecialAnimation(nullptr);
    } else if (animMode == 1) { // 1 = Rotate Left
      // Base amplitudo: 90 derajat. Speed multiplier mengatur kecepatan osilasi.
      auto rotateAnim = std::make_unique<RotateLeftAnimation>(90.0f);
      rotateAnim->setSpeedMultiplier(polygonSpeedMultiplier);
      polys[i].setSpecialAnimation(std::move(rotateAnim));
    }
  }
}
