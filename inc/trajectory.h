#pragma once

#include <localsearch.h>

class ES: public MH<int> {

private:
    float const phi = 0.3;
    float const mu = 0.2;
    float const T_fin = 0.0001; //! 0.001 es mucha temperatura para la explotación final
public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

class BMB_Rand : public LocalSearch { //? Con GRASP?

private:
    int n_inicios = 5;

public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

class ILS : public MH<int> {

    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};