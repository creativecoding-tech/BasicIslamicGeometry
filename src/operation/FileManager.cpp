#include "FileManager.h"
#include "../anim/FadeInAnimation.h"

// Konstanta filename
const std::string FileManager::FILENAME = "custom_lines.bin";
const std::string FileManager::FILENAME_POLYGONS = "polygons.bin";

//--------------------------------------------------------------
FileManager::FileManager()
    : loadSequentialMode(false)
    , loadParallelMode(false)
    , currentLoadIndex(0)
    , loadSpeed(0.05f)
    , loadAccumulator(0.0f) {
}

//--------------------------------------------------------------
void FileManager::saveCustomLines(const std::vector<CustomLine>& customLines) {
    // Cek apakah ada lines untuk disimpan
    if (customLines.empty()) {
        return;  // Tidak create file jika kosong
    }

    ofBuffer buffer;

    // Write jumlah line dulu
    int numLines = static_cast<int>(customLines.size());
    buffer.append(reinterpret_cast<const char*>(&numLines), sizeof(int));

    // Write setiap line dalam binary format
    for (const auto& line : customLines) {
        // Write jumlah points dalam line ini
        const vector<vec2>& points = line.getPoints();
        int numPoints = static_cast<int>(points.size());
        buffer.append(reinterpret_cast<const char*>(&numPoints), sizeof(int));

        // Write semua points
        for (const auto& point : points) {
            buffer.append(reinterpret_cast<const char*>(&point), sizeof(vec2));
        }

        // Write color, lineWidth, dan curve
        ofColor color = line.getColor();
        float lineWidth = line.getLineWidth();
        float curve = line.getCurve();
        buffer.append(reinterpret_cast<const char*>(&color), sizeof(ofColor));
        buffer.append(reinterpret_cast<const char*>(&lineWidth), sizeof(float));
        buffer.append(reinterpret_cast<const char*>(&curve), sizeof(float));
    }

    // Write buffer ke file (selalu replace/overwrite)
    ofBufferToFile(FILENAME, buffer);
}

//--------------------------------------------------------------
bool FileManager::loadCustomLines(std::vector<CustomLine>& customLines) {
    // Cek apakah ada isinya dulu (proteksi kerjaan yang sudah dibuat)
    if (!customLines.empty()) {
        return false;  // Tidak load jika sudah ada isinya
    }

    // Cek apakah file exists
    ofFile file(FILENAME);
    if (!file.exists()) {
        return false;
    }

    // Read file ke buffer
    ofBuffer buffer = ofBufferFromFile(FILENAME);
    char* data = buffer.getData();

    // Read jumlah line
    int numLines = *reinterpret_cast<int*>(data);
    data += sizeof(int);

    // Clear existing lines
    customLines.clear();

    // Read setiap line
    for (int i = 0; i < numLines; i++) {
        CustomLine line;

        // Read jumlah points
        int numPoints = *reinterpret_cast<int*>(data);
        data += sizeof(int);

        // Read semua points
        vector<vec2> points;
        for (int j = 0; j < numPoints; j++) {
            vec2 point = *reinterpret_cast<vec2*>(data);
            data += sizeof(vec2);
            points.push_back(point);
        }
        line.setPoints(points);

        // Read color
        ofColor color = *reinterpret_cast<ofColor*>(data);
        data += sizeof(ofColor);
        line.setColor(color);

        // Read lineWidth
        float lineWidth = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setLineWidth(lineWidth);

        // Read curve
        float curve = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setCurve(curve);

        // Set progress ke 0.0 untuk animasi parallel
        line.setProgress(0.0f);
        line.setSpeed(0.005f);  // Kecepatan animasi

        // Add ke vector
        customLines.push_back(line);
    }

    // Enable parallel mode untuk animate semua lines barengan
    loadParallelMode = true;

    return true;
}

//--------------------------------------------------------------
void FileManager::loadCustomLinesSequential(std::vector<CustomLine>& customLines) {
    // Cek apakah ada isinya dulu (proteksi kerjaan yang sudah dibuat)
    if (!customLines.empty()) {
        return;
    }

    // Cek apakah file exists
    ofFile file(FILENAME);
    if (!file.exists()) {
        return;
    }

    // Baca file ke buffer
    ofBuffer buffer = ofBufferFromFile(FILENAME);
    char* data = buffer.getData();

    // Read jumlah line
    int numLines = *reinterpret_cast<int*>(data);
    data += sizeof(int);

    // Simpan SEMUA lines ke loadedLinesBuffer
    loadedLinesBuffer.clear();

    for (int i = 0; i < numLines; i++) {
        CustomLine line;

        // Read jumlah points
        int numPoints = *reinterpret_cast<int*>(data);
        data += sizeof(int);

        // Read semua points
        vector<vec2> points;
        for (int j = 0; j < numPoints; j++) {
            vec2 point = *reinterpret_cast<vec2*>(data);
            data += sizeof(vec2);
            points.push_back(point);
        }
        line.setPoints(points);

        // Read color
        ofColor color = *reinterpret_cast<ofColor*>(data);
        data += sizeof(ofColor);
        line.setColor(color);

        // Read lineWidth
        float lineWidth = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setLineWidth(lineWidth);

        // Read curve
        float curve = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setCurve(curve);

        // Set initial animation state
        line.setProgress(0.0f);  // Mulai dari 0 (belum tergambar)
        line.setSpeed(0.02f);     // Kecepatan animasi

        // Add ke buffer (BUKAN ke customLines!)
        loadedLinesBuffer.push_back(line);
    }

    // Reset index dan mulai mode sequential
    currentLoadIndex = 0;
    loadAccumulator = 0.0f;
    loadSequentialMode = true;
}

//--------------------------------------------------------------
void FileManager::clearCustomLines(std::vector<CustomLine>& customLines) {
    customLines.clear();
}

//--------------------------------------------------------------
void FileManager::updateSequentialLoad(std::vector<CustomLine>& customLines) {
    // PARALLEL MODE: Update semua lines barengan
    if (loadParallelMode) {
        bool allComplete = true;

        // Update progress semua lines
        for (auto& line : customLines) {
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

    // SEQUENTIAL MODE: Update satu per satu (original logic)
    if (!loadSequentialMode) {
        return;
    }

    // Add line baru jika line terakhir sudah complete
    bool canAddNewLine = false;
    if (customLines.empty()) {
        canAddNewLine = true;
    } else {
        // Cek apakah line terakhir sudah complete
        CustomLine& lastLine = customLines.back();
        if (lastLine.getProgress() >= 1.0f && currentLoadIndex < static_cast<int>(loadedLinesBuffer.size())) {
            canAddNewLine = true;
        }
    }

    // Add line baru dengan delay
    if (canAddNewLine) {
        loadAccumulator += loadSpeed;
        if (loadAccumulator >= 1.0f && currentLoadIndex < static_cast<int>(loadedLinesBuffer.size())) {
            customLines.push_back(loadedLinesBuffer[currentLoadIndex]);
            currentLoadIndex++;
            loadAccumulator = 0.0f;
        }
    }

    // Update progress untuk semua lines yang belum complete
    for (auto& line : customLines) {
        line.updateProgress();
    }

    // Cek apakah sudah selesai (semua lines loaded dan semua complete)
    bool allComplete = true;
    if (currentLoadIndex < static_cast<int>(loadedLinesBuffer.size())) {
        allComplete = false;  // Masih ada lines yang belum di-load
    } else {
        // Cek apakah semua lines sudah complete
        for (const auto& line : customLines) {
            if (line.getProgress() < 1.0f) {
                allComplete = false;
                break;
            }
        }
    }

    if (allComplete) {
        loadSequentialMode = false;  // Selesai
        loadedLinesBuffer.clear();     // Bersihkan buffer
        loadAccumulator = 0.0f;        // Reset accumulator
    }
}

//--------------------------------------------------------------
bool FileManager::isLoadSequentialMode() const {
    return loadSequentialMode;
}

//--------------------------------------------------------------
bool FileManager::isLoadParallelMode() const {
    return loadParallelMode;
}

//--------------------------------------------------------------
int FileManager::getCurrentLoadIndex() const {
    return currentLoadIndex;
}

//--------------------------------------------------------------
int FileManager::getTotalLoadedLines() const {
    return static_cast<int>(loadedLinesBuffer.size());
}

//--------------------------------------------------------------
void FileManager::setLoadSpeed(float speed) {
    loadSpeed = speed;
}

//--------------------------------------------------------------
float FileManager::getLoadSpeed() const {
    return loadSpeed;
}

//--------------------------------------------------------------
void FileManager::saveAll(const std::vector<CustomLine>& customLines, const std::vector<PolygonShape>& polygons) {
    // Save customLines
    saveCustomLines(customLines);

    // Save polygons
    if (polygons.empty()) {
        return;  // Tidak create file jika kosong
    }

    ofBuffer buffer;

    // Write jumlah polygon dulu
    int numPolygons = static_cast<int>(polygons.size());
    buffer.append(reinterpret_cast<const char*>(&numPolygons), sizeof(int));

    // Write setiap polygon dalam binary format
    for (const auto& polygon : polygons) {
        // Write jumlah vertices dalam polygon ini
        const vector<vec2>& vertices = polygon.getVertices();
        int numVertices = static_cast<int>(vertices.size());
        buffer.append(reinterpret_cast<const char*>(&numVertices), sizeof(int));

        // Write semua vertices
        for (const auto& vertex : vertices) {
            buffer.append(reinterpret_cast<const char*>(&vertex), sizeof(vec2));
        }

        // Write fillColor
        ofColor fillColor = polygon.getColor();
        buffer.append(reinterpret_cast<const char*>(&fillColor), sizeof(ofColor));
    }

    // Write buffer ke file
    ofBufferToFile(FILENAME_POLYGONS, buffer);
}

//--------------------------------------------------------------
bool FileManager::loadAll(std::vector<CustomLine>& customLines, std::vector<PolygonShape>& polygons) {
    // Load customLines
    bool linesLoaded = loadCustomLines(customLines);

    // Load polygons
    // Cek apakah ada isinya dulu (proteksi kerjaan yang sudah dibuat)
    if (!polygons.empty()) {
        return linesLoaded;  // Tidak load jika sudah ada isinya, tapi tetap return status loadLines
    }

    // Cek apakah file exists
    ofFile file(FILENAME_POLYGONS);
    if (!file.exists()) {
        return linesLoaded;  // File tidak ada, tapi tetap return status loadLines
    }

    // Read file ke buffer
    ofBuffer buffer = ofBufferFromFile(FILENAME_POLYGONS);
    char* data = buffer.getData();

    // Read jumlah polygon
    int numPolygons = *reinterpret_cast<int*>(data);
    data += sizeof(int);

    // Clear existing polygons
    polygons.clear();

    // Read setiap polygon
    for (int i = 0; i < numPolygons; i++) {
        // Read jumlah vertices
        int numVertices = *reinterpret_cast<int*>(data);
        data += sizeof(int);

        // Read semua vertices
        vector<vec2> vertices;
        for (int j = 0; j < numVertices; j++) {
            vec2 vertex = *reinterpret_cast<vec2*>(data);
            data += sizeof(vec2);
            vertices.push_back(vertex);
        }

        // Read fillColor
        ofColor fillColor = *reinterpret_cast<ofColor*>(data);
        data += sizeof(ofColor);

        // Buat PolygonShape dengan index dan langsung add ke vector
        polygons.emplace_back(vertices, fillColor, i);

        // Set animation ke polygon yang sudah ada di vector (reference)
        auto fadeIn = std::make_unique<FadeInAnimation>(fillColor.a, 0.003f);
        polygons.back().setAnimation(std::move(fadeIn));
    }

    return true;  // Success
}
