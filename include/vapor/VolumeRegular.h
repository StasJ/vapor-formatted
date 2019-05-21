#pragma once

#include <vapor/Texture.h>
#include <vapor/VolumeAlgorithm.h>

namespace VAPoR {

//! \class VolumeRegular
//! \ingroup Public_Render
//!
//! \brief Regular grid rendering algorithm
//!
//! \author Stanislaw Jaroszynski
//! \date Feburary, 2019
//!
//! Renders a regular grid by ray tracing. The CPU side just loads
//! the scalar data and missing values as well as secondary data if needed
//!
//! The glsl code does a standard sampled ray tracing of the volume.

class VolumeRegular : public VolumeAlgorithm {
  public:
    VolumeRegular(GLManager *gl);
    ~VolumeRegular();

    static std::string GetName() { return "Regular"; }
    static Type GetType() { return Type::DVR; }
    virtual bool IsSlow() { return false; }

    virtual int LoadData(const Grid *grid);
    virtual int LoadSecondaryData(const Grid *grid);
    virtual void DeleteSecondaryData();
    virtual ShaderProgram *GetShader() const;
    virtual void SetUniforms(const ShaderProgram *shader) const;
    virtual float GuestimateFastModeSpeedupFactor() const;

  protected:
    Texture3D _data;
    Texture3D _missing;
    bool _hasMissingData;

    std::vector<size_t> _dataDimensions;

    bool _hasSecondData;
    Texture3D _data2;
    Texture3D _missing2;
    bool _hasMissingData2;

    int _loadDataDirect(const Grid *grid, Texture3D *dataTexture, Texture3D *missingTexture,
                        bool *hasMissingData);
};

//! \class VolumeRegularIso
//! \ingroup Public_Render
//!
//! \brief Regular grid isosurface rendering algorithm
//!
//! \author Stanislaw Jaroszynski
//! \date Feburary, 2019
//!
//! Renders isosurfaces by ray tracing. This does the same CPU side tasks
//! as the volume renderer but it provides different GLSL code.

class VolumeRegularIso : public VolumeRegular {
  public:
    VolumeRegularIso(GLManager *gl) : VolumeRegular(gl) {}
    static std::string GetName() { return "Iso Regular"; }
    static Type GetType() { return Type::Iso; }
    virtual ShaderProgram *GetShader() const;
    virtual void SetUniforms(const ShaderProgram *shader) const;
};

} // namespace VAPoR
