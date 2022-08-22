
#include <algorithm>

#include "shaderLibrary.h"
#include "program.h"
#include "log.h"
#include "model.h"

using PGUPV::ShaderLibrary;
using PGUPV::Program;
using PGUPV::Mesh;

ShaderLibrary::ShaderLibrary() {

}


void ShaderLibrary::add(Program *shader) {
	library.push_back(shader);
}

void ShaderLibrary::remove(Program *shader) {
	auto it = std::find(library.begin(), library.end(), shader);
	if (it == library.end()) {
		ERR("El shader " + shader->getFileName(Shader::ShaderType::VERTEX_SHADER) + " no estaba registrado");
	}
	else {
		library.erase(it);
	}
}


// Returns the number of registered programs	
uint ShaderLibrary::size() {
	return (uint)library.size();
}

// Returns the ith shader 
Program *ShaderLibrary::get(uint i) {
	return library[i];
}

void ShaderLibrary::printInfoShaders(std::ostream &os, bool verbose) {
	Program::printOpenGLInfo(os, verbose);
	for (uint i = 0; i < library.size(); i++) {
		library[i]->printInfo(os);
	}
}
