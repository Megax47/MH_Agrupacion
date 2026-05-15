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
};