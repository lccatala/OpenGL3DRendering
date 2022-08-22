
#pragma once

#include <string>
#include <GL/glew.h>

#include "common.h"

namespace PGUPV {
    
    struct GLSLTypeInfo {
        GLenum glEnum;
        std::string name;
        uint numComponents; // Number of elements
        GLenum baseTypeGLEnum;
        uint sizePerComponent;
        unsigned short ncols, nrows;
        uint std140ArrayStride;
    };

    
    std::string formatGLSLValue(const GLSLTypeInfo &t, uint size, void *ptr,
                                bool ubo = false);
    std::string formatSingleGLSLValue(const GLSLTypeInfo &t, void *ptr,
                                             bool ubo = false);
    const GLSLTypeInfo &getGLSLTypeInfo(GLenum type);
};