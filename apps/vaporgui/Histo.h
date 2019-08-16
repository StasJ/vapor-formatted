//************************************************************************
//									*
//		     Copyright (C)  2004				*
//     University Corporation for Atmospheric Research			*
//		     All Rights Reserved				*
//									*
//************************************************************************/
//
//	File:		Histo.h
//
//	Author:		Alan Norton
//			National Center for Atmospheric Research
//			PO 3000, Boulder, Colorado
//
//	Date:		November 2004
//
//	Description:  Definition of Histo class:
//		it contains a histogram derived from volume data.
//		Used by TFEditor to draw histogram behind transfer function opacity
//
#ifndef HISTO_H
#define HISTO_H
#include <vapor/DataMgr.h>
#include <vapor/MyBase.h>
#include <vapor/RenderParams.h>
#include <vapor/StructuredGrid.h>

class Histo {
  public:
    Histo(int numberBins, float mnData, float mxData, string var, int ts);
    Histo(int numberBins);
    Histo(const Histo *histo);
    ~Histo();
    void reset(int newNumBins = -1);
    void reset(int newNumBins, float mnData, float mxData);
    void addToBin(float val);
    long getMaxBinSize();
    long getBinSize(int posn) const { return _binArray[posn]; }
    float getBinSizeNormalized(int bin) const;
    float getMinData() { return _minData; }
    float getMaxData() { return _maxData; }

    int getTimestepOfUpdate() { return _timestepOfUpdate; }
    string getVarnameOfUpdate() { return _varnameOfUpdate; }

    int Populate(VAPoR::DataMgr *dm, VAPoR::RenderParams *rp);

  private:
    long *_binArray = nullptr;
    long _numBelow, _numAbove;
    int _numBins = 0;
    float _minData, _maxData, _range;
    long _maxBinSize = -1;

    int _timestepOfUpdate;
    string _varnameOfUpdate;
    bool autoSetProperties = false;

    void populateIteratingHistogram(const VAPoR::Grid *grid, const int stride);
    void populateSamplingHistogram(const VAPoR::Grid *grid, const vector<double> &minExts,
                                   const vector<double> &maxExts);
    int calculateStride(VAPoR::DataMgr *dm, const VAPoR::RenderParams *rp) const;
    bool shouldUseSampling(VAPoR::DataMgr *dm, const VAPoR::RenderParams *rp) const;
    void setProperties(float mnData, float mxData, string var, int ts);
    void calculateMaxBinSize();
};

#endif // HISTO_H
