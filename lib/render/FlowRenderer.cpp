#include "vapor/FlowRenderer.h"
#include "vapor/OceanField.h"
#include "vapor/Particle.h"
#include "vapor/SteadyVAPORField.h"
#include "vapor/glutil.h"
#include <cstring>
#include <iostream>

#define GL_ERROR -20

using namespace VAPoR;

static RendererRegistrar<FlowRenderer> registrar(FlowRenderer::GetClassType(),
                                                 FlowParams::GetClassType());

GLenum glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Constructor
FlowRenderer::FlowRenderer(const ParamsMgr *pm, std::string &winName, std::string &dataSetName,
                           std::string &instName, DataMgr *dataMgr)
    : Renderer(pm, winName, dataSetName, FlowParams::GetClassType(), FlowRenderer::GetClassType(),
               instName, dataMgr) {
    _shader = nullptr;
    _velField = nullptr;

    _vertexArrayId = 0;
    _vertexBufferId = 0;
}

// Destructor
FlowRenderer::~FlowRenderer() {
    if (_velField) {
        delete _velField;
        _velField = nullptr;
    }

    // Delete vertex arrays
    if (_vertexArrayId) {
        glDeleteVertexArrays(1, &_vertexArrayId);
        _vertexArrayId = 0;
    }
    if (_vertexBufferId) {
        glDeleteBuffers(1, &_vertexBufferId);
        _vertexBufferId = 0;
    }
}

int FlowRenderer::_initializeGL() {
    ShaderProgram *shader = nullptr;
    if ((shader = _glManager->shaderManager->GetShader("FlowLine")))
        _shader = shader;
    else
        return GL_ERROR;

    /* Create Vertex Array Object (VAO) */
    glGenVertexArrays(1, &_vertexArrayId);
    glGenBuffers(1, &_vertexBufferId);

    return 0;
}

int FlowRenderer::_paintGL(bool fast) {
    int ready = _advec.IsReady();
    if (ready != 0) {
        _useSteadyVAPORField();
    }

    size_t numOfStreams = _advec.GetNumberOfStreams();
    for (size_t i = 0; i < numOfStreams; i++) {
        const auto &s = _advec.GetStreamAt(i);
        _drawAStream(s);
    }

    return 0;
}

int FlowRenderer::_drawAStream(const std::vector<flow::Particle> &stream) const {
    size_t numOfPart = stream.size();
    float *posBuf = new float[3 * numOfPart];
    size_t offset = 0;
    for (const auto &p : stream) {
        std::memcpy(posBuf + offset, glm::value_ptr(p.location), sizeof(glm::vec3));
        offset += 3;
        // posBuf[offset++] = p.location.x * 20.0 + 40.0;
        // posBuf[offset++] = p.location.y * 20.0 + 40.0;
        // posBuf[offset++] = p.location.z * 20.0 + 40.0;
    }

    glm::mat4 modelview = _glManager->matrixManager->GetModelViewMatrix();
    glm::mat4 projection = _glManager->matrixManager->GetProjectionMatrix();
    _shader->Bind();
    _shader->SetUniform("MV", modelview);
    _shader->SetUniform("Projection", projection);
    glBindVertexArray(_vertexArrayId);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numOfPart, posBuf, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glDrawArrays(GL_LINE_STRIP, 0, numOfPart);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    delete[] posBuf;

    return 0;
}

void FlowRenderer::_useOceanField() {
    if (_velField) {
        delete _velField;
        _velField = nullptr;
    }
    _velField = new flow::OceanField();
    _advec.UseVelocityField(_velField);
    _advec.SetBaseStepSize(0.1f);

    int numOfSeeds = 5, numOfSteps = 100;
    std::vector<flow::Particle> seeds(numOfSeeds);
    seeds[0].location = glm::vec3(0.65f, 0.65f, 0.1f);
    seeds[1].location = glm::vec3(0.3f, 0.3f, 0.1f);
    for (int i = 2; i < numOfSeeds; i++)
        seeds[i].location = glm::vec3(float(i + 1) / float(numOfSeeds + 1), 0.0f, 0.0f);
    _advec.UseSeedParticles(seeds);
    for (int i = 0; i < numOfSteps; i++)
        _advec.Advect(flow::Advection::RK4);
}

int FlowRenderer::_useSteadyVAPORField() {
    // Step 1: retrieve variable names from the params class
    FlowParams *params = dynamic_cast<FlowParams *>(GetActiveParams());
    std::vector<std::string> velVars = params->GetFieldVariableNames();
    assert(velVars.size() == 3); // need to have three components
    for (auto &s : velVars) {
        if (s.empty()) {
            MyBase::SetErrMsg("Missing velocity field");
            return flow::GRID_ERROR;
        }
    }

    // Step 2: use these variable names to get data grids
    Grid *gridU, *gridV, *gridW;
    int currentTS = params->GetCurrentTimestep();
    int rv = _getAGrid(params, currentTS, velVars[0], &gridU);
    if (rv != 0)
        return rv;
    rv = _getAGrid(params, currentTS, velVars[1], &gridV);
    if (rv != 0)
        return rv;
    rv = _getAGrid(params, currentTS, velVars[2], &gridW);
    if (rv != 0)
        return rv;

    // Step 3: repeat step 2 for the color mapping variable
    Grid *scalarP = nullptr;
    bool singleColor = params->UseSingleColor();
    if (!singleColor) {
        std::string scalarVar = params->GetColorMapVariableName();
        if (!scalarVar.empty()) {
            rv = _getAGrid(params, currentTS, scalarVar, &scalarP);
            if (rv != 0)
                return rv;
        }
    }

    // Step 4: create a SteadyVAPORField using these grids, and ask Advection to use it!
    flow::SteadyVAPORField *field = new flow::SteadyVAPORField();
    field->UseVelocities(gridU, gridV, gridW);
    if (!singleColor)
        field->UseScalar(scalarP);

    // Step 5: hand over this velocity field
    if (_velField)
        delete _velField;
    _velField = field;

    // Plant seeds
    std::vector<flow::Particle> seeds;
    _genSeedsXY(seeds);
    _advec.UseSeedParticles(seeds);
    _advec.UseVelocityField(_velField);

    // Do some advection
    int numOfSteps = 200;
    for (int i = 0; i < numOfSteps; i++)
        _advec.Advect(flow::Advection::RK4);

    return 0;
}

int FlowRenderer::_genSeedsXY(std::vector<flow::Particle> &seeds) const {
    int numX = 5, numY = 5;
    std::vector<double> extMin, extMax;
    FlowParams *params = dynamic_cast<FlowParams *>(GetActiveParams());
    params->GetBox()->GetExtents(extMin, extMax);
    float stepX = (extMax[0] - extMin[0]) / (numX + 1.0);
    float stepY = (extMax[1] - extMin[1]) / (numY + 1.0);
    float stepZ = extMin[2] + (extMax[2] - extMin[2]) / 100.0;

    seeds.resize(numX * numY);
    for (int y = 0; y < numY; y++)
        for (int x = 0; x < numX; x++) {
            int idx = y * numX + x;
            seeds[idx].location.x = extMin[0] + (x + 1.0f) * stepX;
            seeds[idx].location.y = extMin[1] + (y + 1.0f) * stepY;
            seeds[idx].location.z = stepZ;
        }

    return 0;
}

int FlowRenderer::_getAGrid(const FlowParams *params, int timestep, std::string &varName,
                            Grid **gridpp) const {
    std::vector<double> extMin, extMax;
    params->GetBox()->GetExtents(extMin, extMax);
    Grid *grid = _dataMgr->GetVariable(timestep, varName, params->GetRefinementLevel(),
                                       params->GetCompressionLevel(), extMin, extMax);
    if (grid == nullptr) {
        MyBase::SetErrMsg("Not able to get a grid!");
        return flow::GRID_ERROR;
    } else {
        *gridpp = grid;
        return 0;
    }
}

#ifndef WIN32
double FlowRenderer::_getElapsedSeconds(const struct timeval *begin,
                                        const struct timeval *end) const {
    return (end->tv_sec - begin->tv_sec) + ((end->tv_usec - begin->tv_usec) / 1000000.0);
}
#endif
