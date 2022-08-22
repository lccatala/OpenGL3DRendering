#ifndef _STOCK_MATERIALS_H
#define _STOCK_MATERIALS_H 2013

#include <memory>

#include "material.h"

namespace PGUPV {

// Lista de materiales comunes
// De: Advanced Graphics Programming Techniques Using OpenGL. SIGGRAPH '98 Course
// y: http://devernay.free.fr/cours/opengl/materials.html

enum class PredefinedMaterial {WHITE, BRASS, BRONZE, POLISHED_BRONZE, CHROME, COPPER, POLISHED_COPPER,
   GOLD, POLISHED_GOLD, PEWTER, SILVER, POLISHED_SILVER, 
   EMERALD, JADE, OBSIDIAN, PEARL, RUBY, TURQUOISE, 
	BLACK_PLASTIC, BLACK_RUBBER,  CYAN_PLASTIC, GREEN_PLASTIC, RED_PLASTIC,
	WHITE_PLASTIC, YELLOW_PLASTIC, CYAN_RUBBER, GREEN_RUBBER, RED_RUBBER,
	WHITE_RUBBER, YELLOW_RUBBER};

/* Devuelve un objeto Material con las propiedades de uno de los materiales predefinidos */
std::shared_ptr<Material> getMaterial(PredefinedMaterial matid);

};

#endif