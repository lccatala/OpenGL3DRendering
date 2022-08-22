#pragma once
#include <string>
#include <memory>

#include "assimpWrapper.h"

namespace PGUPV {
	class Scene;

	class FileLoader {
	public:
		static std::shared_ptr<Scene> load(const std::string &path, AssimpWrapper::LoadOptions options = AssimpWrapper::LoadOptions::MEDIUM);
		/**
			\return la lista de formatos de fichero soportados para escritura
		*/
		static std::vector<ExportFileFormat> getSupportedExportFileFormats();

		static bool save(const std::string &path, const std::string &id, std::shared_ptr<Scene> scene);
	};
};
