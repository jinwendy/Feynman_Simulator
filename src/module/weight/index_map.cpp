//
//  index_map.cpp
//  Feynman_Simulator
//
//  Created by Kun Chen on 11/24/14.
//  Copyright (c) 2014 Kun Chen. All rights reserved.
//

#include "index_map.h"
#include "utility/logger.h"
#include <math.h>

using namespace weight;

IndexMap::IndexMap(real Beta, const Lattice &lat)
{
    _Beta = Beta;
    _dBeta = Beta / MAX_TAU_BIN;
    _dBetaInverse = 1.0 / _dBeta;
    _Lat = lat;
}

int IndexMap::SublatIndex(const Distance &dist) const
{
    return dist.SublatIndex;
}

int IndexMap::CoordiIndex(const Distance &dist) const
{
    return dist.CoordiIndex;
}

int IndexMap::TauIndex(real tau) const
{
    if (DEBUGMODE && tau < -_Beta || tau >= _Beta)
        LOG_INFO("tau=" << tau << " is out of the range ["
                        << -_Beta << "," << _Beta << ")");
    //TODO: mapping between tau and bin

    int bin = tau < 0 ? floor(tau * _dBetaInverse) + MAX_TAU_BIN
                      : floor(tau * _dBetaInverse);
    if (DEBUGMODE && bin < 0 || tau >= MAX_TAU_BIN) {
        LOG_INFO("tau=" << tau << " is out of the range ["
                        << -_Beta << "," << _Beta << ")");
        LOG_INFO("bin=" << bin << " is out of the range ["
                        << 0 << "," << MAX_TAU_BIN << "]");
    }
    return bin;
}

int IndexMap::TauIndex(real t_in, real t_out) const
{
    return TauIndex(t_out - t_in);
}

real IndexMap::IndexToTau(int Bin) const
{
    //TODO: mapping between tau and bin
    return Bin * _dBeta + _Beta / 2;
}

int IndexMapSPIN2::SpinIndex(spin SpinIn, spin SpinOut)
{
    return SpinIn * SPIN + SpinOut;
}

bool IndexMapSPIN2::IsSameSpin(int spindex)
{
    return (spindex == 0 || spindex == 2);
}

void IndexMapSPIN2::Map(uint *result, spin SpinIn, spin SpinOut,
                        const Site &rin, const Site &rout, real tin, real tout) const
{
    auto dis = _Lat.Dist(rin, rout);
    result[0] = SpinIndex(SpinIn, SpinOut);
    result[1] = dis.SublatIndex;
    result[2] = dis.CoordiIndex;
    result[3] = TauIndex(tin, tout);
}
void IndexMapSPIN2::MapDeltaT(uint *result, spin SpinIn, spin SpinOut,
                              const Site &rin, const Site &rout) const
{
    auto dis = _Lat.Dist(rin, rout);
    result[0] = SpinIndex(SpinIn, SpinOut);
    result[1] = dis.SublatIndex;
    result[2] = dis.CoordiIndex;
}

//First In/Out: direction of WLine; Second In/Out: direction of Vertex
int IndexMapSPIN4::SpinIndex(spin SpinInIn, spin SpinInOut, spin SpinOutIn, spin SpinOutOut)
{
    return SpinInIn * SPIN3 + SpinInOut * SPIN2 +
           SpinOutIn * SPIN + SpinOutOut;
}
int IndexMapSPIN4::SpinIndex(const spin *TwoSpinIn, const spin *TwoSpinOut)
{
    return SpinIndex(TwoSpinIn[0], TwoSpinIn[1],
                     TwoSpinOut[0], TwoSpinOut[1]);
}

std::vector<int> IndexMapSPIN4::GetSpinIndexVector(SPIN4Filter filter)
{
    vector<int> list;
    for (int InIn = 0; InIn < 2; InIn++)
        for (int InOut = 0; InOut < 2; InOut++)
            for (int OutIn = 0; OutIn < 2; OutIn++)
                for (int OutOut = 0; OutOut < 2; OutOut++) {
                    bool flag = false;
                    if (filter == UpUp2UpUp &&
                        InIn == InOut && InIn == OutIn && InIn == OutOut)
                        flag = true;
                    if (filter == UpDown2UpDown &&
                        InIn == InOut && OutIn == OutOut && InIn == FLIP(OutIn))
                        flag = true;
                    if (filter == UpDown2DownUp &&
                        InIn == FLIP(InOut) && OutIn == FLIP(OutOut) && InIn == FLIP(OutIn))
                        flag = true;
                    if (flag)
                        list.push_back(SpinIndex(spin(InIn), spin(InOut),
                                                 spin(OutIn), spin(OutOut)));
                }
    return list;
}

void IndexMapSPIN4::Map(uint *result, const spin *SpinIn, const spin *SpinOut,
                        const Site &rin, const Site &rout, real tin, real tout) const
{
    auto dis = _Lat.Dist(rin, rout);
    result[0] = SpinIndex(SpinIn, SpinOut);
    result[1] = dis.SublatIndex;
    result[2] = dis.CoordiIndex;
    result[3] = TauIndex(tin, tout);
}
void IndexMapSPIN4::MapDeltaT(uint *result, const spin *SpinIn, const spin *SpinOut,
                              const Site &rin, const Site &rout) const
{
    auto dis = _Lat.Dist(rin, rout);
    result[0] = SpinIndex(SpinIn, SpinOut);
    result[1] = dis.SublatIndex;
    result[2] = dis.CoordiIndex;
}