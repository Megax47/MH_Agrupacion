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
           int N = parent1.genes.size();

        tSolution<int> c1(N, -1);
        tSolution<int> c2(N, -1);

        // 1. Elegir segmento
        int pos = Random::get<int>(0, N-1);
        int tam = Random::get<int>(1, std::max(1, N/3));

        // 2. Copiar segmento fijo
        for(int i = 0; i < tam; ++i){
            int idx = (pos + i) % N;
            c1[idx] = parent1.genes[idx];
            c2[idx] = parent2.genes[idx];
        }

        // 3. Rellenar el resto con el otro padre
        for(int i = 0; i < N; ++i){
            if(c1[i] == -1){
                if(Random::get<bool>(0.5)){
                    c1[i] = parent2.genes[i];
                    c2[i] = parent1.genes[i];
                }
                else{
                    c1[i] = parent1.genes[i];
                    c2[i] = parent2.genes[i];
                }
            }
        }

        // 4. Reparar soluciones
        problem.fix(c1);
        problem.fix(c2);

        return {
            Cromosoma(c1, -1),
            Cromosoma(c2, -1)
        };
    }
};

template<typename CrossoverOp>
class Genetic : public MH<int> {

protected:
    CrossoverOp crossover_op;
    std::vector<Cromosoma> population;
    const int population_size = 50;
    const int tournament_size = 3;
    const float mutation_rate = 0.1;
    const float crossover_rate = 0.8;
    
public:
    Genetic() = default;
    virtual ~Genetic() = default;
    virtual ResultMH<int> optimize(Problem<int> &problem, int maxevals) = 0;

    int select(){
        int best = Random::get<int>(0, population_size - 1);

        for(int i = 1; i < tournament_size; ++i){
            int r = Random::get<int>(0, population_size - 1);
            if(population[r].fitness < population[best].fitness){
                best = r;
            }
        }
        return best;
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
        int w1 = 0, w2 = 1;

        if (population[w2].fitness > population[w1].fitness)
            std::swap(w1, w2);

        for(int i = 2; i < population_size; ++i){
            if(population[i].fitness > population[w1].fitness){
                w2 = w1;
                w1 = i;
            } else if(population[i].fitness > population[w2].fitness){
                w2 = i;
            }
        }

        return {w1, w2};
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
            std::vector<int> selected;
            selected.reserve(this->population_size);
            for (int i=0; i< this->population_size; ++i) {
                selected.push_back(this->select());
            }

            std::vector<Cromosoma> new_pop;
            new_pop.reserve(this->population_size);

            //Cruzar el número esperado de veces en orden fijo
            int spected_crossovers = (this->population_size/2)*this->crossover_rate;
            for (int i=0; i < spected_crossovers; ++i){
                auto [child1, child2] = this->crossover(this->population[selected[2*i]], this->population[selected[2*i+1]], problem);
                new_pop.push_back(child1);
                new_pop.push_back(child2);
            }

            //Asignar los que no cruzaron directamente
            for (int i=spected_crossovers*2; i<this->population_size; ++i){
                new_pop.push_back(this->population[selected[i]]);
            }

            this->population = std::move(new_pop);

            //Seleccionamos los cromosomas a mutar
            std::vector<int> index(this->population_size);
            std::iota(std::begin(index), std::end(index), 0); 
            Random::shuffle(index);

            int spected_mutations = this->population_size*this->mutation_rate;
            for(int i=0; i<spected_mutations; ++i){
                this->mutate(this->population[index[i]], problem);
            }

            //Calculamos el fitness de todos los que hayan cambiado
            for (int i = 0; i < this->population_size; ++i) {
                if (this->population[i].fitness < 0){
                    if(evaluations < maxevals){ //No pasarnos evaluando
                        this->population[i].fitness = problem.fitness(this->population[i].genes);
                        ++evaluations;
                    }else this->population[i].fitness = MAXFLOAT;
                    
                }
            }

            //Sustituimos el peor de la actual por el mejor de la anterior si es mejor
            if(this->population[this->lessFit().first].fitness > best.fitness)
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

         // Reunir los 4 candidatos y quedarse los 2 mejores
            std::vector<Cromosoma*> candidates = {
                &child1, &child2,
                &this->population[worst.first],
                &this->population[worst.second]
            };
            std::sort(candidates.begin(), candidates.end(),
                [](const Cromosoma* a, const Cromosoma* b){ return a->fitness < b->fitness; });

            this->population[worst.first]  = *candidates[0];
            this->population[worst.second] = *candidates[1];
        }
        Cromosoma best = this->population[this->bestFit()];
        return ResultMH(best.genes, best.fitness, evaluations); // Devolver el mejor individuo encontrado
    }

};