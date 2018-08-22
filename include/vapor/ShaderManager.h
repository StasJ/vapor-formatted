#pragma once

#include "vapor/ShaderProgram2.h"
#include <map>
#include <string>

namespace VAPoR {

class ShaderManager {
    std::map<std::string, ShaderProgram2 *> _shaderMap;
    std::string _shaderPathPrefix;

  public:
    ShaderProgram2 *GetShader(const std::string name);
    SmartShaderProgram GetSmartShader(const std::string name);
    bool HasShader(const std::string name);
    bool HasShader(const ShaderProgram2 *shader);
    bool SetShaderDirectory(const std::string path);
    bool LoadShaderByName(const std::string name);
    bool AddShader(const std::string name, ShaderProgram2 *shader);
};

} // namespace VAPoR
