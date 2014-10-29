//
//  environment.h
//  Feynman_Simulator
//
//  Created by Kun Chen on 10/17/14.
//  Copyright (c) 2014 Kun Chen. All rights reserved.
//

#ifndef __Feynman_Simulator__environment__
#define __Feynman_Simulator__environment__

#include <list>
#include "../diagram/diagram.h"
#include "../utility/rng.h"
#include "../observable/weight.h"
#include "../lattice/lattice.h"
#include "../utility/scopeguard.h"

#ifndef GET
#define GET(para, thing)                             \
    {                                                \
        stringstream ss(para.front());               \
        (ss) >> thing;                               \
        if ((ss).fail())                             \
            ABORT("Fail to read " << #thing << "!"); \
        para.pop_front();                            \
    }
#endif

enum JobType { MC,
               DYSON };

JobType GetJobsType(std::string);

class Environment {
  protected:
    std::list<std::string> _para;
    Vec<int> _L;

  public:
    Environment();
    ~Environment();
    JobType Type;
    int PID;
    real Jcp;
    real InitialBeta;
    real DeltaBeta;
    real Beta;
    int Order;
    bool DoesLoad;
    std::string StateFile;
    Lattice *Lat;

    bool BuildFromFile(std::string InputFile);
    bool LoadState();
    void SaveState();
};

class EnvMonteCarlo : public Environment {
  private:
    void ReadOrderWeight(char sep = ',');

  public:
    EnvMonteCarlo();
    ~EnvMonteCarlo();
    long long Counter;
    int Toss;
    int Sample;
    int Sweep;
    int Seed;
    real WormSpaceReweight;
    std::string ReadFile;

    Diagram Diag;
    real OrderWeight[MAX_ORDER];

    EstimatorBundle<Complex> cEstimator;
    EstimatorBundle<real> rEstimator;
    EstimatorBundle<real> DetailBalanceEstimator;

    Estimator<real> ZeroOrderWeight;
    Weight::Worm *WormWeight;
    Weight::Sigma *Sigma;
    Weight::Polar *Polar;
    Weight::W *W;
    Weight::G *G;

    bool BuildFromFile(std::string InputFile);
    bool LoadState();
    void SaveState();
};

class EnvDyson : public Environment {
  public:
    EnvDyson();
    bool BuildFromFile(std::string InputFile);
};

#endif /* defined(__Feynman_Simulator__environment__) */
