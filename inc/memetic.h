#pragma once

#include <genetic.h>

class Memetic : public Genetic<FixSegmentCO>{

protected:
    const int BLS_maxevals = 100;
    const float epsilon = 0.1;
    const float round = 10;

public:
    Memetic();

    virtual void applyBLS();

    ResultMH<int> optimize(Problem<int> &problem, int maxevals);

    ResultMH<int> BLS(Problem<int> &problem, Cromosoma &solution);
};

class AM_All : public Memetic{

};

class AM_Rand : public Memetic{

};

class AM_Best : public Memetic{

};