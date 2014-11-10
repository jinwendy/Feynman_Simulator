//
//  measure.cpp
//  Feynman_Simulator
//
//  Created by Kun Chen on 10/19/14.
//  Copyright (c) 2014 Kun Chen. All rights reserved.
//

#include "markov_monitor.h"
#include "module/diagram/diagram.h"
#include "module/parameter/parameter.h"

using namespace std;
using namespace diag;
using namespace para;
using namespace mc;

MarkovMonitor::MarkovMonitor()
{
    cEstimator.AddEstimator("1");
    rEstimator.AddEstimator("1");
}

bool MarkovMonitor::BuildNew(ParaMC &para, Diagram &diag, weight::Weight &weight)
{
    Para = &para;
    Diag = &diag;
    Weight = &weight;
    cEstimator.ClearStatistics();
    rEstimator.ClearStatistics();
    //TODO: more observables
    return true;
}

void MarkovMonitor::ReWeight()
{
    //TODO: reweight Estimators here
}

bool MarkovMonitor::Load(const string &InputFile, ParaMC &para, Diagram &diag, weight::Weight &weight)
{
    Para = &para;
    Diag = &diag;
    Weight = &weight;
    cEstimator.LoadStatistics(InputFile);
    rEstimator.LoadStatistics(InputFile);
    //TODO: more observables
    return true;
}

void MarkovMonitor::Save(const string &InputFile, const string &Mode)
{
    cEstimator.SaveStatistics(InputFile, Mode);
    rEstimator.SaveStatistics(InputFile, "a");
    //TODO: more observables
}

void MarkovMonitor::Annealing()
{
    SqueezeStatistics();
}

void MarkovMonitor::SqueezeStatistics()
{
}

void MarkovMonitor::ReWeightEachOrder()
{
}

void MarkovMonitor::Measure()
{
    //    cEstimator[0].Measure(<#const Complex &#>);
    //    cEstimator["1"].Measure(<#const Complex &#>);
}

void MarkovMonitor::AddStatistics()
{
}