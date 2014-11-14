//
//  analytic.cpp
//  Feynman_Simulator
//
//  Created by Kun Chen on 11/10/14.
//  Copyright (c) 2014 Kun Chen. All rights reserved.
//

#include "weight_inherit.h"
#include <vector>
using namespace weight;

void G::InitialWithBare()
{
    _InitialBareWeight();
    DeltaTWeight = 0.0;
    SmoothWeight = BareWeight;
}

void G::SetTest()
{
    //Spin independent; dr==0; exp(i*tau)
    SmoothWeight = 0.0;
    int spin_down = SpinIndex(DOWN, DOWN);
    int spin_up = SpinIndex(UP, UP);
    for (int sub = 0; sub < _Shape[SUB]; sub++) {
        if (!_Lat.IsOnSameSubLat(sub))
            continue;
        int coor = _Lat.Vec2Index({0, 0});
        for (int tau = 0; tau < _Shape[TAU]; tau++) {
            Complex weight = exp(Complex(0.0, BinToTau(tau)));
            SmoothWeight[spin_down][sub][coor][tau] = weight;
            SmoothWeight[spin_up][sub][coor][tau] = weight;
        }
    }

    DeltaTWeight = 0.0;
    BareWeight = 0.0;
}

void G::InitialWithDiagCounter()
{
    SmoothWeight = 0.0;
    int spin_down = SpinIndex(DOWN, DOWN);
    int spin_up = SpinIndex(UP, UP);

    for (int sub = 0; sub < _Shape[SUB]; sub++) {
        if (!_Lat.IsOnSameSubLat(sub))
            continue;
        int coor = _Lat.Vec2Index({0, 0});
        for (int tau = 0; tau < _Shape[TAU]; tau++) {
            Complex weight = Complex(1.0, 0.0);
            SmoothWeight[spin_down][sub][coor][tau] = weight;
            SmoothWeight[spin_up][sub][coor][tau] = weight;
        }
    }

    DeltaTWeight = 0.0;
    BareWeight = 0.0;
}

// interaction

void W::InitialWithBare()
{
    _InitialBareWeight();
    DeltaTWeight = BareWeight;
    SmoothWeight = 0.0;
}

void W::SetTest()
{
    //spin conserved; dr==0; exp(-i*tau)
    DeltaTWeight = 0.0;
    BareWeight = 0.0;
    SmoothWeight = 0.0;

    int Lx = _Lat.Size[0], Ly = _Lat.Size[1];
    assert(Lx > 1 && Ly > 1 && D == 2);
    int spin_up = SpinIndex(UP,  //InOfW/InOfVertex
                            UP,  //InOfW/OutOfVertex
                            UP,  //OutOfW/InOfVertex
                            UP); //OutOfW/OutOfVertex

    for (int sub = 0; sub < _Shape[SUB]; sub++) {
        if (!_Lat.IsOnSameSubLat(sub))
            continue;
        int coor = _Lat.Vec2Index({0, 0});
        for (int tau = 0; tau < _Shape[TAU]; tau++) {
            Complex weight = exp(Complex(0.0, -BinToTau(tau)));
            SmoothWeight[spin_up][sub][coor][tau] = weight;
        }
    }

    for (auto i : GetSpinIndexVector_FourSpinsFileter(UpUp2UpUp))
        BareWeight[i] = BareWeight[spin_up];

    for (auto i : GetSpinIndexVector_FourSpinsFileter(UpDown2UpDown)) {
        BareWeight[i] = BareWeight[spin_up];
        BareWeight[i] *= -1.0;
    }
    for (auto i : GetSpinIndexVector_FourSpinsFileter(UpDown2DownUp)) {
        BareWeight[i] = BareWeight[spin_up];
        BareWeight[i] *= 2.0;
    }
}

void W::InitialWithDiagCounter()
{
    //spin==UP,UP,UP,UP; dr==0; independent of tau
    SmoothWeight = 0.0;
    int Lx = _Lat.Size[0], Ly = _Lat.Size[1];
    assert(Lx > 1 && Ly > 1 && D == 2);
    int spin_up = SpinIndex(UP,  //InOfW/InOfVertex
                            UP,  //InOfW/OutOfVertex
                            UP,  //OutOfW/InOfVertex
                            UP); //OutOfW/OutOfVertex

    for (int sub = 0; sub < _Shape[SUB]; sub++) {
        if (!_Lat.IsOnSameSubLat(sub))
            continue;
        int coor = _Lat.Vec2Index({0, 0});
        for (int tau = 0; tau < _Shape[TAU]; tau++) {
            Complex weight = Complex(1.0, 0.0);
            SmoothWeight[spin_up][sub][coor][tau] = weight;
        }
    }

    DeltaTWeight = 0.0;
    BareWeight = 0.0;
}

#if SYSTEM == SPIN
void G::_InitialBareWeight()
{
    BareWeight = 0.0;
    Complex mu = Complex(0.0, PI / 2.0 / _Beta);
    int spin_down = SpinIndex(DOWN, DOWN);
    int spin_up = SpinIndex(UP, UP);
    for (int sub = 0; sub < _Shape[SUB]; sub++) {
        if (_Lat.IsOnSameSubLat(sub))
            continue;
        int coor = 0;
        for (int tau = 0; tau < _Shape[TAU]; tau++) {
            Complex weight = exp(mu * BinToTau(tau)) / Complex(1.0, 1.0);
            BareWeight[spin_down][sub][coor][tau] = weight;
            BareWeight[spin_up][sub][coor][tau] = weight;
        }
    }
}
#endif

#if SYSTEM == SPIN && MODEL == J1J2
void W::_InitialBareWeight()
{
    BareWeight = 0.0;
    int Lx = _Lat.Size[0], Ly = _Lat.Size[1];
    assert(Lx > 1 && Ly > 1 && D == 2);
    int spinindex = SpinIndex(UP,  //InOfW/InOfVertex
                              UP,  //InOfW/OutOfVertex
                              UP,  //OutOfW/InOfVertex
                              UP); //OutOfW/OutOfVertex

    vector<initializer_list<int>> coord;
    int sublatA2B = _Lat.Sublat2Index(0, 1);
    int sublatA2A = _Lat.Sublat2Index(0, 0);
    {
        //J1 interaction A-->B
        coord.push_back({0, 0});
        coord.push_back({0, Ly - 1});
        coord.push_back({Lx - 1, 0});
        coord.push_back({Lx - 1, Ly - 1});
        for (auto e : coord)
            BareWeight[spinindex][sublatA2B][_Lat.Vec2Index(e)] = _Interaction[0];
        //J2 interaction A-->A
        coord.clear();
        coord.push_back({0, 1});
        coord.push_back({1, 0});
        coord.push_back({0, Ly - 1});
        coord.push_back({Lx - 1, 0});
        for (auto e : coord)
            BareWeight[spinindex][sublatA2A][_Lat.Vec2Index(e)] = _Interaction[1];
    }
    int sublatB2A = _Lat.Sublat2Index(1, 0);
    int sublatB2B = _Lat.Sublat2Index(1, 1);
    {
        //J1 interaction B-->A
        coord.clear();
        coord.push_back({0, 0});
        coord.push_back({0, 1});
        coord.push_back({1, 0});
        coord.push_back({1, 1});
        for (auto e : coord)
            BareWeight[spinindex][sublatB2A][_Lat.Vec2Index(e)] = _Interaction[0];
        //J2 interaction B-->B
        coord.clear();
        coord.push_back({0, 1});
        coord.push_back({1, 0});
        coord.push_back({0, Ly - 1});
        coord.push_back({Lx - 1, 0});
        for (auto e : coord)
            BareWeight[spinindex][sublatB2B][_Lat.Vec2Index(e)] = _Interaction[1];
    }
    //Generate other non-zero spin configuration
    {
        for (auto i : GetSpinIndexVector_FourSpinsFileter(UpUp2UpUp))
            BareWeight[i] = BareWeight[spinindex];

        for (auto i : GetSpinIndexVector_FourSpinsFileter(UpDown2UpDown)) {
            BareWeight[i] = BareWeight[spinindex];
            BareWeight[i] *= -1.0;
        }
        for (auto i : GetSpinIndexVector_FourSpinsFileter(UpDown2DownUp)) {
            BareWeight[i] = BareWeight[spinindex];
            BareWeight[i] *= 2.0;
        }
    }
}
#endif
