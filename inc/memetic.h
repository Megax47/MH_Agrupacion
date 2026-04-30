#pragma once

#include <genetic.h>

class Memetic : public Genetic<UniformCO>{

protected:
    const int BLS_maxevals = 100;
    const float epsilon = 0.1;
    const int round = 10;

public:
    Memetic();
    virtual ~Memetic() = default;

    virtual void applyBLS(Problem<int> &problem, int &evaluations, int maxevals) = 0;

    ResultMH<int> optimize(Problem<int> &problem, int maxevals);

    ResultMH<int> BLS(Problem<int> &problem, Cromosoma &solution, int &evaluations ,int maxevals);
};

class AM_All : public Memetic{
public:
    AM_All();
    void applyBLS(Problem<int> &problem, int &evaluations, int maxevals);
};

class AM_Rand : public Memetic{
private:
    const float prob_LS = 0.1;
public:
    AM_Rand();
    void applyBLS(Problem<int> &problem, int &evaluations, int maxevals);
};

class AM_Best : public Memetic{
private:
    const float pob_percent = 0.1;
public:
    AM_Best();
    void applyBLS(Problem<int> &problem, int &evaluations, int maxevals);
};