
#ifndef TWODDATAPARAMS_H
#define TWODDATAPARAMS_H

#include <vapor/DataMgr.h>
#include <vapor/RenderParams.h>

namespace VAPoR {

//! \class TwoDDataParams
//! \brief Class that supports drawing Barbs based on 2D or 3D vector field
//! \author Alan Norton
//! \version 3.0
class PARAMS_API TwoDDataParams : public RenderParams {
  public:
    TwoDDataParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave);

    TwoDDataParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave, XmlNode *node);

    virtual ~TwoDDataParams();

#ifdef VAPOR3_0_0_ALPHA
    //! \copydoc Params::Validate()
    virtual void Validate(int type);
#endif

    // Get static string identifier for this params class
    //
    static string GetClassType() { return ("TwoDDataParams"); }

  private:
    void _init();
#ifdef VAPOR3_0_0_ALPHA
    void _validateTF(int type, DataMgr *dataMgr);
#endif

}; // End of Class TwoDDataParams
}; // namespace VAPoR

#endif
