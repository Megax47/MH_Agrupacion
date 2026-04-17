#pragma once

#include <mh.h>
#include <problem.h>
#include <set>

struct Cromosoma {
    tSolution<int> genes;
    tFitness fitness;

    Cromosoma(const tSolution<int> &solution, tFitness fitness) : genes(solution), fitness(fitness) {}
    
    bool operator>(const Cromosoma &other) const{
        return fitness > other.fitness;
    }
};

class Genetic : public MH<int> {

protected:
    const int population_size = 50;
    const int tournament_size = 5;
    const float mutation_rate = 0.01;
    const float crossover_rate = 0.8;

public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
    Cromosoma select();
    std::pair<Cromosoma, Cromosoma> crossover(const Cromosoma &parent1, const Cromosoma &parent2);
    void mutate(Cromosoma &solution, Problem<int> &problem);
};

class AGG : public Genetic {
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

class AGE : public Genetic {

protected:
    std::set<Cromosoma> population;
public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

