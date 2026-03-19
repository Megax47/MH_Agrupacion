#pragma once

#include <mh.h>

/****
 * Implementation of the Local Search metaheuristic
 *  - Iteratively improves a solution by exploring its neighborhood
 *
 * @see MH
 * @see Problem
 */
template <typename tDomain> 
class LocalSearch : public MH<tDomain> {

    ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override {
        assert(maxevals > 0);
        tSolution<tDomain> solution = problem.createSolution();
        tFitness fitness = problem.fitness(solution);
        unsigned int evaluations = 0;

        while  {
            bool improved = false;
            
            // Generamos 
        }while(evaluations < maxevals && improved);

        return ResultMH<tDomain>(solution, fitness, evaluations);
    }
};