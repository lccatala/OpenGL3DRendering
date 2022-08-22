

#ifndef _SCENE_FILE_H
#define _SCENE_FILE_H 2011

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>

#include "fileFormats.h"

struct aiScene;
struct aiMaterial;

namespace PGUPV {
  class Material;
  class PBRMaterial;
  class UBOMaterial;
  class BindableTexture;
  class Scene;
  class Node;
  class Mesh;
  class Skeleton;

  class AssimpWrapper {
  public:
    AssimpWrapper();
    /**
      Opciones de postprocesado de una escena:
      NONE: no se aplica ningún postprocesado a la escena
      FAST: triangula la escena, genera normales y tangentes...
      MEDIUM: igual que el anterior, pero intentando reducir memoria
          reutilizando materiales
      HIGHEST_QUALITY: igual que el anterior, pero buscando instancias
    */
    enum class LoadOptions {
      NONE, FAST, MEDIUM, HIGHEST_QUALITY
    };

    /**
    Carga una escena desde el fichero indicado
    \param filename ruta del fichero a cargar
    \param options Postprocesado a realizar sobre la escena (por defecto,
      LoadOptions::MEDIUM)
    */
    std::shared_ptr<Scene> load(const std::string &filename, LoadOptions options = LoadOptions::MEDIUM);

	std::vector<ExportFileFormat> listSupportedExportFormat();

	/**
	Carga una escena desde el fichero indicado
	\param filename ruta del fichero a cargar
	\param options Postprocesado a realizar sobre la escena (por defecto,
	  LoadOptions::MEDIUM)
	*/
	bool save(const std::string &path, const std::string &id, std::shared_ptr<Scene> scene);

  private:
    void loadMaterials();
	void loadMeshes();
	void saveMeshes(aiScene *assScene, Scene &scene);
	void loadAnimations();
    void loadTextures(const aiMaterial *aimat, Material &pgmat);
    void loadPBRTextures(const aiMaterial *aimat, PBRMaterial &pgmat);

	Assimp::Exporter &getExporter();
	std::shared_ptr<Skeleton> buildSkeleton(const struct aiMesh *mesh);

    std::shared_ptr<Node> recursive_load(const struct aiNode* nd);
    const aiScene* scene;
    std::shared_ptr<Scene> result;
    std::string _filename;
    static Assimp::Importer importer;
	std::unique_ptr<Assimp::Exporter> exporter;
    std::vector<std::shared_ptr<Mesh>> tempMeshes;
  };
};

#endif
