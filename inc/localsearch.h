#pragma once

#include <mh.h>

/****
 * Implementation of the Local Search metaheuristic
 *  - Iteratively improves a solution by exploring its neighborhood
 *
 * @see MH
 * @see Problem
 */
class LocalSearch : public MH<int> {

    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};