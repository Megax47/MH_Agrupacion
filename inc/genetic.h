#pragma once

#include <mh.h>
#include <problem.h>
#include <vector>

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
    std::vector<Cromosoma> population;
    const int population_size = 50;
    const int tournament_size = 3;
    const float mutation_rate = 0.01;
    const float crossover_rate = 0.8;
    
public:
    Genetic();
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
    int select();
    std::pair<Cromosoma, Cromosoma> crossover(const Cromosoma &parent1, const Cromosoma &parent2);
    void mutate(Cromosoma &solution, Problem<int> &problem);

protected:
    int bestFit(){
        int b = -1;
        float bf = MAXFLOAT;
        for(int i=0; i < population_size; ++i){
            if(population[i].fitness < bf){
                bf = population[i].fitness;
                b = i;
            }
        }
    }
    
    //Primero es el peor, después el segundo peor
    std::pair<int,int> lessFit() { 
        std::pair<int,int> w;
        w.first = -1;
        w.second = -1;
        float wf = 0;
        float wf2 = 0;
        for(int i=0; i<population_size; ++i){
            if(population[i].fitness > wf2){
                if(population[i].fitness > wf){
                    wf2 = wf;
                    w.second = w.first;
                    wf = population[i].fitness;
                    w.first = i;
                }
                else{
                    wf2 = population[i].fitness;
                    w.second = i;
                }
            }
        }
    }
};

class AGG : public Genetic {
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

class AGG_UN : public AGG {
    
};

class AGE : public Genetic {

public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

