/*****************************************************************************
 *
 * Copyright (c) 2000 - 2009, Lawrence Livermore National Security, LLC
 * Produced at the Lawrence Livermore National Laboratory
 * LLNL-CODE-400124
 * All rights reserved.
 *
 * This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
 * full copyright notice is contained in the file COPYRIGHT located at the root
 * of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
 *
 * Redistribution  and  use  in  source  and  binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of  source code must  retain the above  copyright notice,
 *    this list of conditions and the disclaimer below.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
 * ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
 * LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
 * DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
 * CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
 * LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
 * OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 *****************************************************************************/

// ************************************************************************* //
//                            avtvdfFileFormat.h                           //
// ************************************************************************* //

#ifndef AVT_vdf_FILE_FORMAT_H
#define AVT_vdf_FILE_FORMAT_H

#include <avtDataSelection.h>
#include <avtMTMDFileFormat.h>
#include <avtMeshMetaData.h>

#include <map>
#include <vector>

#include "vapor/DataMgr.h"
#include "vapor/VDFIOBase.h"
#include "vapor/WaveletBlock3DRegionReader.h"
#include "vapor/WaveletBlockIOBase.h"

class DBOptionsAttributes;

// ****************************************************************************
//  Class: avtvdfFileFormat
//
//  Purpose:
//      Reads in vdf files as a plugin to VisIt.
//
//  Programmer: dlagreca -- generated by xml2avt
//  Creation:   Wed Feb 24 15:32:35 PST 2010
//
// ****************************************************************************

class avtvdfFileFormat : public avtMTMDFileFormat {
  public:
    avtvdfFileFormat(const char *, DBOptionsAttributes *);
    virtual ~avtvdfFileFormat();

    //
    // This is used to return unconvention data -- ranging from material
    // information to information about block connectivity.
    //
    virtual void *GetAuxiliaryData(const char *var, int timestep, int domain, const char *type,
                                   void *args, DestructorFunction &);
    //

    //
    // If you know the times and cycle numbers, overload this function.
    // Otherwise, VisIt will make up some reasonable ones for you.
    //
    virtual void GetCycles(std::vector<int> &);
    virtual void GetTimes(std::vector<double> &);
    virtual int GetNTimesteps(void) { return ntsteps; };

    virtual const char *GetType(void) { return "vdf"; };
    virtual void FreeUpResources(void);

    virtual vtkDataSet *GetMesh(int, int, const char *);
    virtual vtkDataArray *GetVar(int, int, const char *);
    virtual vtkDataArray *GetVectorVar(int, int, const char *);

  protected:
    // structure used to pass info about ghost boundaries (padding)
    typedef struct datasize_t {
        size_t real[3];     // dimensions of actual voxel data
        size_t padded[3];   // dimensions of data with padding
        size_t frontPad[3]; // padding before real data
        size_t backPad[3];  // padding after real data
    };

    bool isLayered, // weather or not the VDF contains layered a dataset
        multiDom;   // weather or not we expose multiple domains to VisIt
    int num_levels, // max # of refinement levels
        ntsteps,    // number of valid timesteps
        *tsteps,    // map valid timestate (0-indexed) to dataset timestep
        levels3D,   // Highest refinement level 3D mesh needed
        levelsXY,   // Highest refinement level 2D XY mesh needed
        levelsXZ,   // Highest refinement level 2D XZ mesh needed
        levelsYZ;   // Highest refinement level 2D YZ mesh needed

    VAPoR::MetadataVDC *vdc_md;           // determines type of datamanger is needed
    VAPoR::WaveletBlockIOBase *vdfiobase; // determines type of datamanger is needed
    VAPoR::DataMgr *data_mgr;             // Pointer to subclass

    // relate var name to dimensional type (3D,XY,XZ,YZ)
    map<string, int> varTypes;

    // relate var name to refinement level
    map<string, int> refLevel;

    // Hold names of all vars in dataset, per type
    vector<string> Names3D, xyNames, xzNames, yzNames, meshNames;

    virtual void PopulateDatabaseMetaData(avtDatabaseMetaData *, int);
    bool CanCacheVariable(const char *var) { return false; };

    void getVoxelsInBlock(size_t *, size_t *, size_t *, size_t *);
    void getCoordsFromIndex(size_t *, size_t *, int);
    void getDatasizeFromCoords(datasize_t *, size_t *, size_t *, size_t *, size_t *);
    char *cleanVarname(const char *);
    void getVarDims(string, size_t *, size_t *);
    inline void getVarDims(int, size_t *, size_t *);
};

#endif
