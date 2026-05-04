#include "memetic.h"

//Basado en AGG
ResultMH<int> Memetic::optimize(Problem<int> &problem, int maxevals){
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
            int rounds = 0;
            while(rounds < round && evaluations < maxevals){
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
                ++rounds;
            }
            applyBLS(problem, evaluations, maxevals);
        }

        Cromosoma best = this->population[this->bestFit()];
        return ResultMH(best.genes, best.fitness, evaluations); // Devolver el mejor individuo encontrado
}

ResultMH<int> Memetic::BLS(Problem<int> &problem, Cromosoma &solution, int &evaluations ,int maxevals){

    int N = problem.getSolutionSize();
    std::vector<int> index(N);
    std::iota(std::begin(index), std::end(index), 0); 
    Random::shuffle(index);

    int max_fallos = epsilon*N;
    int fallos = 0;
    int i = 0;
    int BLS_evals = 0;
    SolutionFactoringInfo<int> *solution_info = problem.generateFactoringInfo(solution.genes);

    int l1 = problem.getSolutionDomainRange().first;
    int l2 = problem.getSolutionDomainRange().second;

    while((fallos < max_fallos) && (i < N) && (BLS_evals < BLS_maxevals) && (evaluations < maxevals)){
        int p = index[i];
        int old_value = solution.genes[p];

        for( int val=l1; val<l2 && (BLS_evals < BLS_maxevals) && (evaluations < maxevals); ++val){
            if(val == old_value || !problem.isValid(solution.genes, p, val)) continue;
            tFitness new_fitness = problem.fitness(solution.genes,solution_info,p,val);
            ++evaluations;
            ++BLS_evals;
            if(new_fitness < solution.fitness){
                problem.updateSolutionFactoringInfo(solution_info,solution.genes, p, val);
                solution.genes[p] = val;
                solution.fitness = new_fitness; 
            }
        }

        if(old_value == solution.genes[p]) ++fallos;
        ++i;
    }

    delete solution_info;

    return ResultMH(solution.genes, solution.fitness, evaluations);
}

void AM_All::applyBLS(Problem<int> &problem, int &evaluations, int maxevals){
    
    for(int i=0; i<population_size && evaluations < maxevals; ++i){
        auto mejora = BLS(problem, population[i], evaluations, maxevals);
        population[i].genes = mejora.solution;
        population[i].fitness = mejora.fitness;
    }
}

void AM_Rand::applyBLS(Problem<int> &problem, int &evaluations, int maxevals){
    
    for(int i=0; i<population_size && evaluations < maxevals; ++i){
        if(Random::get<bool>(prob_LS)){
            auto mejora = BLS(problem, population[i], evaluations, maxevals);
            population[i].genes = mejora.solution;
            population[i].fitness = mejora.fitness;
        }
    }
}

void AM_Best::applyBLS(Problem<int> &problem, int &evaluations, int maxevals){

    std::sort(population.begin(), population.end(),
        [](const Cromosoma& a, const Cromosoma& b){ return a.fitness < b.fitness; });

    int selected = pob_percent*population_size;

    for(int i=0; i<selected && evaluations < maxevals; ++i){
        auto mejora = BLS(problem, population[i], evaluations, maxevals);
        population[i].genes = mejora.solution;
        population[i].fitness = mejora.fitness;
    }
}

Memetic::Memetic() = default;
AM_All::AM_All()  = default;
AM_Rand::AM_Rand() = default;
AM_Best::AM_Best() = default;
