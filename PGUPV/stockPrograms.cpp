
#include "stockPrograms.h"
#include "program.h"
#include "mesh.h"
#include "glMatrices.h"

using namespace PGUPV;

void PGUPV::buildConstantShading(Program &program) {
	program.addAttributeLocation(Mesh::VERTICES, "position");
	program.addAttributeLocation(Mesh::COLORS, "vertcolor");
	program.replaceString("$" + PGUPV::GLMatrices::blockName, PGUPV::GLMatrices::definition);
	std::vector<std::string> vtxShaderSrc{
	  "#version 420",
	  "$GLMatrices",
	  "in vec4 position;",
	  "in vec4 vertcolor;",
	  "out vec4 fragcolor;",
	  "void main() {",
	  "  fragcolor = vertcolor;",
	  "  gl_Position = modelviewprojMatrix * position;",
	  "}"
	};
	std::vector<std::string> frgShaderSrc{
	  "#version 420",
	  "in vec4 fragcolor;",
	  "out vec4 final_color;",
	  "void main() {",
	  "  final_color = fragcolor;",
	  "}"
	};
	program.loadStrings(vtxShaderSrc, frgShaderSrc);
}

int TextureReplaceProgram::currentTexUnit;

int TextureReplaceProgram::setTextureUnit(int texUnit) {
	auto tu = StockProgram<buildReplaceTexture>::getProgram().getUniformLocation("texUnit");
	glUniform1i(tu, texUnit);
	int prev = TextureReplaceProgram::currentTexUnit;
	TextureReplaceProgram::currentTexUnit = texUnit;
	return prev;
}

Program *TextureReplaceProgram::use() {
	return StockProgram<buildReplaceTexture>::use();
}

void PGUPV::buildReplaceTexture(Program &program) {
	program.addAttributeLocation(Mesh::VERTICES, "position");
	program.addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
	program.replaceString("$" + PGUPV::GLMatrices::blockName, PGUPV::GLMatrices::definition);
	std::vector<std::string> vtxShaderSrc{
	  "#version 420 core",
	  "$GLMatrices",
	  "in vec4 position;",
	  "in vec2 texCoord;",
	  "out vec2 texCoordFrag;",
	  "void main() {",
	  "  texCoordFrag = texCoord;",
	  "  gl_Position = modelviewprojMatrix * position;",
	  "}"
	};
	std::vector<std::string> frgShaderSrc{
	  "#version 420 core",
	  "uniform sampler2D texUnit;",
	  "in vec2 texCoordFrag;",
	  "out vec4 fragColor;",
	  "void main() {",
	  "  fragColor = texture(texUnit, texCoordFrag);",
	  "}"
	};
	program.loadStrings(vtxShaderSrc, frgShaderSrc);
}


GLint ConstantUniformColorProgram::colorLoc = -1;

Program *ConstantUniformColorProgram::use() {
	return StockProgram<PGUPV::buildConstantColorUniform>::use();
}


void ConstantUniformColorProgram::setColor(const glm::vec4 &color) {
	if (colorLoc == -1) 
		colorLoc = StockProgram<PGUPV::buildConstantColorUniform>::getProgram().getUniformLocation("color");
	glUniform4fv(colorLoc, 1, &color.x);
}


void PGUPV::buildConstantColorUniform(Program &program) {
	program.addAttributeLocation(Mesh::VERTICES, "position");
	program.replaceString("$" + PGUPV::GLMatrices::blockName, PGUPV::GLMatrices::definition);
	std::vector<std::string> vtxShaderSrc{
		"#version 420",
		"$GLMatrices",
		"in vec4 position;",
		"void main() {",
		"  gl_Position = modelviewprojMatrix * position;",
		"}"
	};
	std::vector<std::string> frgShaderSrc{
		"#version 420",
		"uniform vec4 color;",
		"out vec4 final_color;",
		"void main() {",
		"  final_color = color;",
		"}"
	};
	program.loadStrings(vtxShaderSrc, frgShaderSrc);
}
