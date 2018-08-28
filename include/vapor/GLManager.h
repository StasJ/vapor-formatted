#pragma once

#include "vapor/FontManager.h"
#include "vapor/LegacyGL.h"
#include "vapor/MatrixManager.h"
#include "vapor/ShaderManager.h"

namespace VAPoR {

struct GLManager {
    ShaderManager *shaderManager;
    FontManager *fontManager;
    MatrixManager *matrixManager;
    LegacyGL *legacy;

    GLManager();
    ~GLManager();
};

} // namespace VAPoR
