#pragma once

#include "vapor/MatrixManager.h"
#include "vapor/ShaderManager.h"

namespace VAPoR {

class LegacyGL;

struct GLManager {
    ShaderManager *shaderManager;
    MatrixManager *matrixManager;
    LegacyGL *legacy;

    GLManager();
    ~GLManager();
};

} // namespace VAPoR
