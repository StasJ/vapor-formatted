#pragma once

#include <glm/glm.hpp>
#include <string>

namespace VAPoR {

struct GLManager;
class Font;

class TextLabel {
    GLManager *_glManager;

  public:
    enum Alignment { Left, Center, Right, Top, Bottom };

    std::string FontName;
    unsigned int FontSize;
    glm::vec4 BackgroundColor;
    glm::vec4 ForegroundColor;
    Alignment HorizontalAlignment;
    Alignment VerticalAlignment;
    float Padding;

    TextLabel(GLManager *glManager, const std::string &fontName, unsigned int fontSize);

    void DrawText(const glm::vec2 &position, const std::string &text);
    void DrawText(const glm::vec3 &position, const std::string &text);

    Font *GetFont() const;
};

} // namespace VAPoR
