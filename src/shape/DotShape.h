#pragma once

#include "AbstractShape.h"
#include <string>

/**
 * DotShape - Shape khusus untuk menampilkan single dot
 * Digunakan untuk fitur "Duplicate Dot Above" - membuat dot terpisah dari template
 */
class DotShape : public AbstractShape {
public:
    DotShape(glm::vec2 position, std::string label = "", float radius = 6.0f);
    virtual ~DotShape() = default;

    // Override pure virtual methods dari AbstractShape
    void update(float deltaTime = 0.016f) override;
    void draw() override;
    void addDotsToCache(std::vector<DotInfo>& dots) override;

    // Setter untuk position
    void setPosition(glm::vec2 pos) { position = pos; }
    glm::vec2 getPosition() const { return position; }

    // Setter untuk lower bound (batas bawah)
    void setLowerBound(glm::vec2 bound) { lowerBound = bound; }
    glm::vec2 getLowerBound() const { return lowerBound; }

    // Setter untuk radius
    void setRadius(float r) { radius = r; }
    float getRadius() const { return radius; }

    // Setter untuk color
    void setColor(ofColor c) { color = c; }
    ofColor getColor() const { return color; }

    // Setter untuk track mode (Duplicate Dot Track dari customLine)
    void setTrackLineIndex(int index) { trackLineIndex = index; }
    int getTrackLineIndex() const { return trackLineIndex; }
    bool hasTrackMode() const { return trackLineIndex >= 0; }

    // Method untuk set titik asli dari luar ⭐ NEW
    void saveOriginalPosition(float currentTemplateRadius);
    // Method untuk scale geometri berdasar ratio absolute dari original ⭐ NEW
    void scaleToRadius(float newRadius);

private:
    glm::vec2 originalPosition;   // Backup dari posisi asli ⭐ NEW
    glm::vec2 originalLowerBound; // Backup dari lowerBound asli ⭐ NEW
    float baseRadius =
        1.0f; // Radius template saat posisi asli dibuat/diload ⭐ NEW

    glm::vec2 position;      // Position dot
    float radius;           // Radius dot (ukuran dot)
    ofColor color = ofColor(0, 0, 255);  // Warna dot (default biru)
    glm::vec2 lowerBound;    // Batas bawah (tidak boleh melewati ini)
    std::string label;       // Label untuk dot
    int trackLineIndex = -1;   // Index customLine yang di-track (-1 = tidak track)
};
