#pragma once

#include "../SacredGeometryTemplate.h"

/**
 * BasicZelligeTemplate - Moroccan Islamic geometric pattern (Basic version)
 *
 * Pattern ini terdiri dari:
 * - 5 Circles (A, B, C, D, E) - 1 center + 4 cardinal directions
 * - Cartesian Axes - X dan Y axis
 * - 4 Cross Lines (F, G, H, I) - Diagonal lines
 * - 4 Parallelogram Lines - Connecting outer circles
 * - 4 Rectangle Lines - Intersecting diagonal lines
 * - 8 Octagram Lines - 8-pointed star pattern
 *
 * Total: 26 shapes dengan kompleksitas geometri tinggi
 *
 * Note: Ini adalah "Basic" version. Nanti bisa ada ZelligeTemplate (advanced)
 *       atau variations lainnya.
 */
class BasicZelligeTemplate : public SacredGeometryTemplate {
public:
	std::string getName() override;
	std::string getDescription() override;

	void setupShapes(std::vector<std::unique_ptr<AbstractShape>>& shapes) override;

	// radius diambil dari SacredGeometryTemplate::radius (public)
	// Tidak perlu radius sendiri lagi

private:
	void setupCircles(std::vector<std::unique_ptr<AbstractShape>>& shapes);
	void setupCartesianAxes(std::vector<std::unique_ptr<AbstractShape>>& shapes);
	void setupCrossLines(std::vector<std::unique_ptr<AbstractShape>>& shapes);
	void setupParallelograms(std::vector<std::unique_ptr<AbstractShape>>& shapes);
	void setupRectangleLines(std::vector<std::unique_ptr<AbstractShape>>& shapes);
	void setupOctagramLines(std::vector<std::unique_ptr<AbstractShape>>& shapes);
};
