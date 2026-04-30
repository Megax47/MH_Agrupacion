#pragma once

#include <genetic.h>

class Memetic : public Genetic<UniformCO>{

private:
    const int BSL_maxeval = 100;
    const float epsilon = 0.1;
    
public:
    Memetic();

    ResultMH<int> optimize(Problem<int> &problem, int maxevals);

    ResultMH<int> BLS(Problem<int> &problem, Cromosoma &solution);
};