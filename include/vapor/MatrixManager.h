#pragma once

#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <map>
#include <stack>
#include <string>

using std::map;
using std::stack;
using std::string;

namespace VAPoR {

class MatrixManager {
  public:
    enum class Mode { ModelView, Projection };

    MatrixManager();

    glm::mat4 GetCurrentMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetModelViewMatrix() const;
    glm::mat4 GetModelViewProjectionMatrix() const;
    void SetCurrentMatrix(const glm::mat4 m);

    void MatrixModeProjection();
    void MatrixModeModelView();
    void PushMatrix();
    void PopMatrix();

    void LoadMatrixd(const double *m);
    void GetDoublev(Mode mode, double *m) const;

    void LoadIdentity();
    void Translate(float x, float y, float z);
    void Scale(float x, float y, float z);
    void Rotate(float angle, float x, float y, float z);
    void Perspective(float fovy, float aspect, float zNear, float zFar);
    void Ortho(float left, float right, float bottom, float top);
    void Ortho(float left, float right, float bottom, float top, float zNear, float zFar);

    glm::vec2 ProjectToScreen(float x, float y, float z) const;
    glm::vec2 ProjectToScreen(const glm::vec3 &v) const;

#ifndef NDEBUG
    void Test();
    void TestUpload();
    int GetGLMatrixMode();
    const char *GetGLMatrixModeStr();
    int GetGLModelViewStackDepth();
    int GetGLProjectionStackDepth();
    int GetGLCurrentStackDepth();
    const char *GetMatrixModeStr();

    float Near, Far;
#endif

  private:
    stack<glm::mat4> _modelviewStack;
    stack<glm::mat4> _projectionStack;
    stack<glm::mat4> *_currentStack;
    Mode _mode;

    glm::mat4 &top();
    const glm::mat4 &top() const;
};

} // namespace VAPoR
