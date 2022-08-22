
#include <glm/glm.hpp>

#include "stockMaterials.h"
#include "utils.h"

using PGUPV::Material;
using glm::vec4;


// Lista de materiales comunes
// De: Advanced Graphics Programming Techniques Using OpenGL. SIGGRAPH '98 Course
// y: http://devernay.free.fr/cours/opengl/materials.html

struct Tmaterial {
	std::string name;
	float ambient[4], diffuse[4], specular[4], shininess;
};

static Tmaterial matdb[] = {
  { "White",
	{.8f, .8f, .8f, 1.0f},
  {.9f, .9f, .9f, 1.0f},
  {.9f, .9f, .9f, 1.0f}, 30.0f},
  { "Brass",
	{0.32941f, 0.223529f, 0.027451f, 1.0f},
  {0.780392f, 0.568627f, 0.113725f, 1.0f},
  {0.992157f, 0.941176f, 0.807843f, 1.0f}, 27.8974f},
  { "Bronze",
	{0.2125f, 0.1275f, 0.054f, 1.0f},
   {0.714f, 0.4284f, 0.18144f, 1.0f},
   {0.393548f, 0.271906f, 0.166721f}, 25.6f},
  {"Polished Bronze",
  {0.25f, 0.148f, 0.06475f, 1.0f},
  {0.4f, 0.2368f,0.1036f,1.0},
  {0.774597f, 0.458561f, 0.200621f, 1.0f}, 76.8f},
  { "Chrome",
	{0.25f, 0.25f, 0.25f, 1.0f},
  {0.4f, 0.4f, 0.4f, 1.0f},
  {0.774597f, 0.774597f, 0.774597f,1.0f}, 76.8f},
  { "Copper",
	{0.19125f, 0.0735f, 0.0225f,1.0f},
  {0.7038f, 0.27048f, 0.0828f,1.0},
  {0.256777f, 0.137622f, 0.086014f,1.0f},12.8f},
  { "Polished Copper",
  {0.2295f, 0.08825f, 0.0275f,1.0f},
  {0.5508f, 0.2118f, 0.066f,1.0},
  {0.580594f, 0.223257f, 0.0695701f,1.0f},51.2f},
  { "Gold",
  {0.24725f, 0.1995f, 0.0745f,1.0f},
  {0.75164f, 0.60648f, 0.22648f,1.0f},
  {0.628281f, 0.555802f, 0.366065f,1.0f}, 51.2f},
  { "Polished Gold",
	{0.24725f, 0.2245f, 0.0645f,1.0f},
  {0.34615f, 0.3143f, 0.0903f, 1.0f},
  {0.797357f, 0.723991f, 0.208006f,1.0f},83.2f},
  { "Pewter",
  {0.105882f, 0.058824f, 0.113725f, 1.0f},
  {0.427451f, 0.470588f, 0.541176f, 1.0f},
  {0.333333f, 0.333333f, 0.521569f, 1.0f},9.84615f},
  { "Silver",
  {0.19225f, 0.19225f, 0.19225f,1.0f},
  {0.50754f, 0.50754f, 0.50754f,1.0},
  {0.508273f, 0.508273f, 0.508273f,1.0f},51.2f},
  { "Polished Silver",
  {0.23125f, 0.23125f, 0.23125f,1.0f},
  {0.2775f, 0.2775f, 0.2775f,1.0},
  {0.773911f, 0.773911f, 0.773911f,1.0f},89.6f},
  { "Emerald",
  {0.0215f, 0.1745f, 0.0215f, 0.55f},
  {0.07568f, 0.61424f, 0.07568f, 0.55f},
  {0.633f, 0.727811f, 0.633f, 0.55f},76.8f},
  { "Jade",
  {0.135f, 0.2225f, 0.1575f, 0.95f},
  {0.54f, 0.89f, 0.63f, 0.95f},
  {0.316228f, 0.316228f, 0.316228f, 0.95f},12.8f},
  { "Obsidian",
  {0.05375f, 0.05f, 0.06625f, 0.82f},
  { 0.18275f, 0.17f, 0.22525f, 0.82f},
  { 0.332741f, 0.328634f, 0.346435f, 0.82f}, 38.4f},
  { "Pearl",
  {0.25f, 0.20725f, 0.20725f, 0.922f},
  {1.0f, 0.829f, 0.829f, 0.922f},
  { 0.296648f, 0.296648f, 0.296648f, 0.922f}, 11.264f},
  { "Ruby",
  {0.1745f, 0.01175f,0.01175f, 0.55f},
  {0.61424f,0.04136f, 0.04136f, 0.55f },
  {0.727811f, 0.626959f, 0.626959f, 0.55f}, 76.8f},
  { "Turquoise",
  {0.1f, 0.18725f, 0.1745f, 0.8f},
  {0.396f, 0.74151f, 0.69102f, 0.8f},
  {0.297254f, 0.30829f, 0.306678f, 0.8f}, 12.8f},
  { "Black Plastic",
	{0.0f, 0.0f, 0.0f,1.0f},
  {0.01f, 0.01f, 0.01f,1.0},
  {0.5f, 0.5f, 0.5f, 1.0f}, 32.0f},
  { "Black Rubber",
  {0.02f, 0.02f, 0.02f, 1.0f},
  {0.01f, 0.01f, 0.01f, 1.0f},
  {0.4f, 0.4f, 0.4f, 1.0f}, 10.0f},
  { "Cyan Plastic",
  {0.0f, 0.1f, 0.06f, 1.0f},
  {0.0f, 0.50980392f, 0.50980392f, 1.0f},
  {0.50196078f, 0.50196078f, 0.50196078f, 1.0f}, 32.0f},
  { "Green Plastic",
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.1f, 0.35f, 0.1f, 1.0f},
  {0.45f, 0.55f, 0.45f, 1.0f}, 32.0f},
  { "Red Plastic",
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.5f, 0.0f, 0.0f, 1.0f},
  {0.7f, 0.6f, 0.6f, 1.0f}, 32.0f},
  { "White Plastic",
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.55f, 0.55f, 0.55f, 1.0f},
  {0.7f, 0.7f, 0.7f, 1.0f}, 32.0f},
  { "Yellow Plastic",
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.5f, 0.5f, 0.0f, 1.0f},
  {0.6f, 0.6f, 0.5f, 1.0f}, 32.0f},
  { "Cyan Rubber",
  {0.0f, 0.05f, 0.05f, 1.0f},
  {0.4f, 0.5f, 0.5f, 1.0f},
  {0.04f, 0.7f, 0.7f, 1.0f}, 10.0f},
  { "Green Rubber",
	  {0.0f, 0.05f, 0.0f, 1.0f},
  {0.4f, 0.5f, 0.4f, 1.0f},
  {0.04f, 0.7f, 0.04f, 1.0f}, 10.0f},
  { "Red Rubber",
  {0.05f, 0.0f, 0.0f, 1.0f},
  {0.5f, 0.4f, 0.4f, 1.0f},
  {0.7f, 0.04f, 0.04f, 1.0f}, 10.0f},
  { "White Rubber",
  {0.05f, 0.05f, 0.05f, 1.0f},
  {0.5f, 0.5f, 0.5f, 1.0f},
  {0.7f, 0.5f, 0.5f, 1.0f}, 10.0f},
  { "Yellow Rubber",
  {0.05f, 0.05f, 0.0f, 1.0f},
  {0.5f, 0.5f, 0.4f, 1.0f},
  {0.7f, 0.7f, 0.04f, 1.0f}, 10.0f}
};

#define DIVIDE(a) (a)[0], (a)[1], (a)[2], (a)[3]
std::shared_ptr<Material> PGUPV::getMaterial(PredefinedMaterial matid) {
	auto indexMat = PGUPV::to_underlying(matid);

	return std::make_shared<Material>(
		matdb[indexMat].name,
		vec4(DIVIDE(matdb[indexMat].ambient)),
		vec4(DIVIDE(matdb[indexMat].diffuse)),
		vec4(DIVIDE(matdb[indexMat].specular)),
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		matdb[indexMat].shininess);
}
#undef DIVIDE

