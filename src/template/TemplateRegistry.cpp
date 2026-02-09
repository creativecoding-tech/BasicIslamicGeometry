#include "TemplateRegistry.h"

//--------------------------------------------------------------
TemplateRegistry& TemplateRegistry::getInstance() {
	static TemplateRegistry instance;
	return instance;
}

//--------------------------------------------------------------
void TemplateRegistry::registerTemplate(std::unique_ptr<SacredGeometryTemplate> tmpl) {
	templates.push_back(std::move(tmpl));
}

//--------------------------------------------------------------
std::vector<SacredGeometryTemplate*> TemplateRegistry::getAllTemplates() {
	std::vector<SacredGeometryTemplate*> result;
	for (auto& tmpl : templates) {
		result.push_back(tmpl.get());
	}
	return result;
}

//--------------------------------------------------------------
SacredGeometryTemplate* TemplateRegistry::getTemplate(const std::string& name) {
	for (auto& tmpl : templates) {
		if (tmpl->getName() == name) {
			return tmpl.get();
		}
	}
	return nullptr;
}

//--------------------------------------------------------------
bool TemplateRegistry::hasTemplate(const std::string& name) {
	return getTemplate(name) != nullptr;
}

//--------------------------------------------------------------
size_t TemplateRegistry::getTemplateCount() const {
	return templates.size();
}
