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

template<typename CrossoverOp>
class Genetic : public MH<int> {

protected:
    CrossoverOp crossover_op;
    std::vector<Cromosoma> population;
    Problem<int> &problem;
    const int population_size = 50;
    const int tournament_size = 3;
    const float mutation_rate = 0.01;
    const float crossover_rate = 0.8;
    
public:
    Genetic();
    virtual ResultMH<int> optimize(int maxevals);
    int select();
    virtual std::pair<Cromosoma, Cromosoma> crossover(const Cromosoma &parent1, const Cromosoma &parent2){
        return crossover_op(parent1, parent2, problem);
    }
    void mutate(Cromosoma &solution);

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

template<typename CrossoverOp>
class AGG : public Genetic<CrossoverOp> {
public:
    ResultMH<int> optimize(int maxevals);
};

template<typename CrossoverOp>
class AGE : public Genetic<CrossoverOp>  {
public:
    ResultMH<int> optimize(int maxevals);
};

std::pair<Cromosoma, Cromosoma> uniformCO(const Cromosoma &parent1, const Cromosoma &parent2, Problem<int> &problem){
    //Inicializamos los hijos
    int sol_size = parent1.genes.size();
    tSolution<int> c1g(sol_size), c2g(sol_size);

    //Aplicamos el operador
    for(int i=0; i<sol_size; ++i){
        if(Random::get<bool>(0.5)){
            c1g[i] = parent1.genes[i];
            c2g[i] = parent2.genes[i];
        }
        else{
            c1g[i] = parent2.genes[i];
            c2g[i] = parent1.genes[i];
        }
    }

    //Devolvemos
    Cromosoma child1(c1g,-1), child2(c2g,-1);
    return std::make_pair(child1, child2);
}

std::pair<Cromosoma, Cromosoma> fixSegmentCO(const Cromosoma &parent1, const Cromosoma &parent2, Problem<int> &problem){
     //Inicializamos los hijos
    int N = problem.getSolutionSize();
    tSolution<int> c1g(N), c2g(N);

    int pos= Random::get<int>(0,N);
    int tam = Random::get<int>(1,N/3);

    int idx = -1;
    for(int i=0; i<tam; ++i){
        idx = (i+pos)%N;
        c1g[idx] = parent1.genes[idx];
        c2g[idx] = parent2.genes[idx];
    }

    for(int i=0; i<N-tam; ++i){
        int idx2 = (i+idx)%N;
        if(Random::get<bool>(0.5)){
            c1g[i] = parent1.genes[i];
            c2g[i] = parent2.genes[i];
        }
        else{
            c1g[i] = parent2.genes[i];
            c2g[i] = parent1.genes[i];
        }
    }
    
    //Devolvemos
    Cromosoma child1(c1g,-1), child2(c2g,-1);
    return std::make_pair(child1, child2);
}

