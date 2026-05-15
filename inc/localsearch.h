#pragma once

#include <mh.h>

std::vector<std::pair<int,int>> getMoves(Problem<int> &problem);

/****
 * Implementation of the Local Search metaheuristic
 *  - Iteratively improves a solution by exploring its neighborhood
 *
 * @see MH
 * @see Problem
 */
class LocalSearch : public MH<int> {
public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);

    //Optimiza una solución dada solution_ini. Si la diferencia en fitness de dos mejoras consecutivas es menor que epsilon, no cuenta como mejora a la hora de seguir.
    ResultMH<int> optimize(Problem<int> &problem, int maxevals, tSolution<int> solution_ini);

};