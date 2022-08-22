
#include <fstream>
#include <bitset>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Exporter.hpp>

#include <sstream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assimpWrapper.h"
#include "uboBones.h"
#include "textureGenerator.h"
#include "transform.h"
#include "utils.h"
#include "drawCommand.h"
#include "geode.h"
#include "material.h"
#include "scene.h"
#include "animationClip.h"
#include "animationChannel.h"
#include "bone.h"
#include "skeleton.h"
#include "material.h"
#include "pbrMaterial.h"

using PGUPV::AssimpWrapper;
using PGUPV::Node;
using PGUPV::Texture2D;
using PGUPV::Material;
using PGUPV::PBRMaterial;
using PGUPV::Mesh;
using PGUPV::Group;
using PGUPV::Geode;
using PGUPV::Transform;
using PGUPV::Scene;
using PGUPV::AnimationClip;
using PGUPV::AnimationChannel;
using PGUPV::Skeleton;
using PGUPV::Bone;

using std::string;
using std::endl;


Assimp::Importer AssimpWrapper::importer;

static std::string printMaterialInfo(const aiMaterial* mat);
static std::string printMeshInfo(const aiScene* scene, size_t n);
static std::string printMetadataInfo(const aiNode* nd);

AssimpWrapper::AssimpWrapper() :
	scene(nullptr) {
}

std::shared_ptr<Scene> AssimpWrapper::load(const string& filename, LoadOptions options) {
	result = std::make_shared<Scene>();

	unsigned int flags = aiProcess_TransformUVCoords;
	switch (options) {
	case LoadOptions::FAST:
		flags |= aiProcessPreset_TargetRealtime_Fast;
		break;
	case LoadOptions::MEDIUM:
		flags |= aiProcessPreset_TargetRealtime_Quality;
		break;
	case LoadOptions::HIGHEST_QUALITY:
		flags |= aiProcessPreset_TargetRealtime_MaxQuality;
		break;
	case LoadOptions::NONE:
		break;
	}

	importer.SetPropertyBool(AI_CONFIG_IMPORT_MD5_NO_ANIM_AUTOLOAD, true);

	scene = importer.ReadFile(filename, flags);
	if (!scene) {
		ERRT(importer.GetErrorString());
	}

	_filename = filename;

	if (scene->HasTextures()) {
		ERRT("El modelo " + filename + " tiene las texturas embebidas. Por el momento no "
			"podemos trabajar con este tipo de ficheros");
	}

	if (!scene->HasMaterials()) {
		WARN("El modelo " + filename + " no tiene materiales. El modelo podría no verse correctamente");
	}

	if (!scene->HasAnimations()) {
		INFO("El modelo " + filename + " no tiene animaciones");
	}

	loadMaterials();
	loadMeshes();
	loadAnimations();

	result->setRoot(recursive_load(scene->mRootNode));
	tempMeshes.clear();
	return result;
}

Assimp::Exporter& AssimpWrapper::getExporter() {
	if (!exporter) {
		exporter = std::make_unique<Assimp::Exporter>();
	}
	return *exporter;
}

std::vector<PGUPV::ExportFileFormat> AssimpWrapper::listSupportedExportFormat()
{
	std::vector<PGUPV::ExportFileFormat> formats;
	auto count = getExporter().GetExportFormatCount();

	for (size_t i = 0; i < count; i++) {
		auto format = getExporter().GetExportFormatDescription(i);
		PGUPV::ExportFileFormat eff;
		eff.id = format->id;
		eff.extension = format->fileExtension;
		eff.description = format->description;
		formats.push_back(eff);
	}

	return formats;
}

std::shared_ptr<Node> AssimpWrapper::recursive_load(const struct aiNode* nd) {
	if (nd == nullptr) return nullptr;

	std::shared_ptr<Geode> geode;
	if (nd->mNumMeshes > 0) {
		geode = Geode::build();
		geode->setName(nd->mName.C_Str());
		// draw all meshes assigned to this node
		for (unsigned int n = 0; n < nd->mNumMeshes; ++n)
			geode->addMesh(tempMeshes[nd->mMeshes[n]]);
	}

	if (nd->mMetaData) {
		INFO(printMetadataInfo(nd));
	}

	std::shared_ptr<Group> root;

	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;

	// OpenGL matrices are column major
	glm::mat4 localM = glm::transpose(glm::make_mat4(&m.a1));
	if (PGUPV::isIdentity(localM)) {
		// No transformation
		if (nd->mNumChildren == 0) {
			return geode; // this can't be null here
		}
		root = Group::build();
	}
	else {
		// There is a transformation
		auto xform = Transform::build(localM);
		root = xform;
	}
	if (geode)
		root->addChild(geode);
	// add all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		auto c = recursive_load(nd->mChildren[n]);
		if (c)
			root->addChild(c);
	}
	root->setName(nd->mName.C_Str());

	return root;
}

#define P(a) {a, #a}
std::map<aiTextureType, std::string> aiTextureTypes{
  P(aiTextureType_NONE), P(aiTextureType_DIFFUSE), P(aiTextureType_SPECULAR), P(aiTextureType_AMBIENT), P(aiTextureType_EMISSIVE),
  P(aiTextureType_HEIGHT), P(aiTextureType_NORMALS), P(aiTextureType_SHININESS), P(aiTextureType_OPACITY), P(aiTextureType_DISPLACEMENT),
  P(aiTextureType_LIGHTMAP), P(aiTextureType_REFLECTION), P(aiTextureType_UNKNOWN)
};
#undef P

void AssimpWrapper::loadMeshes() {
	for (unsigned int n = 0; n < scene->mNumMeshes; ++n) {
		auto mymesh = std::make_shared<Mesh>();

		const struct aiMesh* mesh = scene->mMeshes[n];

		mymesh->setName(mesh->mName.C_Str());

		INFO(printMeshInfo(scene, n));

		size_t numVertPerFace;

		switch (mesh->mPrimitiveTypes) {
		case aiPrimitiveType_TRIANGLE:
			numVertPerFace = 3;
			mymesh->addDrawCommand(
				new DrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->mNumFaces * numVertPerFace), GL_UNSIGNED_INT, 0));
			break;
		case aiPrimitiveType_LINE:
			numVertPerFace = 2;
			mymesh->addDrawCommand(
				new DrawElements(GL_LINES, static_cast<GLsizei>(mesh->mNumFaces * numVertPerFace), GL_UNSIGNED_INT, 0));
			break;
		case aiPrimitiveType_POINT:
			numVertPerFace = 1;
			mymesh->addDrawCommand(new DrawArrays(GL_POINTS, 0, mesh->mNumVertices));
			break;
		default:
			ERRT("Tipo de primitiva no soportada (" + std::to_string(mesh->mPrimitiveTypes) + ")");
		}

		if (numVertPerFace > 1) {
			// create array with indices
			// have to convert from Assimp format to array
			std::vector<unsigned int> faceArray(mesh->mNumFaces * numVertPerFace);
			size_t faceIndex = 0;
			for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
				const struct aiFace* face = &mesh->mFaces[t];
				memcpy(&faceArray[faceIndex], face->mIndices, numVertPerFace * sizeof(unsigned int));
				faceIndex += numVertPerFace;
			}
			mymesh->addIndices(faceArray);
		}
		// buffer for vertex positions
		if (mesh->HasPositions())
			mymesh->addVertices((GLfloat*)mesh->mVertices, 3, mesh->mNumVertices);

		// buffer for vertex normals
		if (mesh->HasNormals())
			mymesh->addNormals((GLfloat*)mesh->mNormals, mesh->mNumVertices);

		if (mesh->HasTangentsAndBitangents()) {
			mymesh->addTangents((GLfloat*)mesh->mTangents, mesh->mNumVertices);
		}

		if (mesh->HasBones()) {
			INFO("La malla " + std::to_string(n) + " tiene " + std::to_string(mesh->mNumBones) + " huesos");
			auto skeleton = buildSkeleton(mesh);
			mymesh->setSkeleton(skeleton);
		}

		// buffer for vertex texture coordinates
		for (unsigned int idx = 0; idx < NUM_TEX_COORD; ++idx) {
			if (mesh->HasTextureCoords(idx)) {
				std::vector<glm::vec2> texCoords(mesh->mNumVertices);
				for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
					texCoords[k].s = mesh->mTextureCoords[0][k].x;
					texCoords[k].t = mesh->mTextureCoords[0][k].y;
				}
				mymesh->addTexCoord(idx, texCoords);
			}
			else
				break;
		}

		mymesh->setMaterial(result->getMaterial(mesh->mMaterialIndex));
		tempMeshes.push_back(mymesh);
	}
}

std::shared_ptr<Skeleton> AssimpWrapper::buildSkeleton(const struct aiMesh* mesh)
{
	auto skeleton = std::make_shared<Skeleton>();

	for (unsigned int bone = 0; bone < mesh->mNumBones; bone++) {
		auto boneObject = std::make_shared<Bone>(mesh->mBones[bone]->mName.C_Str());
		glm::mat4 m = glm::transpose(glm::make_mat4(&mesh->mBones[bone]->mOffsetMatrix.a1));
		boneObject->setMatrix(m);

		for (unsigned int j = 0; j < mesh->mBones[bone]->mNumWeights; j++) {
			aiVertexWeight& vw = mesh->mBones[bone]->mWeights[j];
			boneObject->addWeight(vw.mVertexId, vw.mWeight);
		}
		skeleton->addBone(boneObject);
	}
	return skeleton;

}


/**
Dada la ruta de una textura, intenta buscar el fichero, en el siguiente orden:
-# En la ruta indicada por filename
-# En el directorio actual (ignorando la ruta proporcionada)
-# Añadiendo como prefijo el directorio donde se encontraba el modelo
-# En el directorio donde estaba el modelo

\param filename La ruta por donde empezar la búsqueda
\return La ruta completa de un fichero que se puede abrir, o filename, si no existe
*/
std::string findTexture(const std::string& filename, const std::string& modelfilename) {
	// First, try to load the texture filename as given by the model file
	std::ifstream f(filename.c_str());
	if (f) {
		f.close();
		return filename;
	}

	// If it does not work, then try removing the directory and use only the filename
	// it will work only if the texture file is in the current working directory
	std::string name = PGUPV::getFilenameFromPath(filename);
	f.open(name);
	if (f) {
		f.close();
		return name;
	}

	// If it does not work, try to prepend the directory where the model file is located 
	// to the original texture filename (it may have relative directories like ../textures/pp.jpg"
	name = PGUPV::getDirectory(modelfilename) + filename;
	f.open(name);
	if (f) {
		f.close();
		return name;
	}

	// Next, try the directory of the model file with the filename of the texture (just the filename)
	name = PGUPV::getDirectory(modelfilename) + PGUPV::getFilenameFromPath(filename);
	f.open(name);
	if (f) {
		f.close();
		return name;
	}

	// Finally, search in all the subdirectories inside the directory that contains the model
	auto res = PGUPV::listFiles(PGUPV::getDirectory(modelfilename), true, std::vector<std::string> {PGUPV::getFilenameFromPath(filename)});
	if (res.size() == 1) {
		return res[0];
	}

	return filename;
}

uint acceptTexture(const std::string& modelFileName, aiTextureType type, const uint textureUnitBase, const aiMaterial* mtl, PGUPV::Material& mat) {
	aiString path;
	unsigned int c = MIN(mtl->GetTextureCount(type), 4U); // Soportamos hasta 4 texturas de cada tipo
	for (uint i = 0; i < c; i++) {
		mtl->GetTexture(type, i, &path);
		// This function will throw if we can't find the texture
		try {
			auto t = std::make_shared<Texture2D>(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			t->loadImage(findTexture(path.C_Str(), modelFileName));
			t->generateMipmap();
			mat.setTexture(textureUnitBase + i, t);
		}
		catch (std::runtime_error&) {
			// Could not load the texture: show a flashy checkboard
			mat.setTexture(textureUnitBase + i, std::shared_ptr<Texture2D>(PGUPV::TextureGenerator::makeChecker(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))));
		}
	}
	return c;
}

uint acceptTexture(const std::string& modelFileName, aiTextureType type, const uint textureUnitBase, const aiMaterial* mtl, PGUPV::PBRMaterial& mat) {
	aiString path;
	unsigned int c = MIN(mtl->GetTextureCount(type), 4U); // Soportamos hasta 4 texturas de cada tipo
	for (uint i = 0; i < c; i++) {
		mtl->GetTexture(type, i, &path);
		// This function will throw if we can't find the texture
		try {
			auto t = std::make_shared<Texture2D>(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			t->loadImage(findTexture(path.C_Str(), modelFileName));
			t->generateMipmap();
			mat.setTexture(textureUnitBase + i, t);
		}
		catch (std::runtime_error&) {
			// Could not load the texture: show a flashy checkboard
			mat.setTexture(textureUnitBase + i, std::shared_ptr<Texture2D>(PGUPV::TextureGenerator::makeChecker(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))));
		}
	}
	return c;
}

void rejectTexture(aiTextureType type, const aiMaterial* mtl) {
	aiString path;
	int texIndex = 0;
	for (uint i = 0; i < mtl->GetTextureCount(type); i++) {
		mtl->GetTexture(type, texIndex, &path);
		WARN(std::string("Textura de tipo " + aiTextureTypes[type] + " ignorada: " + path.C_Str()));
	}
}

void AssimpWrapper::loadTextures(const aiMaterial* aimat, Material& pgmat) {
	/*uint nDiff = */acceptTexture(_filename, aiTextureType_DIFFUSE, Material::DIFFUSE_TUNIT, aimat, pgmat);
	/*uint nSpec = */acceptTexture(_filename, aiTextureType_SPECULAR, Material::SPECULAR_TUNIT, aimat, pgmat);
	/*uint nNor = */acceptTexture(_filename, aiTextureType_NORMALS, Material::NORMALMAP_TUNIT, aimat, pgmat);
	/*uint nHei = */acceptTexture(_filename, aiTextureType_HEIGHT, Material::HEIGHTMAP_TUNIT, aimat, pgmat);
	/*uint nOpac = */acceptTexture(_filename, aiTextureType_OPACITY, Material::OPACITYMAP_TUNIT, aimat, pgmat);
	/*uint nAmb = */acceptTexture(_filename, aiTextureType_AMBIENT, Material::AMBIENT_TUNIT, aimat, pgmat);

	rejectTexture(aiTextureType_DISPLACEMENT, aimat);
	rejectTexture(aiTextureType_EMISSIVE, aimat);
	rejectTexture(aiTextureType_LIGHTMAP, aimat);
	rejectTexture(aiTextureType_NONE, aimat);
	rejectTexture(aiTextureType_REFLECTION, aimat);
	rejectTexture(aiTextureType_SHININESS, aimat);
	rejectTexture(aiTextureType_UNKNOWN, aimat);
}

void PGUPV::AssimpWrapper::loadPBRTextures(const aiMaterial* aimat, PBRMaterial& pgmat)
{
	/*uint nDiff = */acceptTexture(_filename, aiTextureType_BASE_COLOR, PBRMaterial::BASECOLOR_TUNIT, aimat, pgmat);
	/*uint nSpec = */acceptTexture(_filename, aiTextureType_NORMAL_CAMERA, PBRMaterial::NORMAL_TUNIT, aimat, pgmat);
	/*uint nNor = */acceptTexture(_filename, aiTextureType_EMISSION_COLOR, PBRMaterial::EMISSION_TUNIT, aimat, pgmat);
	/*uint nHei = */acceptTexture(_filename, aiTextureType_METALNESS, PBRMaterial::METALNESS_TUNIT, aimat, pgmat);
	/*uint nOpac = */acceptTexture(_filename, aiTextureType_DIFFUSE_ROUGHNESS, PBRMaterial::ROUGHNESS_TUNIT, aimat, pgmat);
	/*uint nAmb = */acceptTexture(_filename, aiTextureType_AMBIENT_OCCLUSION, PBRMaterial::AMBIENTOCLUSSION_TUNIT, aimat, pgmat);

	rejectTexture(aiTextureType_DIFFUSE, aimat);
	rejectTexture(aiTextureType_SPECULAR, aimat);
	rejectTexture(aiTextureType_NORMALS, aimat);
	rejectTexture(aiTextureType_HEIGHT, aimat);
	rejectTexture(aiTextureType_OPACITY, aimat);
	rejectTexture(aiTextureType_AMBIENT, aimat);
	rejectTexture(aiTextureType_DISPLACEMENT, aimat);
	rejectTexture(aiTextureType_EMISSIVE, aimat);
	rejectTexture(aiTextureType_LIGHTMAP, aimat);
	rejectTexture(aiTextureType_NONE, aimat);
	rejectTexture(aiTextureType_REFLECTION, aimat);
	rejectTexture(aiTextureType_SHININESS, aimat);
	rejectTexture(aiTextureType_UNKNOWN, aimat);
}

// Consideramos que el material es PBR cuando tiene alguna de las siguientes texturas
bool isPBR(const struct aiMaterial* mtl) {
	return
		(mtl->GetTextureCount(aiTextureType_BASE_COLOR) > 0) ||
		(mtl->GetTextureCount(aiTextureType_NORMAL_CAMERA) > 0) ||
		(mtl->GetTextureCount(aiTextureType_EMISSION_COLOR) > 0) ||
		(mtl->GetTextureCount(aiTextureType_METALNESS) > 0) ||
		(mtl->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) ||
		(mtl->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0);
}
std::shared_ptr<Material> loadRegularMaterial(const struct aiMaterial* mtl) {
	aiColor3D color;
	glm::vec4 mdif(0.0, 0.0, 0.0, 1.0),
		mamb(0.0, 0.0, 0.0, 1.0),
		mspe(0.0, 0.0, 0.0, 1.0),
		memi(0.0, 0.0, 0.0, 1.0);

	if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		mdif = glm::vec4(color.r, color.g, color.b, 1.0f);
	if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_AMBIENT, color))
		mamb = glm::vec4(color.r, color.g, color.b, 1.0f);
	if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_SPECULAR, color))
		mspe = glm::vec4(color.r, color.g, color.b, 1.0f);
	if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_EMISSIVE, color))
		memi = glm::vec4(color.r, color.g, color.b, 1.0f);

	float shininess = 0.0;
	mtl->Get(AI_MATKEY_SHININESS, shininess);

	aiString name;
	mtl->Get(AI_MATKEY_NAME, name);

	auto theMaterial = std::make_shared<Material>(name.C_Str(), mamb, mdif, mspe, memi, shininess);
	return theMaterial;
}


std::shared_ptr<PBRMaterial> loadPBRMaterial(const struct aiMaterial* mtl) {
	aiString name;
	mtl->Get(AI_MATKEY_NAME, name);

	auto theMaterial = std::make_shared<PBRMaterial>(name.C_Str());
	return theMaterial;
}


void AssimpWrapper::loadMaterials() {
	/* scan scene's materials for textures */
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		struct aiMaterial* mtl = scene->mMaterials[i];
		if (isPBR(mtl)) {
			auto mat = loadPBRMaterial(mtl);
			loadPBRTextures(mtl, *mat);
			result->addMaterial(mat);
			INFO(printMaterialInfo(mtl) + " TextureCount: " + std::bitset<32>(mat->getTextureCounters()).to_string());
		}
		else {
			auto mat = loadRegularMaterial(mtl);
			loadTextures(mtl, *mat);
			result->addMaterial(mat);
			INFO(printMaterialInfo(mtl) + " TextureCount: " + std::bitset<32>(mat->getTextureCounters()).to_string());
		}
	}
}

static std::string printMetadataInfo(const aiNode* nd) {
	std::ostringstream os;

	os << "METADATA IN " << nd->mName.C_Str() << ". " << nd->mMetaData->mNumProperties << " properties\n";
	for (unsigned int i = 0; i < nd->mMetaData->mNumProperties; i++) {
		const aiString* key;
		const aiMetadataEntry* data;
		nd->mMetaData->Get(i, key, data);
		os << "[" << i << "] " << key->C_Str() << " ";
		switch (data->mType) {
		case AI_BOOL:
			if (*static_cast<bool*>(data->mData)) os << "true";
			else os << "false";
			break;
		case AI_INT32:
			os << *static_cast<int32_t*>(data->mData);
			break;
		case AI_UINT64:
			os << *static_cast<uint64_t*>(data->mData);
			break;
		case AI_FLOAT:
			os << *static_cast<float*>(data->mData);
			break;
		case AI_DOUBLE:
			os << *static_cast<double*>(data->mData);
			break;
		case AI_AISTRING:
			os << "\"" << static_cast<aiString*>(data->mData)->C_Str() << "\"";
			break;
		case AI_AIVECTOR3D:
		{
			auto v3 = *static_cast<aiVector3D*>(data->mData);
			os << "{" << v3.x << ", " << v3.y << ", " << v3.z << "}";
			break;
		}
		default:
			os << "unkwown type";
		}
		os << "\n";
	}

	return os.str();
}

std::shared_ptr<AnimationChannel> fromAssimpToPG(const aiNodeAnim* channel) {
	std::shared_ptr<AnimationChannel> result = std::make_shared<AnimationChannel>(channel->mNodeName.C_Str());
	for (auto i = 0U; i < channel->mNumPositionKeys; i++) {
		auto pos = channel->mPositionKeys[i];
		result->addPositionKeyFrame({ static_cast<float>(pos.mTime),
			glm::vec3(pos.mValue.x, pos.mValue.y, pos.mValue.z) });
	}

	for (auto i = 0U; i < channel->mNumRotationKeys; i++) {
		auto rot = channel->mRotationKeys[i];
		result->addRotationKeyFrame({ static_cast<float>(rot.mTime),
			glm::quat(rot.mValue.w, rot.mValue.x, rot.mValue.y, rot.mValue.z) });
	}

	for (auto i = 0U; i < channel->mNumScalingKeys; i++) {
		auto sca = channel->mScalingKeys[i];
		result->addScalingKeyFrame({ static_cast<float>(sca.mTime),
			glm::vec3(sca.mValue.x, sca.mValue.y, sca.mValue.z) });
	}
	return result;
}



void fixAnimationTiming(AnimationClip& anim) {
	if (anim.getDurationInTicks() <= 0 || anim.getTicksPerSecond() <= 0) {
		auto acs = anim.getAnimationChannels();
		uint32_t maxNumKeyFrames = 0;
		for (const auto& ac : acs) {
			maxNumKeyFrames = std::max(ac->getNumFrames(), maxNumKeyFrames);
		}
		anim.setDurationInTicks(static_cast<float>(maxNumKeyFrames));
		anim.setTicksPerSecond(25);
	}
}

std::shared_ptr<AnimationClip> fromAssimpToPG(const std::string& name, aiAnimation* anim) {
	std::shared_ptr<AnimationClip> result = std::make_shared<AnimationClip>(name, static_cast<float>(anim->mDuration),
		static_cast<float>(anim->mTicksPerSecond));
	for (auto i = 0U; i < anim->mNumChannels; i++) {
		result->addChannel(fromAssimpToPG(anim->mChannels[i]));
	}

	if (anim->mNumMeshChannels) {
		WARN("El fichero tiene animaciones a nivel de malla (no soportadas). Habla con Paco");
	}

	if (anim->mNumMorphMeshChannels) {
		WARN("El fichero tiene animaciones de morphing de malla (no soportardas). Habla con Paco");
	}

	// Fix duration (if needed)
	fixAnimationTiming(*result);

	return result;
}


void AssimpWrapper::loadAnimations() {
	for (size_t i = 0; i < scene->mNumAnimations; i++) {
		auto* assimpAnim = scene->mAnimations[i];
		std::string assimpName{ assimpAnim->mName.C_Str() };
		if (assimpName.empty()) {
			assimpName = "animation#" + std::to_string(i);
		}
		result->addAnimation(fromAssimpToPG(assimpName, assimpAnim));
	}
}

static std::string printMeshInfo(const aiScene* scene, size_t n) {
	std::ostringstream os;
	const struct aiMesh* mesh = scene->mMeshes[n];

	os << "Mesh #" << std::to_string(n)
		<< " (" << mesh->mName.C_Str() << "). ";
	switch (mesh->mPrimitiveTypes) {
	case aiPrimitiveType_TRIANGLE:
		os << "Tris: ";
		break;
	case aiPrimitiveType_LINE:
		os << "Lines: ";
		break;
	case aiPrimitiveType_POINT:
		os << "Points: ";
		break;
	default:
		ERRT("Tipo de primitiva no soportada (" + std::to_string(mesh->mPrimitiveTypes) + ")");
	}

	os << std::to_string(mesh->mNumFaces)
		<< ". Vtx: " << std::to_string(mesh->mNumVertices);

	if (mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
		os << (mesh->HasNormals() ? " with normals" : " WITHOUT normals")
		<< (mesh->HasTangentsAndBitangents() ? " with tangents" : " WITHOUT tangents");

	return os.str();
}

static std::string printMaterialInfo(const aiMaterial* mat) {
	std::ostringstream oss;

	aiString matName;
	mat->Get(AI_MATKEY_NAME, matName);
	oss << "Material: " << matName.C_Str() << "\n";

	aiColor3D color;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color))
		oss << "Amb: " << PGUPV::to_string(glm::vec3(color.r, color.g, color.b)) << " ";
	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		oss << "Dif: " << PGUPV::to_string(glm::vec3(color.r, color.g, color.b)) << " ";
	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
		oss << "Esp: " << PGUPV::to_string(glm::vec3(color.r, color.g, color.b)) << " ";
	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, color))
		oss << "Emi: " << PGUPV::to_string(glm::vec3(color.r, color.g, color.b)) << " ";

	float shininess = 0.0;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, shininess))
		oss << "Shin: " << shininess;

	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_TRANSPARENT, color)) {
		oss << "\nTransparent color *ignored*: " << PGUPV::to_string(glm::vec3(color.r, color.g, color.b));
	}

	int sm;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_SHADING_MODEL, sm)) {
		oss << "\nShading model *ignored*: ";
		switch (sm) {
		case aiShadingMode_Flat:
			oss << "Flat shading";
			break;
		case aiShadingMode_Gouraud:
			oss << "Simple Gouraud shading";
			break;
		case aiShadingMode_Phong:
			oss << "Phong - Shading";
			break;
		case aiShadingMode_Blinn:
			oss << "Phong - Blinn - Shading";
			break;
		case aiShadingMode_Toon:
			oss << "Toon - Shading per pixel. Also known as 'comic' shader";
			break;
		case aiShadingMode_OrenNayar:
			oss << "OrenNayar - Shading per pixel. Extension to standard Lambertian shading, taking the roughness of the material into account";
			break;
		case aiShadingMode_Minnaert:
			oss << "Minnaert - Shading per pixel. Extension to standard Lambertian shading, taking the \"darkness\" of the material into account";
			break;
		case aiShadingMode_CookTorrance:
			oss << "CookTorrance - Shading per pixel. Special shader for metallic surfaces.";
			break;
		case aiShadingMode_NoShading:
			oss << "No shading at all. Constant light influence of 1.0.";
			break;
		case aiShadingMode_Fresnel:
			oss << "Fresnel shading";
			break;
		}
	}

	oss << "\nTextures:\n";
	for (auto tt : aiTextureTypes) {
		unsigned int c = mat->GetTextureCount(tt.first);
		aiString ss;
		if (c) {
			oss << tt.second << ": " << c << " (";
			for (uint t = 0; t < c; t++) {
				mat->GetTexture(tt.first, t, &ss);
				oss << ss.C_Str() << " ";
			}
			oss << ")\n";
		}
	}

	return oss.str();
}


#include "nodeVisitor.h"

class TranslateToAssimp : public PGUPV::NodeVisitor {
public:
	TranslateToAssimp(const Scene& pgScene) : scene(pgScene) {
		parentStack.push_back(nullptr);
		assimpScene = new aiScene();
	}
	~TranslateToAssimp() {
		delete assimpScene;
	}

	// WOW
	void addChild(aiNode* parent, aiNode* child) {
		if (parent == nullptr) {
			assimpScene->mRootNode = child;
		}
		else {
			parent->mChildren[parent->mNumChildren++] = child;
		}
	}

	aiNode* createGroup(const std::string& name, unsigned int numChildren) {
		auto g = new aiNode();
		g->mName = name.c_str();
		if (numChildren > 0) {
			g->mChildren = new aiNode * [numChildren];
		}
		g->mNumChildren = 0;
		g->mParent = parentStack.back();
		addChild(g->mParent, g);
		return g;
	}

	void apply(Group& group) override {
		auto g = createGroup(group.getName(), static_cast<unsigned int>(group.getNumChildren()));
		parentStack.push_back(g);
		traverse(group);
		parentStack.pop_back();
	}

	void apply(Transform& transform) override {
		auto g = createGroup(transform.getName(), static_cast<unsigned int>(transform.getNumChildren()));

		// OpenGL matrices are column major
		glm::mat4 lm = transform.getTransform();
		aiMatrix4x4 m(
			lm[0][0], lm[1][0], lm[2][0], lm[3][0],
			lm[0][1], lm[1][1], lm[2][1], lm[3][1],
			lm[0][2], lm[1][2], lm[2][2], lm[3][2],
			lm[0][3], lm[1][3], lm[2][3], lm[3][3]);

		g->mTransformation = m;

		parentStack.push_back(g);
		traverse(transform);
		parentStack.pop_back();
	}

	aiVector3D* fromStdVectorVec2ToArrayAiVector3D(const std::vector<glm::vec2>& data) {
		auto v = new aiVector3D[data.size()];
		for (size_t j = 0; j < data.size(); j++) {
			v[j] = aiVector3D(data[j].x, data[j].y, 0.0f);
		}
		return v;
	}

	aiVector3D* fromStdVectorVec3ToArrayAiVector3D(const std::vector<glm::vec3>& data) {
		auto v = new aiVector3D[data.size()];
		for (size_t j = 0; j < data.size(); j++) {
			v[j] = aiVector3D(data[j].x, data[j].y, data[j].z);
		}
		return v;
	}

	void apply(Geode& geode) override {
		auto g = new aiNode(geode.getName());
		addChild(parentStack.back(), g);

		g->mNumMeshes = geode.getModel().getNMeshes();
		g->mMeshes = new unsigned int[g->mNumMeshes];

		for (unsigned int i = 0; i < g->mNumMeshes; i++) {
			auto m = new aiMesh();
			const auto& pgmesh = geode.getModel().getMesh(i);
			m->mName = pgmesh.getName();
			m->mNumVertices = static_cast<unsigned int>(pgmesh.getNVertices());
			auto vtcs = pgmesh.getVertices();
			m->mVertices = fromStdVectorVec3ToArrayAiVector3D(vtcs);
			if (pgmesh.getNNormals() > 0) {
				auto nrmls = pgmesh.getNormals();
				m->mNormals = fromStdVectorVec3ToArrayAiVector3D(nrmls);
			}
			if (pgmesh.getNTexCoord(0) > 0) {
				auto ct = pgmesh.getTexCoords(0);
				m->mTextureCoords[0] = fromStdVectorVec2ToArrayAiVector3D(ct);
			}

			auto bb = pgmesh.getBB();
			m->mAABB.mMin = aiVector3D(bb.min.x, bb.min.y, bb.min.z);
			m->mAABB.mMax = aiVector3D(bb.max.x, bb.max.y, bb.max.z);

			m->mMaterialIndex = 0;
			if (pgmesh.getDrawCommands().size() > 1) {
				WARN("No se soporta una malla con varios drawcommands. Habla con Paco.");
			}
			if (pgmesh.getNIndices() > 0) {
				auto inds = pgmesh.getIndices();
				switch (pgmesh.getDrawCommands()[0]->getGLPrimitiveType()) {
				case GL_TRIANGLES:
				{
					m->mNumFaces = static_cast<unsigned int>(inds.size() / 3);
					m->mFaces = new aiFace[m->mNumFaces];
					for (unsigned int j = 0; j < m->mNumFaces; j++) {
						m->mFaces[j].mNumIndices = 3;
						m->mFaces[j].mIndices = new unsigned int[3];
						m->mFaces[j].mIndices[0] = inds[j * 3];
						m->mFaces[j].mIndices[1] = inds[j * 3 + 1];
						m->mFaces[j].mIndices[2] = inds[j * 3 + 2];
					}
				}
				break;
				default:
					ERRT("Tipo de primitiva no soportada para exportar [aún]. (Habla con Paco)");
				}
			}

			m->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
			g->mMeshes[i] = static_cast<unsigned int>(globalMeshesVector.size());
			globalMeshesVector.push_back(m);
		}
	}

	aiScene* getResult() {
		assimpScene->mNumMeshes = static_cast<unsigned int>(globalMeshesVector.size());
		if (assimpScene->mMeshes)
			delete[] assimpScene->mMeshes;
		assimpScene->mMeshes = new aiMesh * [globalMeshesVector.size()];
		std::copy(globalMeshesVector.begin(), globalMeshesVector.end(), assimpScene->mMeshes);

		assimpScene->mMetaData = new aiMetadata();
		assimpScene->mNumMaterials = 1;
		assimpScene->mMaterials = new aiMaterial * [1];
		assimpScene->mMaterials[0] = new aiMaterial();

		return assimpScene;
	}

private:
	const Scene& scene;
	aiScene* assimpScene;
	std::vector<aiNode*> parentStack;
	std::vector<aiMesh*> globalMeshesVector;
};


bool AssimpWrapper::save(const std::string& path, const std::string& id, std::shared_ptr<Scene> pgscene) {
	auto assScene = std::make_unique<aiScene>();

	CHECK_GL();
	TranslateToAssimp ttass(*pgscene);
	pgscene->getRoot()->accept(ttass);
	auto res = ttass.getResult();

	bool returnValue = AI_SUCCESS == getExporter().Export(res, id, path);
	CHECK_GL();

	return returnValue;
}