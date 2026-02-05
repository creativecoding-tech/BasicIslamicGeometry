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
    void update() override;
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

private:
    glm::vec2 position;      // Position dot
    float radius;           // Radius dot (ukuran dot)
    glm::vec2 lowerBound;    // Batas bawah (tidak boleh melewati ini)
    std::string label;       // Label untuk dot
};
