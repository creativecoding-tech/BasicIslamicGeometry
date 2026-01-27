#pragma once

#include "SacredGeometryTemplate.h"
#include <vector>
#include <memory>
#include <string>

/**
 * TemplateRegistry - Singleton untuk manage semua available templates
 *
 * Setiap template harus register ke registry di awal aplikasi.
 * UI bisa query registry untuk get semua available templates.
 *
 * Usage:
 *   // Register template di setup()
 *   TemplateRegistry::getInstance().registerTemplate(std::make_unique<ZelligeTemplate>());
 *
 *   // Get semua templates untuk display di UI
 *   auto templates = TemplateRegistry::getInstance().getAllTemplates();
 *
 *   // Get template tertentu
 *   auto* tmpl = TemplateRegistry::getInstance().getTemplate("Zellige");
 */
class TemplateRegistry {
public:
	// Get singleton instance
	static TemplateRegistry& getInstance();

	// Register template ke registry
	void registerTemplate(std::unique_ptr<SacredGeometryTemplate> tmpl);

	// Get semua registered templates
	std::vector<SacredGeometryTemplate*> getAllTemplates();

	// Get template by name
	SacredGeometryTemplate* getTemplate(const std::string& name);

	// Cek apakah template dengan nama tertentu exists
	bool hasTemplate(const std::string& name);

	// Get jumlah template yang terdaftar
	size_t getTemplateCount() const;

private:
	TemplateRegistry() = default;
	~TemplateRegistry() = default;

	// Prevent copy
	TemplateRegistry(const TemplateRegistry&) = delete;
	TemplateRegistry& operator=(const TemplateRegistry&) = delete;

	std::vector<std::unique_ptr<SacredGeometryTemplate>> templates;
};
