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

struct UniformCO {
    std::pair<Cromosoma, Cromosoma> operator()(
        const Cromosoma &parent1, 
        const Cromosoma &parent2, 
        Problem<int> &problem
    ) const {
        int sol_size = parent1.genes.size();
        tSolution<int> c1g(sol_size), c2g(sol_size);

        for(int i = 0; i < sol_size; ++i) {
            if(Random::get<bool>(0.5)) {
                c1g[i] = parent1.genes[i];
                c2g[i] = parent2.genes[i];
            } else {
                c1g[i] = parent2.genes[i];
                c2g[i] = parent1.genes[i];
            }
        }

        problem.fix(c1g);
        problem.fix(c2g);
        return std::make_pair(Cromosoma(c1g, -1), Cromosoma(c2g, -1));
    }
};

struct FixSegmentCO {
    std::pair<Cromosoma, Cromosoma> operator()(
        const Cromosoma &parent1, 
        const Cromosoma &parent2, 
        Problem<int> &problem
    ) const {
        int N = problem.getSolutionSize();
        tSolution<int> c1g(N), c2g(N);

        int pos = Random::get<int>(0, N-1);
        int tam = std::max(1,Random::get<int>(1, N/3));

        int idx = 0;
        for(int i = 0; i < tam; ++i) {
            idx = (i + pos) % N;
            c1g[idx] = parent1.genes[idx];
            c2g[idx] = parent2.genes[idx];
        }

        for(int i = 0; i < N - tam; ++i) {
            int idx2 = (i + idx) % N;
            if(Random::get<bool>(0.5)) {
                c1g[idx2] = parent1.genes[idx2];
                c2g[idx2] = parent2.genes[idx2];
            } else {
                c1g[idx2] = parent2.genes[idx2];
                c2g[idx2] = parent1.genes[idx2];
            }
        }

        problem.fix(c1g);
        problem.fix(c2g);
        return std::make_pair(Cromosoma(c1g, -1), Cromosoma(c2g, -1));
    }
};

template<typename CrossoverOp>
class Genetic : public MH<int> {

protected:
    CrossoverOp crossover_op;
    std::vector<Cromosoma> population;
    const int population_size = 50;
    const int tournament_size = 3;
    const float mutation_rate = 0.01;
    const float crossover_rate = 0.8;
    
public:
    Genetic() = default;
    virtual ~Genetic() = default;
    virtual ResultMH<int> optimize(Problem<int> &problem, int maxevals) = 0;
    int select(){
        std::vector<int> index(population_size);
        std::iota(std::begin(index), std::end(index), 0); 
        Random::shuffle(index);

        float bf = MAXFLOAT;
        int best = -1;

        for(int i=0; i<tournament_size; ++i){
            if(population[index[i]].fitness < bf){
                bf = population[index[i]].fitness;
                best = i;
            }
        }
        //std::cout << "select: " << best << endl;
        return index[best];
    }
    std::pair<Cromosoma, Cromosoma> crossover(const Cromosoma &parent1, const Cromosoma &parent2, Problem<int> &problem) {
        return crossover_op(parent1, parent2, problem);
    }
    
    void mutate(Cromosoma &solution, Problem<int> &problem) {
        int pos = Random::get<int>(0, problem.getSolutionSize()-1);
        int val = Random::get<int>(0, problem.getSolutionDomainRange().second-1);
        solution.genes[pos] = val;
        problem.fix(solution.genes);
    }


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
        //std::cout << "bestFit: " << b << endl;
        return b;
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
        //std::cout << "lessFit: " << w.first << " , " << w.second << endl;
        return w;
    }
};

template<typename CrossoverOp>
class AGG : public Genetic<CrossoverOp> {
public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals){
        int evaluations = this->population_size;
        this->population.clear();
        this->population.reserve(this->population_size);

        // Inicializar la población
        for (int i = 0; i < this->population_size; ++i) {
            tSolution<int> solution = problem.createSolution();
            tFitness fitness = problem.fitness(solution);
            this->population.emplace_back(solution, fitness);
        }

        while(evaluations < maxevals){
            
            //Nos quedamos el mejor (Elitismo)
            Cromosoma best = this->population[this->bestFit()];

            //Seleccionar padres
            std::vector<int> selected(this->population_size);
            for (int i=0; i< this->population_size; ++i) {
                selected[i] = this->select();
            }

            //Cruzar el número esperado de veces en orden fijo
            int spected_crossovers = (this->population_size/2)*this->crossover_rate;
            for (int i=0; i < spected_crossovers; i+=2){
                auto [child1, child2] = this->crossover(this->population[selected[i]], this->population[selected[i+1]], problem);
                this->population[i].genes = child1.genes;
                this->population[i+1].genes = child2.genes;
            }

            //Asignar los que no cruzaron directamente
            for (int i=spected_crossovers*2; i<this->population_size; ++i){
                this->population[i] = this->population[selected[i]];
            }

            //Seleccionamos los cromosomas a mutar
            std::vector<int> index(this->population_size);
            std::iota(std::begin(index), std::end(index), 0); 
            Random::shuffle(index);

            int spected_mutations = this->population_size*this->mutation_rate;
            for(int i=0; i<spected_mutations; ++i){
                this->mutate(this->population[index[i]], problem);
            }

            //Calculamos el fitness de todos
            for(int i=0; i<this->population_size; ++i){
                this->population[i].fitness = problem.fitness(this->population[i].genes);
                ++evaluations;
            }

            //Sustituimos el peor de la actual por el mejor de la anterior
            this->population[this->lessFit().first] = best;
        }
        Cromosoma best = this->population[this->bestFit()];
        return ResultMH(best.genes, best.fitness, evaluations); // Devolver el mejor individuo encontrado
    }

};

template<typename CrossoverOp>
class AGE : public Genetic<CrossoverOp>  {
public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals){
        int evaluations = this->population_size;
        this->population.clear();
        this->population.reserve(this->population_size);

        // Inicializar la población
        for (int i = 0; i < this->population_size; ++i) {
            tSolution<int> solution = problem.createSolution();
            tFitness fitness = problem.fitness(solution);
            this->population.emplace_back(solution, fitness);
        }

        while (evaluations < maxevals) {
            // Seleccionar padres
            Cromosoma parent1 = this->population[this->select()];
            Cromosoma parent2 = this->population[this->select()];

            // Cruzar padres para generar hijos
            auto [child1, child2] = this->crossover(parent1, parent2, problem);

            // Mutar hijos con cierta probabilidad
            if(Random::get<bool>(this->mutation_rate)) this->mutate(child1, problem);
            if(Random::get<bool>(this->mutation_rate)) this->mutate(child2, problem);

            // Evaluar hijos
            tFitness fitness1 = problem.fitness(child1.genes);
            tFitness fitness2 = problem.fitness(child2.genes);
            evaluations += 2;

            child1.fitness = fitness1;
            child2.fitness = fitness2;

            std::pair<int,int> worst = this->lessFit();
            float wf1 = this->population[worst.first].fitness;
            float wf2 = this->population[worst.second].fitness;

            if(fitness1 < fitness2){
                if(fitness2 < wf2){
                    this->population[worst.first] = child1;
                    this->population[worst.second] = child2;
                }else if(fitness1 < wf2){
                    this->population[worst.first] = child1;
                }else if(fitness1 < wf1){
                    this->population[worst.first] = child1;
                }
            }else{
                if(fitness1 < wf2){
                    this->population[worst.first] = child2;
                    this->population[worst.second] = child1;
                }else if(fitness2 < wf2){
                    this->population[worst.first] = child2;
                }else if(fitness2 < wf1){
                    this->population[worst.first] = child2;
                }
            }
        }
        Cromosoma best = this->population[this->bestFit()];
        return ResultMH(best.genes, best.fitness, evaluations); // Devolver el mejor individuo encontrado
    }

};