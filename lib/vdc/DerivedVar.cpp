#include <algorithm>
#include <cassert>
#include <sstream>
#include <vapor/DerivedVar.h>
#include <vapor/NetCDFCollection.h>
#include <vapor/UDUnitsClass.h>
#include <vapor/utils.h>

using namespace VAPoR;
using namespace Wasp;

namespace {

size_t numBlocks(size_t min, size_t max, size_t bs) {
    size_t b0 = min / bs;
    size_t b1 = max / bs;
    return (b1 - b0 + 1);
}

size_t numBlocks(const vector<size_t> &min, const vector<size_t> &max, const vector<size_t> &bs) {
    assert(min.size() == max.size());
    assert(min.size() == bs.size());

    size_t nblocks = 1;
    for (int i = 0; i < bs.size(); i++) {
        nblocks *= numBlocks(min[i], max[i], bs[i]);
    }
    return (nblocks);
}

#ifdef UNUSED_FUNCTION
size_t numBlocks(const vector<size_t> &dims, const vector<size_t> &bs) {
    assert(dims.size() == bs.size());

    size_t nblocks = 1;
    for (int i = 0; i < bs.size(); i++) {
        assert(dims[i] != 0);
        nblocks *= (((dims[i] - 1) / bs[i]) + 1);
    }
    return (nblocks);
}
#endif

size_t numElements(const vector<size_t> &min, const vector<size_t> &max) {
    assert(min.size() == max.size());

    size_t nElements = 1;
    for (int i = 0; i < min.size(); i++) {
        nElements *= (max[i] - min[i] + 1);
    }
    return (nElements);
}

size_t blockSize(const vector<size_t> &bs) {
    size_t sz = 1;
    for (int i = 0; i < bs.size(); i++) {
        sz *= bs[i];
    }
    return (sz);
}

vector<size_t> increment(vector<size_t> dims, vector<size_t> coord) {
    assert(dims.size() == coord.size());

    for (int i = 0; i < coord.size(); i++) {
        coord[i] += 1;
        if (coord[i] < (dims[i])) {
            break;
        }
        coord[i] = 0;
    }
    return (coord);
}

// Product of elements in a vector
//
size_t vproduct(vector<size_t> a) {
    size_t ntotal = 1;

    for (int i = 0; i < a.size(); i++)
        ntotal *= a[i];
    return (ntotal);
}

void extractBlock(const float *data, const vector<size_t> &dims, const vector<size_t> &bcoords,
                  const vector<size_t> &bs, float *block) {
    assert(dims.size() == bcoords.size());
    assert(dims.size() == bs.size());

    // Block dimensions
    //
    size_t bz = bs.size() > 2 ? bs[2] : 1;
    size_t by = bs.size() > 1 ? bs[1] : 1;
    size_t bx = bs.size() > 0 ? bs[0] : 1;

    // Data dimensions
    //
    size_t nz = dims.size() > 2 ? dims[2] : 1;
    size_t ny = dims.size() > 1 ? dims[1] : 1;
    size_t nx = dims.size() > 0 ? dims[0] : 1;

    // Data dimensions
    //
    size_t bcz = bcoords.size() > 2 ? bcoords[2] : 0;
    size_t bcy = bcoords.size() > 1 ? bcoords[1] : 0;
    size_t bcx = bcoords.size() > 0 ? bcoords[0] : 0;

    size_t z = bcz * bz;
    for (size_t zb = 0; zb < bz && z < nz; zb++, z++) {
        size_t y = bcy * by;

        for (size_t yb = 0; yb < by && y < ny; yb++, y++) {
            size_t x = bcx * bx;

            for (size_t xb = 0; xb < bx && x < nx; xb++, x++) {
                block[bx * by * zb + bx * yb + xb] = data[nx * ny * z + nx * y + x];
            }
        }
    }
}

void blockit(const float *data, const vector<size_t> &dims, const vector<size_t> &bs,
             float *blocks) {
    assert(dims.size() == bs.size());

    size_t block_size = vproduct(bs);

    vector<size_t> bdims;
    for (int i = 0; i < bs.size(); i++) {
        bdims.push_back(((dims[i] - 1) / bs[i]) + 1);
    }

    size_t nbz = bdims.size() > 2 ? bdims[2] : 1;
    size_t nby = bdims.size() > 1 ? bdims[1] : 1;
    size_t nbx = bdims.size() > 0 ? bdims[0] : 1;

    float *blockptr = blocks;
    vector<size_t> bcoord(bdims.size(), 0);

    for (size_t zb = 0; zb < nbz; zb++) {
        for (size_t yb = 0; yb < nby; yb++) {
            for (size_t xb = 0; xb < nbx; xb++) {

                extractBlock(data, dims, bcoord, bs, blockptr);
                blockptr += block_size;
                bcoord = increment(bdims, bcoord);
            }
        }
    }
}

// make 2D lat and lon arrays from 1D arrays by replication, in place
//
void make2D(float *lonBuf, float *latBuf, vector<size_t> dims) {
    assert(dims.size() == 2);
    size_t nx = dims[0];
    size_t ny = dims[1];

    // longitude
    //
    for (int j = 1; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            lonBuf[j * nx + i] = lonBuf[i];
        }
    }

    // latitude requires a transpose first
    //
    for (int i = 0; i < ny; i++) {
        latBuf[i * nx] = latBuf[i];
    }

    for (int j = 0; j < ny; j++) {
        for (int i = 1; i < nx; i++) {
            latBuf[j * nx + i] = latBuf[j * nx];
        }
    }
}

bool parse_formula(string formula_terms, map<string, string> &parsed_terms) {
    parsed_terms.clear();

    // Remove ":" to ease parsing. It's superflous
    //
    replace(formula_terms.begin(), formula_terms.end(), ':', ' ');

    string buf;                     // Have a buffer string
    stringstream ss(formula_terms); // Insert the string into a stream

    vector<string> tokens; // Create vector to hold our words

    while (ss >> buf) {
        tokens.push_back(buf);
    }

    if (tokens.size() % 2)
        return (false);

    for (int i = 0; i < tokens.size(); i += 2) {
        parsed_terms[tokens[i]] = tokens[i + 1];
    }
    return (true);
}

#define BLOCKSIZE 256

void Transpose(const float *a, float *b, int p1, int m1, int s1, int p2, int m2, int s2) {
    int I1, I2;
    int i1, i2;
    int q, r;
    const int block = BLOCKSIZE;
    for (I2 = p2; I2 < p2 + m2; I2 += block)
        for (I1 = p1; I1 < p1 + m1; I1 += block)
            for (i2 = I2; i2 < min(I2 + block, p2 + m2); i2++)
                for (i1 = I1; i1 < min(I1 + block, p1 + m1); i1++) {
                    q = i2 * s1 + i1;
                    r = i1 * s2 + i2;
                    b[r] = a[q];
                }
}

void Transpose(const float *a, float *b, int s1, int s2) { Transpose(a, b, 0, s1, s1, 0, s2, s2); }

// Transpose a 1D, 2D, or 3D array. For 1D 'a' is simply copied
// to 'b'. Otherwise 'b' contains a permuted version of 'a' as follows:
//
//    axis        1D        2D        3D
//    ----        --        --        --
//    0	          (0)       (0,1)     (0,1,2)
//    1	          N/A       (1,0)     (1,0,2)
//    2	          N/A       N/A       (2,0,1)
//
// where the numbers in parenthesis indicate the permutation of the
// axes.
//
// NOTE: The contents of 'a' are overwritten
//
void transpose(float *a, float *b, vector<size_t> inDims, int axis) {

    assert(inDims.size() < 4);
    assert(axis >= 0 && axis < inDims.size());

    size_t sz = vproduct(inDims);

    // No-op if axis is 0
    //
    if (axis == 0) { // 1D, 2D, and 3D case
        for (size_t i = 0; i < sz; i++) {
            b[i] = a[i];
        }
        return;
    }

    if (inDims.size() == 2) {
        assert(axis == 1);

        Transpose(a, b, inDims[0], inDims[1]);
    } else if (inDims.size() == 3) {
        assert(axis == 1 || axis == 2);

        size_t stride = inDims[0] * inDims[1];
        ;
        const float *aptr = a;
        float *bptr = b;
        for (size_t i = 0; i < inDims[2]; i++) {
            Transpose(aptr, bptr, inDims[0], inDims[1]);
            aptr += stride;
            bptr += stride;
        }

        // For (2,1,0) permutation we do (0,1,2) -> (1,0,2) -> (2,1,0)
        //
        if (axis == 2) {

            // We can treat 3D array as 2D in this case, linearizing X and Y
            //
            Transpose(b, a, inDims[0] * inDims[1], inDims[2]);

            // Ugh need to copy data from a back to b
            //
            for (size_t i = 0; i < vproduct(inDims); i++) {
                b[i] = a[i];
            }
        }
    }
}

void transpose(vector<size_t> inDims, int axis, vector<size_t> &outDims) {
    outDims = inDims;

    if (axis == 1) {
        size_t tmp = outDims[0];
        outDims[0] = outDims[1];
        outDims[1] = tmp;
    } else if (axis == 2) {
        size_t tmp = outDims[0];
        outDims[0] = outDims[2];
        outDims[2] = tmp;
    }
}

void resampleToStaggered(float *src, const vector<size_t> &inMin, const vector<size_t> &inMax,
                         float *dst, const vector<size_t> &outMin, const vector<size_t> &outMax,
                         int stagDim) {
    assert(inMin.size() == inMax.size());
    assert(inMin.size() == outMax.size());
    assert(inMin.size() == outMax.size());

    vector<size_t> inDims, outDims;
    for (size_t i = 0; i < outMin.size(); i++) {
        inDims.push_back(inMax[i] - inMin[i] + 1);
        outDims.push_back(outMax[i] - outMin[i] + 1);
    }
    size_t sz = std::max(vproduct(outDims), vproduct(inDims));

    float *buf = new float[sz];

    // Tranpose the dimensions and array so that we always interpolate
    // with unit stride
    //
    vector<size_t> inDimsT;  // transposed input dimensions
    vector<size_t> outDimsT; // transposed output dimensions
    transpose(inDims, stagDim, inDimsT);
    transpose(outDims, stagDim, outDimsT);

    transpose(src, buf, inDims, stagDim);

    size_t nz = inDimsT.size() >= 3 ? inDimsT[2] : 1;
    size_t ny = inDimsT.size() >= 2 ? inDimsT[1] : 1;
    size_t nx = inDimsT.size() >= 1 ? inDimsT[0] : 1;

    // Interpolate interior
    //
    size_t nxs = outDimsT[0]; // staggered dimension
    size_t i0 = outMin[stagDim] > inMin[stagDim] ? 0 : 1;

    for (size_t k = 0; k < nz; k++) {
        for (size_t j = 0; j < ny; j++) {
            for (size_t i = 0, ii = i0; i < nx - 1; i++, ii++) {
                src[k * nxs * ny + j * nxs + ii] =
                    0.5 * (buf[k * nx * ny + j * nx + i] + buf[k * nx * ny + j * nx + i + 1]);
            }
        }
    }

    // Next extrapolate boundary points if needed
    //
    // left boundary
    //
    if (outMin[stagDim] <= inMin[stagDim]) {
        if (inMin[stagDim] < inMax[stagDim]) {
            for (size_t k = 0; k < nz; k++) {
                for (size_t j = 0; j < ny; j++) {

                    src[k * nxs * ny + j * nxs] =
                        buf[k * nx * ny + j * nx + 0] +
                        (-0.5 * (buf[k * nx * ny + j * nx + 1] - buf[k * nx * ny + j * nx + 0]));
                }
            }
        } else {
            for (size_t k = 0; k < nz; k++) {
                for (size_t j = 0; j < ny; j++) {
                    src[k * nxs * ny + j * nxs] = buf[k * nx * ny + j * nx + 0];
                }
            }
        }
    }

    // right boundary
    //
    if (outMax[stagDim] > inMax[stagDim]) {
        if (inMin[stagDim] < inMax[stagDim]) {
            for (size_t k = 0; k < nz; k++) {
                for (size_t j = 0; j < ny; j++) {

                    src[k * nxs * ny + j * nxs + nxs - 1] =
                        buf[k * nx * ny + j * nx + nx - 1] +
                        (0.5 *
                         (buf[k * nx * ny + j * nx + nx - 1] - buf[k * nx * ny + j * nx + nx - 2]));
                }
            }
        } else {
            for (size_t k = 0; k < nz; k++) {
                for (size_t j = 0; j < ny; j++) {
                    src[k * nxs * ny + j * nxs + nxs - 1] = buf[k * nx * ny + j * nx + nx - 1];
                }
            }
        }
    }

    // Undo tranpose
    //
    transpose(src, dst, outDimsT, stagDim);

    delete[] buf;
}

void resampleToUnStaggered(float *src, const vector<size_t> &inMin, const vector<size_t> &inMax,
                           float *dst, const vector<size_t> &outMin, const vector<size_t> &outMax,
                           int stagDim) {
    assert(inMin.size() == inMax.size());
    assert(inMin.size() == outMax.size());
    assert(inMin.size() == outMax.size());

    vector<size_t> myOutMax = outMax;
    vector<size_t> myOutMin = outMin;

    myOutMin[stagDim] += 1;
    myOutMax[stagDim] += 1;

    resampleToStaggered(src, inMin, inMax, dst, myOutMin, myOutMax, stagDim);
}

#ifdef UNIT_TEST

void print_matrix(const float *a, const vector<size_t> &dims) {

    size_t nz = dims.size() >= 3 ? dims[2] : 1;
    size_t ny = dims.size() >= 2 ? dims[1] : 1;
    size_t nx = dims.size() >= 1 ? dims[0] : 1;

    for (int k = 0; k < nz; k++) {
        for (int j = 0; j < ny; j++) {
            for (int i = 0; i < nx; i++) {
                cout << a[k * nx * ny + j * nx + i] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

void test_resample(int stagDim) {

    vector<size_t> inMin = {0, 0, 0};
    vector<size_t> inMax = {1, 2, 3};

    vector<size_t> outMin = inMin;
    vector<size_t> outMax = inMax;

    outMax[stagDim] += 1;

    vector<size_t> inDims, outDims;
    for (int i = 0; i < inMax.size(); i++) {
        inDims.push_back(inMax[i] - inMin[i] + 1);
        outDims.push_back(outMax[i] - outMin[i] + 1);
    }

    size_t nz = inDims.size() >= 3 ? inDims[2] : 1;
    size_t ny = inDims.size() >= 2 ? inDims[1] : 1;
    size_t nx = inDims.size() >= 1 ? inDims[0] : 1;

    size_t nzs = outDims.size() >= 3 ? outDims[2] : 1;
    size_t nys = outDims.size() >= 2 ? outDims[1] : 1;
    size_t nxs = outDims.size() >= 1 ? outDims[0] : 1;

    size_t sz = std::max(vproduct(outDims), vproduct(inDims));
    float *src = new float[sz];
    float *dst = new float[sz];

    for (int k = 0; k < nz; k++) {
        for (int j = 0; j < ny; j++) {
            for (int i = 0; i < nx; i++) {
                src[k * nx * ny + j * nx + i] = k * nx * ny + j * nx + i;
            }
        }
    }

    for (int k = 0; k < nzs; k++) {
        for (int j = 0; j < nys; j++) {
            for (int i = 0; i < nxs; i++) {
                dst[k * nxs * nys + j * nxs + i] = 99;
            }
        }
    }

    cout << "original array" << endl;
    print_matrix(src, inDims);

    resampleToStaggered(src, inMin, inMax, dst, outMin, outMax, stagDim);

    cout << endl << endl;

    cout << "staggered array" << endl;
    print_matrix(dst, outDims);

    resampleToUnStaggered(dst, outMin, outMax, src, inMin, inMax, stagDim);

    cout << "reconstructed unstaggered array" << endl;
    print_matrix(src, inDims);
}

int main(int argc, char **argv) {
    assert(argc == 2);
    int stagDim = atoi(argv[1]);
    test_resample(stagDim);
}

#endif

}; // namespace

int DerivedVar::_getVar(DC *dc, size_t ts, string varname, int level, int lod,
                        const vector<size_t> &min, const vector<size_t> &max, float *region) const {

    int fd = dc->OpenVariableRead(ts, varname, level, lod);
    if (fd < 0)
        return (-1);

    int rc = dc->ReadRegion(fd, min, max, region);
    if (rc < 0) {
        dc->CloseVariable(fd);
        return (-1);
    }

    return (dc->CloseVariable(fd));
}

int DerivedVar::_getVarBlock(DC *dc, size_t ts, string varname, int level, int lod,
                             const vector<size_t> &min, const vector<size_t> &max,
                             float *region) const {

    int fd = dc->OpenVariableRead(ts, varname, level, lod);
    if (fd < 0)
        return (-1);

    int rc = dc->ReadRegionBlock(fd, min, max, region);
    if (rc < 0) {
        dc->CloseVariable(fd);
        return (-1);
    }

    return (dc->CloseVariable(fd));
}

//////////////////////////////////////////////////////////////////////////////
//
//	DerivedCoordVar_PCSFromLatLon
//
//////////////////////////////////////////////////////////////////////////////

DerivedCoordVar_PCSFromLatLon::DerivedCoordVar_PCSFromLatLon(string derivedVarName, DC *dc,
                                                             vector<string> inNames,
                                                             string proj4String, bool uGridFlag,
                                                             bool lonFlag)
    : DerivedCoordVar(derivedVarName) {

    assert(inNames.size() == 2);

    _dc = dc;
    _proj4String = proj4String;
    _lonName = inNames[0];
    _latName = inNames[1];
    _make2DFlag = false;
    _uGridFlag = uGridFlag;
    _lonFlag = lonFlag;
    _dimLens.clear();
}

int DerivedCoordVar_PCSFromLatLon::Initialize() {

    int rc = _proj4API.Initialize("", _proj4String);
    if (rc < 0) {
        SetErrMsg("Invalid map projection : %s", _proj4String.c_str());
        return (-1);
    }

    rc = _setupVar();
    if (rc < 0)
        return (-1);

    return (0);
}

bool DerivedCoordVar_PCSFromLatLon::GetBaseVarInfo(DC::BaseVar &var) const {
    var = _coordVarInfo;
    return (true);
}

bool DerivedCoordVar_PCSFromLatLon::GetCoordVarInfo(DC::CoordVar &cvar) const {
    cvar = _coordVarInfo;
    return (true);
}

int DerivedCoordVar_PCSFromLatLon::GetDimLensAtLevel(int, std::vector<size_t> &dims_at_level,
                                                     std::vector<size_t> &bs_at_level) const {
    dims_at_level.clear();
    bs_at_level.clear();

    dims_at_level = _dimLens;
    bs_at_level = _bs;

    return (0);
}

int DerivedCoordVar_PCSFromLatLon::OpenVariableRead(size_t ts, int, int) {

    DC::FileTable::FileObject *f = new DC::FileTable::FileObject(ts, _derivedVarName, -1, -1);

    return (_fileTable.AddEntry(f));
}

int DerivedCoordVar_PCSFromLatLon::CloseVariable(int fd) {
    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }

    _fileTable.RemoveEntry(fd);
    delete f;
    return (0);
}

int DerivedCoordVar_PCSFromLatLon::_readRegionBlockHelper1D(DC::FileTable::FileObject *f,
                                                            const vector<size_t> &min,
                                                            const vector<size_t> &max,
                                                            float *region) {

    size_t ts = f->GetTS();
    string varname = f->GetVarname();
    int level = f->GetLevel();
    int lod = f->GetLOD();

    // Dimensions are same for X & Y coord vars
    //
    vector<size_t> dummy, bs;
    GetDimLensAtLevel(level, dummy, bs);

    // Need temporary buffer space for the X or Y coordinate
    // NOT being returned (we still need to calculate it)
    //
    size_t nElements = numBlocks(min, max, bs) * blockSize(bs);
    float *buf = new float[nElements];

    vector<size_t> roidims;
    for (int i = 0; i < min.size(); i++) {
        roidims.push_back(max[i] - min[i] + 1);
    }

    // Assign temporary buffer 'buf' as appropriate
    //
    float *lonBufPtr;
    float *latBufPtr;
    if (_lonFlag) {
        lonBufPtr = buf;
        latBufPtr = region;
    } else {
        lonBufPtr = region;
        latBufPtr = buf;
    }

    // Reading 1D data so no blocking
    //
    vector<size_t> lonMin = {min[0]};
    vector<size_t> lonMax = {max[0]};
    int rc = _getVar(_dc, ts, _lonName, level, lod, lonMin, lonMax, lonBufPtr);
    if (rc < 0) {
        delete[] buf;
        return (rc);
    }

    vector<size_t> latMin = {min[1]};
    vector<size_t> latMax = {max[1]};
    rc = _getVar(_dc, ts, _latName, level, lod, latMin, latMax, latBufPtr);
    if (rc < 0) {
        delete[] buf;
        return (rc);
    }

    // Combine the 2 1D arrays into a 2D array
    //
    make2D(lonBufPtr, latBufPtr, roidims);

    rc = _proj4API.Transform(lonBufPtr, latBufPtr, vproduct(roidims));

    // Finally, block the data since the original 1D data is not blocked
    // (and make2D doesn't add blocking)
    //
    if (_lonFlag) {
        blockit(lonBufPtr, roidims, bs, region);
    } else {
        blockit(latBufPtr, roidims, bs, region);
    }

    delete[] buf;

    return (rc);
}

int DerivedCoordVar_PCSFromLatLon::_readRegionBlockHelper2D(DC::FileTable::FileObject *f,
                                                            const vector<size_t> &min,
                                                            const vector<size_t> &max,
                                                            float *region) {

    size_t ts = f->GetTS();
    string varname = f->GetVarname();
    int level = f->GetLevel();
    int lod = f->GetLOD();

    // Dimensions are same for X & Y coord vars
    //
    vector<size_t> dummy, bs;
    GetDimLensAtLevel(level, dummy, bs);

    // Need temporary buffer space for the X or Y coordinate
    // NOT being returned (we still need to calculate it)
    //
    size_t nElements = numBlocks(min, max, bs) * blockSize(bs);
    float *buf = new float[nElements];

    // Assign temporary buffer 'buf' as appropriate
    //
    float *lonBufPtr;
    float *latBufPtr;
    if (_lonFlag) {
        lonBufPtr = region;
        latBufPtr = buf;
    } else {
        lonBufPtr = buf;
        latBufPtr = region;
    }

    int rc = _getVarBlock(_dc, ts, _lonName, level, lod, min, max, lonBufPtr);
    if (rc < 0) {
        delete[] buf;
        return (rc);
    }

    rc = _getVarBlock(_dc, ts, _latName, level, lod, min, max, latBufPtr);
    if (rc < 0) {
        delete[] buf;
        return (rc);
    }

    rc = _proj4API.Transform(lonBufPtr, latBufPtr, nElements);

    delete[] buf;

    return (rc);
}

int DerivedCoordVar_PCSFromLatLon::ReadRegionBlock(int fd, const vector<size_t> &min,
                                                   const vector<size_t> &max, float *region) {
    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor: %d", fd);
        return (-1);
    }
    if (_make2DFlag) {
        return (_readRegionBlockHelper1D(f, min, max, region));
    } else {
        return (_readRegionBlockHelper2D(f, min, max, region));
    }
}

bool DerivedCoordVar_PCSFromLatLon::VariableExists(size_t ts, int, int) const {

    return (_dc->VariableExists(ts, _lonName, -1, -1) && _dc->VariableExists(ts, _latName, -1, -1));
}

int DerivedCoordVar_PCSFromLatLon::_setupVar() {

    DC::CoordVar lonVar;
    bool ok = _dc->GetCoordVarInfo(_lonName, lonVar);
    if (!ok)
        return (-1);

    DC::CoordVar latVar;
    ok = _dc->GetCoordVarInfo(_latName, latVar);
    if (!ok)
        return (-1);

    vector<size_t> lonDims;
    ok = _dc->GetVarDimLens(_lonName, true, lonDims);
    if (!ok) {
        SetErrMsg("GetVarDimLens(%s) failed", _lonName.c_str());
        return (-1);
    }

    vector<size_t> latDims;
    ok = _dc->GetVarDimLens(_latName, true, latDims);
    if (!ok) {
        SetErrMsg("GetVarDimLens(%s) failed", _lonName.c_str());
        return (-1);
    }

    if (lonDims.size() != latDims.size()) {
        SetErrMsg("Incompatible block size");
        return (-1);
    }

    vector<string> dimNames;
    if (lonVar.GetDimNames().size() == 1 && !_uGridFlag) {
        dimNames.push_back(lonVar.GetDimNames()[0]);
        dimNames.push_back(latVar.GetDimNames()[0]);
        _dimLens.push_back(lonDims[0]);
        _dimLens.push_back(latDims[0]);
        _make2DFlag = true;
    } else if (lonVar.GetDimNames().size() == 2 && !_uGridFlag) {
        if (lonDims[0] != latDims[0] && lonDims[1] != latDims[1]) {
            SetErrMsg("Incompatible dimensions ");
            return (-1);
        }
        dimNames.push_back(lonVar.GetDimNames()[0]);
        dimNames.push_back(lonVar.GetDimNames()[1]);
        _dimLens.push_back(lonDims[0]);
        _dimLens.push_back(lonDims[1]);
        _make2DFlag = false;
    } else {
        assert(lonVar.GetDimNames().size() == 1 && _uGridFlag);
        dimNames = lonVar.GetDimNames();
        _dimLens = lonDims;
    }

    // Use blocking or no?
    //
    _bs = _dc->GetBlockSize();
    if (_bs.empty()) {
        _bs = _dimLens;
    }
    while (_bs.size() > _dimLens.size())
        _bs.pop_back();

    if (lonVar.GetTimeDimName() != latVar.GetTimeDimName()) {
        SetErrMsg("Incompatible time dimensions");
        return (-1);
    }
    string timeDimName = lonVar.GetTimeDimName();

    DC::XType xtype = lonVar.GetXType();
    vector<bool> periodic = lonVar.GetPeriodic();

    _coordVarInfo.SetName(_derivedVarName);
    _coordVarInfo.SetUnits("meters");
    _coordVarInfo.SetXType(xtype);
    _coordVarInfo.SetWName("");
    _coordVarInfo.SetCRatios(vector<size_t>());
    _coordVarInfo.SetPeriodic(periodic);
    _coordVarInfo.SetUniform(false);

    _coordVarInfo.SetDimNames(dimNames);
    _coordVarInfo.SetTimeDimName(timeDimName);

    _coordVarInfo.SetAxis(_lonFlag ? 0 : 1);

    return (0);
}

//////////////////////////////////////////////////////////////////////////////
//
//	DerivedCoordVar_CF1D
//
//////////////////////////////////////////////////////////////////////////////

DerivedCoordVar_CF1D::DerivedCoordVar_CF1D(string derivedVarName, DC *dc, string dimName, int axis,
                                           string units)
    : DerivedCoordVar(derivedVarName) {

    _dc = dc;
    _dimName = dimName;
    _dimLen = 0;

    _coordVarInfo = DC::CoordVar(_derivedVarName, units, DC::XType::FLOAT, vector<bool>(1, false),
                                 axis, false, vector<string>(1, dimName), "");
}

int DerivedCoordVar_CF1D::Initialize() {

    DC::Dimension dimension;
    int rc = _dc->GetDimension(_dimName, dimension);
    if (rc < 0)
        return (-1);

    _dimLen = dimension.GetLength();

    return (0);
}

bool DerivedCoordVar_CF1D::GetBaseVarInfo(DC::BaseVar &var) const {
    var = _coordVarInfo;
    return (true);
}

bool DerivedCoordVar_CF1D::GetCoordVarInfo(DC::CoordVar &cvar) const {
    cvar = _coordVarInfo;
    return (true);
}

int DerivedCoordVar_CF1D::GetDimLensAtLevel(int level, std::vector<size_t> &dims_at_level,
                                            std::vector<size_t> &bs_at_level) const {
    dims_at_level.clear();
    bs_at_level.clear();

    dims_at_level.push_back(_dimLen);
    bs_at_level = _dc->GetBlockSize();
    if (bs_at_level.empty()) {
        bs_at_level = dims_at_level;
    }
    while (bs_at_level.size() > dims_at_level.size())
        bs_at_level.pop_back();

    return (0);
}

int DerivedCoordVar_CF1D::OpenVariableRead(size_t ts, int level, int lod) {
    if (level != 0) {
        SetErrMsg("Invalid parameter");
        return (-1);
    }

    if (lod != 0) {
        SetErrMsg("Invalid parameter");
        return (-1);
    }

    DC::FileTable::FileObject *f = new DC::FileTable::FileObject(ts, _derivedVarName, level, lod);

    return (_fileTable.AddEntry(f));

    return (0);
}

int DerivedCoordVar_CF1D::CloseVariable(int fd) {

    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }

    _fileTable.RemoveEntry(fd);
    delete f;

    return (0);
}

int DerivedCoordVar_CF1D::ReadRegionBlock(int fd, const vector<size_t> &min,
                                          const vector<size_t> &max, float *region) {
    assert(min.size() == 1);
    assert(max.size() == 1);

    float *regptr = region;
    for (size_t i = min[0]; i <= max[0]; i++) {
        *regptr++ = (float)i;
    }

    return (0);
}

bool DerivedCoordVar_CF1D::VariableExists(size_t ts, int reflevel, int lod) const {
    if (reflevel != 0)
        return (false);
    if (lod != 0)
        return (false);

    return (true);
}

//////////////////////////////////////////////////////////////////////////////
//
//	DerivedCoordVar_WRFTime
//
//////////////////////////////////////////////////////////////////////////////

DerivedCoordVar_WRFTime::DerivedCoordVar_WRFTime(string derivedVarName, NetCDFCollection *ncdfc,
                                                 string wrfTimeVar, string dimName, float p2si)
    : DerivedCoordVar(derivedVarName) {

    _ncdfc = ncdfc;
    _times.clear();
    _timePerm.clear();
    _wrfTimeVar = wrfTimeVar;
    _p2si = p2si;
    _ovr_ts = 0;

    string units = "seconds";
    int axis = 3;
    _coordVarInfo = DC::CoordVar(_derivedVarName, units, DC::XType::FLOAT, vector<bool>(), axis,
                                 false, vector<string>(), dimName);
}

int DerivedCoordVar_WRFTime::Initialize() {

    // Use UDUnits for unit conversion
    //
    UDUnits udunits;
    int rc = udunits.Initialize();
    if (rc < 0) {
        SetErrMsg("Failed to initialize udunits2 library : %s", udunits.GetErrMsg().c_str());
        return (-1);
    }

    size_t numTS = _ncdfc->GetNumTimeSteps();

    vector<size_t> dims = _ncdfc->GetSpatialDims(_wrfTimeVar);

    char *buf = new char[dims[0] + 1];
    buf[dims[0]] = '\0'; // Null terminate

    const char *format = "%4d-%2d-%2d_%2d:%2d:%2d";

    // Read all of the formatted time strings up front - it's a 1D array
    // so we can simply store the results in memory - and convert from
    // a formatted time string to seconds since the EPOCH
    //
    _times.clear();
    for (size_t ts = 0; ts < numTS; ts++) {

        int fd = _ncdfc->OpenRead(ts, _wrfTimeVar);
        if (fd < 0) {
            SetErrMsg("Can't read time variable");
            return (-1);
        }

        int rc = _ncdfc->Read(buf, fd);
        if (rc < 0) {
            SetErrMsg("Can't read time variable");
            _ncdfc->Close(fd);
            return (-1);
        }
        _ncdfc->Close(fd);

        int year, mon, mday, hour, min, sec;
        rc = sscanf(buf, format, &year, &mon, &mday, &hour, &min, &sec);
        if (rc != 6) {
            rc = sscanf(buf, "%4d-%5d_%2d:%2d:%2d", &year, &mday, &hour, &min, &sec);
            mon = 1;
            if (rc != 5) {
                SetErrMsg("Unrecognized time stamp: %s", buf);
                return (-1);
            }
        }

        _times.push_back(udunits.EncodeTime(year, mon, mday, hour, min, sec) * _p2si);
    }
    delete[] buf;

    // The NetCDFCollection class doesn't handle the WRF time
    // variable. Hence, the time steps aren't sorted. Sort them now and
    // create a lookup table to map a time index to the correct time step
    // in the WRF data collection. N.B. this is only necessary if multiple
    // WRF files are present and they're not passed to Initialize() in
    // the correct order.
    //

    _timePerm.clear();
    for (int i = 0; i != _times.size(); i++) {
        _timePerm.push_back(i);
    }
    sort(_timePerm.begin(), _timePerm.end(),
         [&](const int &a, const int &b) { return (_times[a] < _times[b]); });

    return (0);
}

bool DerivedCoordVar_WRFTime::GetBaseVarInfo(DC::BaseVar &var) const {
    var = _coordVarInfo;
    return (true);
}

bool DerivedCoordVar_WRFTime::GetCoordVarInfo(DC::CoordVar &cvar) const {
    cvar = _coordVarInfo;
    return (true);
}

int DerivedCoordVar_WRFTime::GetDimLensAtLevel(int level, std::vector<size_t> &dims_at_level,
                                               std::vector<size_t> &bs_at_level) const {
    dims_at_level.clear();
    bs_at_level.clear();

    return (0);
}

int DerivedCoordVar_WRFTime::OpenVariableRead(size_t ts, int level, int lod) {
    ts = ts < _times.size() ? ts : _times.size() - 1;

    DC::FileTable::FileObject *f = new DC::FileTable::FileObject(ts, _derivedVarName, level, lod);

    return (_fileTable.AddEntry(f));
}

int DerivedCoordVar_WRFTime::CloseVariable(int fd) {
    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }

    _fileTable.RemoveEntry(fd);
    delete f;

    return (0);
}

int DerivedCoordVar_WRFTime::ReadRegionBlock(int fd, const vector<size_t> &min,
                                             const vector<size_t> &max, float *region) {
    assert(min.size() == 0);
    assert(max.size() == 0);

    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor: %d", fd);
        return (-1);
    }

    size_t ts = f->GetTS();

    *region = _times[ts];

    return (0);
}

bool DerivedCoordVar_WRFTime::VariableExists(size_t ts, int reflevel, int lod) const {

    return (ts < _times.size());
}

//////////////////////////////////////////////////////////////////////////////
//
//	DerivedCoordVar_TimeInSeconds
//
//////////////////////////////////////////////////////////////////////////////

DerivedCoordVar_TimeInSeconds::DerivedCoordVar_TimeInSeconds(string derivedVarName, DC *dc,
                                                             string nativeTimeVar, string dimName)
    : DerivedCoordVar(derivedVarName) {

    _dc = dc;
    _times.clear();
    _nativeTimeVar = nativeTimeVar;

    string units = "seconds";
    int axis = 3;
    _coordVarInfo = DC::CoordVar(_derivedVarName, units, DC::XType::FLOAT, vector<bool>(), axis,
                                 false, vector<string>(), dimName);
}

int DerivedCoordVar_TimeInSeconds::Initialize() {

    // Use UDUnits for unit conversion
    //
    UDUnits udunits;
    int rc = udunits.Initialize();
    if (rc < 0) {
        SetErrMsg("Failed to initialize udunits2 library : %s", udunits.GetErrMsg().c_str());
        return (-1);
    }

    DC::CoordVar cvar;
    bool status = _dc->GetCoordVarInfo(_nativeTimeVar, cvar);
    if (!status) {
        SetErrMsg("Invalid coordinate variable %s", _nativeTimeVar.c_str());
        return (-1);
    }

    if (!udunits.IsTimeUnit(cvar.GetUnits())) {
        SetErrMsg("Invalid coordinate variable %s", _nativeTimeVar.c_str());
        return (-1);
    }

    size_t numTS = _dc->GetNumTimeSteps(_nativeTimeVar);

    float *buf = new float[2 * numTS];
    float *bufptr1 = buf;
    float *bufptr2 = buf + numTS;

    rc = _dc->GetVar(_nativeTimeVar, -1, -1, bufptr1);
    if (rc < 0) {
        SetErrMsg("Can't read time variable");
        return (-1);
    }

    status = udunits.Convert(cvar.GetUnits(), "seconds", bufptr1, bufptr2, numTS);
    if (!status) {
        SetErrMsg("Invalid coordinate variable %s", _nativeTimeVar.c_str());
        return (-1);
    }

    _times.clear();
    for (int i = 0; i < numTS; i++) {
        _times.push_back(bufptr2[i]);
    }
    delete[] buf;

    return (0);
}

bool DerivedCoordVar_TimeInSeconds::GetBaseVarInfo(DC::BaseVar &var) const {
    var = _coordVarInfo;
    return (true);
}

bool DerivedCoordVar_TimeInSeconds::GetCoordVarInfo(DC::CoordVar &cvar) const {
    cvar = _coordVarInfo;
    return (true);
}

int DerivedCoordVar_TimeInSeconds::GetDimLensAtLevel(int level, std::vector<size_t> &dims_at_level,
                                                     std::vector<size_t> &bs_at_level) const {
    dims_at_level.clear();
    bs_at_level.clear();

    return (0);
}

int DerivedCoordVar_TimeInSeconds::OpenVariableRead(size_t ts, int level, int lod) {
    ts = ts < _times.size() ? ts : _times.size() - 1;

    DC::FileTable::FileObject *f = new DC::FileTable::FileObject(ts, _derivedVarName, level, lod);

    return (_fileTable.AddEntry(f));
}

int DerivedCoordVar_TimeInSeconds::CloseVariable(int fd) {
    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }

    _fileTable.RemoveEntry(fd);
    delete f;

    return (0);
}

int DerivedCoordVar_TimeInSeconds::ReadRegionBlock(int fd, const vector<size_t> &min,
                                                   const vector<size_t> &max, float *region) {
    assert(min.size() == 0);
    assert(max.size() == 0);

    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor: %d", fd);
        return (-1);
    }

    size_t ts = f->GetTS();

    *region = _times[ts];

    return (0);
}

bool DerivedCoordVar_TimeInSeconds::VariableExists(size_t ts, int reflevel, int lod) const {

    return (ts < _times.size());
}

//////////////////////////////////////////////////////////////////////////////
//
//	DerivedCoordVar_Staggered
//
//////////////////////////////////////////////////////////////////////////////

DerivedCoordVar_Staggered::DerivedCoordVar_Staggered(string derivedVarName, string stagDimName,
                                                     DC *dc, string inName, string dimName)
    : DerivedCoordVar(derivedVarName) {

    _stagDimName = stagDimName;
    _inName = inName;
    _dimName = dimName;
    _dc = dc;
}

int DerivedCoordVar_Staggered::Initialize() {

    bool ok = _dc->GetCoordVarInfo(_inName, _coordVarInfo);
    if (!ok)
        return (-1);

    vector<size_t> dims, bs;
    int rc = _dc->GetDimLensAtLevel(_inName, 0, dims, bs);
    if (rc < 0)
        return (-1);

    if (dims != bs) {
        SetErrMsg("Blocked data not supported");
        return (-1);
    }

    vector<string> dimNames = _coordVarInfo.GetDimNames();
    _stagDim = -1;
    for (int i = 0; i < dimNames.size(); i++) {
        if (dimNames[i] == _dimName) {
            _stagDim = i;
            dimNames[i] = _stagDimName;
            break;
        }
    }
    if (_stagDim < 0) {
        SetErrMsg("Dimension %s not found", _dimName.c_str());
        return (-1);
    }

    // Change the name of the staggered dimension
    //
    _coordVarInfo.SetDimNames(dimNames);

    return (0);
}

bool DerivedCoordVar_Staggered::GetBaseVarInfo(DC::BaseVar &var) const {
    var = _coordVarInfo;
    return (true);
}

bool DerivedCoordVar_Staggered::GetCoordVarInfo(DC::CoordVar &cvar) const {
    cvar = _coordVarInfo;
    return (true);
}

int DerivedCoordVar_Staggered::GetDimLensAtLevel(int level, std::vector<size_t> &dims_at_level,
                                                 std::vector<size_t> &bs_at_level) const {
    dims_at_level.clear();
    bs_at_level.clear();

    int rc = _dc->GetDimLensAtLevel(_inName, level, dims_at_level, bs_at_level);
    if (rc < 0)
        return (-1);

    dims_at_level[_stagDim] += 1;

    return (0);
}

int DerivedCoordVar_Staggered::OpenVariableRead(size_t ts, int level, int lod) {

    int fd = _dc->OpenVariableRead(ts, _inName, level, lod);
    if (fd < 0)
        return (fd);

    DC::FileTable::FileObject *f =
        new DC::FileTable::FileObject(ts, _derivedVarName, level, lod, fd);

    return (_fileTable.AddEntry(f));
}

int DerivedCoordVar_Staggered::CloseVariable(int fd) {
    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }
    int rc = _dc->CloseVariable(f->GetAux());

    _fileTable.RemoveEntry(fd);
    delete f;

    return (rc);
}

int DerivedCoordVar_Staggered::ReadRegion(int fd, const vector<size_t> &min,
                                          const vector<size_t> &max, float *region) {

    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }
    int level = f->GetLevel();

    vector<size_t> dims, bs;
    int rc = GetDimLensAtLevel(level, dims, bs);
    if (rc < 0)
        return (-1);

    vector<size_t> inMin = min;
    vector<size_t> inMax = max;

    // adjust coords for native data so that we have what we
    // need for interpolation or extrapolation.
    //

    // Adjust min max boundaries to handle 4 case (below) where X's
    // are samples on the destination grid (staggered) and O's are samples
    // on the source grid (unstaggered) and the numbers represent
    // the address of the samples in their respective arrays
    //
    //	X O X O X O X
    //	0 0 1 1 2 2 3
    //
    //	  O X O X O X
    //	  0 1 1 2 2 3
    //
    //	X O X O X O
    //	0 0 1 1 2 2
    //
    //	  O X O X O
    //	  0 1 1 2 2

    // Adjust input min so we can interpolate interior.
    //
    if (min[_stagDim] > 0) {
        inMin[_stagDim] -= 1;
    }

    // input dimensions are one less then output
    //
    if (max[_stagDim] >= (dims[_stagDim] - 1)) {
        inMax[_stagDim] -= 1;
    }

    // Adjust min and max for edge cases
    //
    if (max[_stagDim] == 0 && (dims[_stagDim] - 1) > 1) {
        inMax[_stagDim] += 1;
    }
    if (min[_stagDim] == dims[_stagDim] - 1 && min[_stagDim] > 0) {
        inMin[_stagDim] -= 1;
    }

    vector<size_t> inDims, outDims;
    for (size_t i = 0; i < min.size(); i++) {
        inDims.push_back(inMax[i] - inMin[i] + 1);
        outDims.push_back(max[i] - min[i] + 1);
    }
    size_t sz = std::max(vproduct(outDims), vproduct(inDims));

    float *buf = new float[sz];

    // Read unstaggered data
    //
    rc = _dc->ReadRegion(f->GetAux(), inMin, inMax, buf);
    if (rc < 0)
        return (-1);

    resampleToStaggered(buf, inMin, inMax, region, min, max, _stagDim);

    delete[] buf;

    return (0);
}

bool DerivedCoordVar_Staggered::VariableExists(size_t ts, int reflevel, int lod) const {

    return (_dc->VariableExists(ts, _inName, reflevel, lod));
}

//////////////////////////////////////////////////////////////////////////////
//
//	DerivedCoordVar_UnStaggered
//
//////////////////////////////////////////////////////////////////////////////

DerivedCoordVar_UnStaggered::DerivedCoordVar_UnStaggered(string derivedVarName,
                                                         string unstagDimName, DC *dc,
                                                         string inName, string dimName)
    : DerivedCoordVar(derivedVarName) {

    _unstagDimName = unstagDimName;
    _inName = inName;
    _dimName = dimName;
    _dc = dc;
}

int DerivedCoordVar_UnStaggered::Initialize() {

    bool ok = _dc->GetCoordVarInfo(_inName, _coordVarInfo);
    if (!ok)
        return (-1);

    vector<size_t> dims, bs;
    int rc = _dc->GetDimLensAtLevel(_inName, 0, dims, bs);
    if (rc < 0)
        return (-1);

    if (dims != bs) {
        SetErrMsg("Blocked data not supported");
        return (-1);
    }

    vector<string> dimNames = _coordVarInfo.GetDimNames();
    _stagDim = -1;
    for (int i = 0; i < dimNames.size(); i++) {
        if (dimNames[i] == _dimName) {
            _stagDim = i;
            dimNames[i] = _unstagDimName;
            break;
        }
    }
    if (_stagDim < 0) {
        SetErrMsg("Dimension %s not found", _dimName.c_str());
        return (-1);
    }

    // Change the name of the staggered dimension
    //
    _coordVarInfo.SetDimNames(dimNames);

    return (0);
}

bool DerivedCoordVar_UnStaggered::GetBaseVarInfo(DC::BaseVar &var) const {
    var = _coordVarInfo;
    return (true);
}

bool DerivedCoordVar_UnStaggered::GetCoordVarInfo(DC::CoordVar &cvar) const {
    cvar = _coordVarInfo;
    return (true);
}

int DerivedCoordVar_UnStaggered::GetDimLensAtLevel(int level, std::vector<size_t> &dims_at_level,
                                                   std::vector<size_t> &bs_at_level) const {
    dims_at_level.clear();
    bs_at_level.clear();

    int rc = _dc->GetDimLensAtLevel(_inName, level, dims_at_level, bs_at_level);
    if (rc < 0)
        return (-1);

    dims_at_level[_stagDim] -= 1;

    return (0);
}

int DerivedCoordVar_UnStaggered::OpenVariableRead(size_t ts, int level, int lod) {

    int fd = _dc->OpenVariableRead(ts, _inName, level, lod);
    if (fd < 0)
        return (fd);

    DC::FileTable::FileObject *f =
        new DC::FileTable::FileObject(ts, _derivedVarName, level, lod, fd);

    return (_fileTable.AddEntry(f));
}

int DerivedCoordVar_UnStaggered::CloseVariable(int fd) {
    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }
    int rc = _dc->CloseVariable(f->GetAux());

    _fileTable.RemoveEntry(fd);
    delete f;

    return (rc);
}

int DerivedCoordVar_UnStaggered::ReadRegion(int fd, const vector<size_t> &min,
                                            const vector<size_t> &max, float *region) {

    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }
    int level = f->GetLevel();

    vector<size_t> dims, bs;
    int rc = GetDimLensAtLevel(level, dims, bs);
    if (rc < 0)
        return (-1);

    vector<size_t> inMin = min;
    vector<size_t> inMax = max;

    // adjust coords for native data so that we have what we
    // need for interpolation .
    //
    inMax[_stagDim] += 1;

    vector<size_t> inDims, outDims;
    for (size_t i = 0; i < min.size(); i++) {
        inDims.push_back(inMax[i] - inMin[i] + 1);
        outDims.push_back(max[i] - min[i] + 1);
    }
    size_t sz = std::max(vproduct(outDims), vproduct(inDims));

    float *buf = new float[sz];

    // Read staggered data
    //
    rc = _dc->ReadRegion(f->GetAux(), inMin, inMax, buf);
    if (rc < 0)
        return (-1);

    resampleToUnStaggered(buf, inMin, inMax, region, min, max, _stagDim);

    delete[] buf;

    return (0);
}

bool DerivedCoordVar_UnStaggered::VariableExists(size_t ts, int reflevel, int lod) const {

    return (_dc->VariableExists(ts, _inName, reflevel, lod));
}

//////////////////////////////////////////////////////////////////////////////
//
//	DerivedCoordVarStandardWRF_Terrain
//
//////////////////////////////////////////////////////////////////////////////

DerivedCoordVarStandardWRF_Terrain::DerivedCoordVarStandardWRF_Terrain(DC *dc, string mesh,
                                                                       string formula)
    : DerivedCFVertCoordVar("", dc, mesh, formula) {

    _PHVar.clear();
    _PHBVar.clear();
    _grav = 9.80665;
}

int DerivedCoordVarStandardWRF_Terrain::Initialize() {

    map<string, string> formulaMap;
    if (!parse_formula(_formula, formulaMap)) {
        SetErrMsg("Invalid conversion formula \"%s\"", _formula.c_str());
        return (-1);
    }

    map<string, string>::const_iterator itr;
    itr = formulaMap.find("PH");
    if (itr != formulaMap.end()) {
        _PHVar = itr->second;
    }

    itr = formulaMap.find("PHB");
    if (itr != formulaMap.end()) {
        _PHBVar = itr->second;
    }

    if (_PHVar.empty() || _PHBVar.empty()) {
        SetErrMsg("Invalid conversion formula \"%s\"", _formula.c_str());
        return (-1);
    }

    DC::DataVar dvarInfo;
    bool status = _dc->GetDataVarInfo(_PHVar, dvarInfo);
    if (!status) {
        SetErrMsg("Invalid variable \"%s\"", _PHVar.c_str());
        return (-1);
    }

    string timeCoordVar = dvarInfo.GetTimeCoordVar();
    string timeDimName;
    if (!timeCoordVar.empty()) {
        DC::CoordVar cvarInfo;
        bool status = _dc->GetCoordVarInfo(timeCoordVar, cvarInfo);
        if (!status) {
            SetErrMsg("Invalid variable \"%s\"", timeCoordVar.c_str());
            return (-1);
        }
        timeDimName = cvarInfo.GetTimeDimName();
    }

    DC::Mesh m;
    status = _dc->GetMesh(_mesh, m);
    if (!status) {
        SetErrMsg("Invalid mesh \"%s\"", _mesh.c_str());
        return (-1);
    }

    // Elevation variable
    //
    vector<string> dimnames = m.GetDimNames();
    assert(dimnames.size() == 3);
    if (dimnames[0] == "west_east" && dimnames[1] == "south_north" && dimnames[2] == "bottom_top") {
        _derivedVarName = "Elevation";
    } else if (dimnames[0] == "west_east_stag" && dimnames[1] == "south_north" &&
               dimnames[2] == "bottom_top") {
        _derivedVarName = "ElevationU";
    } else if (dimnames[0] == "west_east" && dimnames[1] == "south_north_stag" &&
               dimnames[2] == "bottom_top") {
        _derivedVarName = "ElevationV";
    } else if (dimnames[0] == "west_east" && dimnames[1] == "south_north" &&
               dimnames[2] == "bottom_top_stag") {
        _derivedVarName = "ElevationW";
    } else {
        SetErrMsg("Invalid mesh \"%s\"", _mesh.c_str());
        return (-1);
    }

    _coordVarInfo = DC::CoordVar(_derivedVarName, "m", DC::XType::FLOAT, vector<bool>(3, false), 2,
                                 false, dimnames, timeDimName);

    return (0);
}

bool DerivedCoordVarStandardWRF_Terrain::GetBaseVarInfo(DC::BaseVar &var) const {
    var = _coordVarInfo;
    return (true);
}

bool DerivedCoordVarStandardWRF_Terrain::GetCoordVarInfo(DC::CoordVar &cvar) const {
    cvar = _coordVarInfo;
    return (true);
}

int DerivedCoordVarStandardWRF_Terrain::GetDimLensAtLevel(int level,
                                                          std::vector<size_t> &dims_at_level,
                                                          std::vector<size_t> &bs_at_level) const {
    dims_at_level.clear();
    bs_at_level.clear();

    int rc = _dc->GetDimLensAtLevel(_PHVar, level, dims_at_level, bs_at_level);
    if (rc < 0)
        return (-1);

    bool blocked = dims_at_level != bs_at_level;

    if (_derivedVarName == "Elevation") {
        dims_at_level[2]--;
    } else if (_derivedVarName == "ElevationU") {
        dims_at_level[0]++;
        dims_at_level[2]--;
    } else if (_derivedVarName == "ElevationV") {
        dims_at_level[1]++;
        dims_at_level[2]--;
    } else if (_derivedVarName == "ElevationW") {
    } else {
        SetErrMsg("Invalid variable name: %s", _derivedVarName.c_str());
        return (-1);
    }

    // Ugh. The blocking is invariant for data that are truly blocked (i.e.
    // block size is not the dimension size)
    //
    if (!blocked) {
        bs_at_level = dims_at_level;
    }
    return (0);
}

int DerivedCoordVarStandardWRF_Terrain::OpenVariableRead(size_t ts, int level, int lod) {

    DC::FileTable::FileObject *f = new DC::FileTable::FileObject(ts, _derivedVarName, level, lod);

    return (_fileTable.AddEntry(f));
}

int DerivedCoordVarStandardWRF_Terrain::CloseVariable(int fd) {
    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    if (!f) {
        SetErrMsg("Invalid file descriptor : %d", fd);
        return (-1);
    }

    _fileTable.RemoveEntry(fd);
    delete f;

    return (0);
}

int DerivedCoordVarStandardWRF_Terrain::ReadRegionBlock(int fd, const vector<size_t> &min,
                                                        const vector<size_t> &max, float *region) {
    assert(min.size() == 3);
    assert(min.size() == max.size());

    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);
    int level = f->GetLevel();

    vector<size_t> dims, bs;
    GetDimLensAtLevel(level, dims, bs);

    bool blockFlag = dims != bs;

    // We're going to use an unblocked read to fetch the data, so need to
    // ensure ROI does not include block padding
    //
    vector<size_t> myMax = max;
    for (int i = 0; i < myMax.size(); i++) {
        if (myMax[i] >= dims[i]) {
            myMax[i] = dims[i] - 1;
        }
    }

    vector<size_t> roidims; // region of interest dims
    for (int i = 0; i < min.size(); i++) {
        roidims.push_back(myMax[i] - min[i] + 1);
    }

    if (!blockFlag) {

        // Data actually aren't blocked. Do a normal read
        //
        assert(roidims == dims);
        return (ReadRegion(fd, min, myMax, region));
    } else {
        // Do an unblocked read of the data and then block the results. Too
        // damn hard to do the resampling to a staggered grid with blocked
        // reads
        //
        size_t nElements = numElements(min, myMax);

        float *buf = new float[nElements];

        int rc = ReadRegion(fd, min, myMax, buf);
        if (rc < 0) {
            delete[] buf;
            return (-1);
        }

        blockit(buf, roidims, bs, region);

        delete[] buf;

        return (0);
    }
}

#ifdef DEAD
// Voxel coordinates aligned to block boundaries
//
vector<size_t> bmin = min;
vector<size_t> bmax;
for (int i = 0; i < bmin.size(); i++) {
    bmax.push_back(min[i] + bs[i] - 1);
}

size_t block_size = blockSize(bs);
float *ptr = region;

// Currently don't have resampling code for staggered grids
// that works with blocked data, so we simply read one block
// at a time using non-bricked version of ReadRegion()
//
for (size_t k = 0; k < numBlocks(min[2], max[2], bs[2]); k++) {

    if (bmax[2] >= dims[2])
        bmax[2] = dims[2] - 1;

    for (size_t j = 0; j < numBlocks(min[1], max[1], bs[1]); j++) {

        if (bmax[1] >= dims[1])
            bmax[1] = dims[1] - 1;

        for (size_t i = 0; i < numBlocks(min[0], max[0], bs[0]); i++) {

            if (bmax[0] >= dims[0])
                bmax[0] = dims[0] - 1;

            int rc = ReadRegion(fd, bmin, bmax, ptr);
            if (rc < 0)
                return (-1);

            ptr += block_size;
            bmin[0] += bs[0];
            bmax[0] += bs[0];
        }
        bmin[0] = min[0];
        bmax[0] = min[0] + bs[0] - 1;
        bmin[1] += bs[1];
        bmax[1] += bs[1];
    }
    bmin[1] = min[1];
    bmax[1] = min[1] + bs[1] - 1;
    bmin[2] += bs[2];
    bmax[2] += bs[2];
}

return (0);
}
#endif

int DerivedCoordVarStandardWRF_Terrain::ReadRegion(int fd, const vector<size_t> &min,
                                                   const vector<size_t> &max, float *region) {

    DC::FileTable::FileObject *f = _fileTable.GetEntry(fd);

    string varname = f->GetVarname();

    // Dimensions of "W" grid: PH and PHB variables are sampled on the
    // same grid as the W component of velocity
    //
    vector<size_t> wDims, wBS;
    int rc = _dc->GetDimLensAtLevel(_PHVar, f->GetLevel(), wDims, wBS);
    if (rc < 0)
        return (-1);

    // coordinates of "W" grid.
    //
    vector<size_t> wMin = min;
    vector<size_t> wMax = max;

    // coordinates of base (Elevation) grid.
    //
    if (varname == "Elevation") {
        wMax[2] += 1;
    } else if (varname == "ElevationU") {
        wMax[2] += 1;

        if (min[0] > 0) {
            wMin[0] -= 1;
        }
        if (max[0] >= (wDims[0] - 1)) {
            wMax[0] -= 1;
        }
    } else if (varname == "ElevationV") {
        wMax[2] += 1;

        if (min[1] > 0) {
            wMin[1] -= 1;
        }
        if (max[1] >= (wDims[1] - 1)) {
            wMax[1] -= 1;
        }
    }

    // Base grid dimensions
    //
    vector<size_t> bMin = wMin;
    vector<size_t> bMax = wMax;
    bMax[2] -= 1;

    size_t nElements = std::max(numElements(wMin, wMax), numElements(min, max));

    float *buf1 = new float[nElements];
    rc = _getVar(_dc, f->GetTS(), _PHVar, f->GetLevel(), f->GetLOD(), wMin, wMax, buf1);
    if (rc < 0) {
        delete[] buf1;
        return (rc);
    }

    float *buf2 = new float[nElements];
    rc = _getVar(_dc, f->GetTS(), _PHBVar, f->GetLevel(), f->GetLOD(), wMin, wMax, buf2);
    if (rc < 0) {
        delete[] buf1;
        delete[] buf2;
        return (rc);
    }

    float *dst = region;
    if (varname != "ElevationW") {
        dst = buf1;
    }

    // Compute elevation on the W grid
    //
    for (size_t i = 0; i < nElements; i++) {
        dst[i] = (buf1[i] + buf2[i]) / _grav;
    }

    // Elevation is correct for W grid. If we want Elevation, ElevationU, or
    // Elevation V grid we need to interpolate
    //

    if (varname == "Elevation") {

        // Resample stagged W grid to base grid
        //
        resampleToUnStaggered(buf1, wMin, wMax, region, min, max, 2);
    } else if (varname == "ElevationU") {

        // Resample stagged W grid to base grid
        //
        resampleToUnStaggered(buf1, wMin, wMax, buf2, bMin, bMax, 2);

        resampleToStaggered(buf2, bMin, bMax, region, min, max, 0);
    } else if (varname == "ElevationV") {

        // Resample stagged W grid to base grid
        //
        resampleToUnStaggered(buf1, wMin, wMax, buf2, bMin, bMax, 2);

        resampleToStaggered(buf2, bMin, bMax, region, min, max, 1);
    }

    delete[] buf1;
    delete[] buf2;

    return (0);
}

bool DerivedCoordVarStandardWRF_Terrain::VariableExists(size_t ts, int reflevel, int lod) const {

    return (_dc->VariableExists(ts, _PHVar, reflevel, lod) &&
            _dc->VariableExists(ts, _PHBVar, reflevel, lod));
}
