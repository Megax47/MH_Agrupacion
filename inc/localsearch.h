#pragma once

#include <mhtrayectory.h>

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
    ResultMH<int> optimize(Problem<int> &problem, int maxevals){
        tSolution<int> initial = problem.createSolution();
        tFitness fitness = problem.fitness(initial);
        return optimize(problem, initial, fitness, maxevals-1);
     }

    //Optimiza una solución dada solution_ini.
    ResultMH<int> optimize(Problem<int> &problem, const tSolution<int> &solution_ini, tFitness fitness, int maxevals);

};