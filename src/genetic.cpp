#include <genetic.h>

int Genetic::select(){
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

    return best;
}

void Genetic::mutate(Cromosoma &solution, Problem<int> &problem){
    do{ //?Si veo que puede atascarse, evitar repeticiones con un shufle
        int pos = Random::get<int>(0,problem.getSolutionSize());
        int val = Random::get<int>(0,problem.getSolutionDomainRange().second);
        solution.genes[pos] = val;
    }while(!problem.isValid(solution.genes));
}

ResultMH<int> AGE::optimize(Problem<int> &problem, int maxevals) {
    int evaluations = population_size;

    // Inicializar la población
    for (int i = 0; i < population_size; ++i) {
        tSolution<int> solution = problem.createSolution();
        tFitness fitness = problem.fitness(solution);
        population.emplace_back(solution, fitness);
    }

    while (evaluations < maxevals) {
        // Seleccionar padres
        Cromosoma parent1 = population[select()];
        Cromosoma parent2 = population[select()];

        // Cruzar padres para generar hijos
        auto [child1, child2] = crossover(parent1, parent2);

        // Mutar hijos con cierta probabilidad
        if(Random::get<bool>(mutation_rate)) mutate(child1, problem);
        if(Random::get<bool>(mutation_rate)) mutate(child2, problem);

        // Evaluar hijos
        tFitness fitness1 = problem.fitness(child1.genes);
        tFitness fitness2 = problem.fitness(child2.genes);
        evaluations += 2;

        child1.fitness = fitness1;
        child2.fitness = fitness2;

        std::pair<int,int> worst = lessFit();
        float wf1 = population[worst.first].fitness;
        float wf2 = population[worst.second].fitness;

        if(fitness1 < fitness2){
            if(fitness2 < wf2){
                population[worst.first] = child1;
                population[worst.second] = child2;
            }else if(fitness1 < wf2){
                population[worst.first] = child1;
            }else if(fitness1 < wf1){
                population[worst.first] = child1;
            }
        }else{
            if(fitness1 < wf2){
                population[worst.first] = child2;
                population[worst.second] = child1;
            }else if(fitness2 < wf2){
                population[worst.first] = child2;
            }else if(fitness2 < wf1){
                population[worst.first] = child2;
            }
        }
    }
    Cromosoma best = population[bestFit()];
    return ResultMH(best.genes, best.fitness, evaluations); // Devolver el mejor individuo encontrado
}

ResultMH<int> AGG::optimize(Problem<int> &problem, int maxevals) {
    int evaluations = population_size;

    // Inicializar la población
    for (int i = 0; i < population_size; ++i) {
        tSolution<int> solution = problem.createSolution();
        tFitness fitness = problem.fitness(solution);
        population.emplace_back(solution, fitness);
    }

    while(evaluations < maxevals){
        
        //Nos quedamos el mejor (Elitismo)
        Cromosoma best = population[bestFit()];

        //Seleccionar padres
        std::vector<int> selected(population_size);
        for (int i=0; i< population_size; ++i) {
            selected[i] = select();
        }

        //Cruzar el número esperado de veces en orden fijo
        int spected_crossovers = (population_size/2)*crossover_rate;
        for (int i=0; i < spected_crossovers; i+=2){
            auto [child1, child2] = crossover(population[selected[i]], population[selected[i+1]]);
            population[i].genes = child1.genes;
            population[i+1].genes = child2.genes;
        }

        //Asignar los que no cruzaron directamente
        for (int i=spected_crossovers*2; i<population_size; ++i){
            population[i] = population[selected[i]];
        }

        //Seleccionamos los cromosomas a mutar
        std::vector<int> index(population_size);
        std::iota(std::begin(index), std::end(index), 0); 
        Random::shuffle(index);

        int spected_mutations = population_size*mutation_rate;
        for(int i=0; i<spected_mutations; ++i){
            mutate(population[index[i]], problem);
        }

        //Calculamos el fitness de todos
        for(int i=0; i<population_size; ++i){
            population[i].fitness = problem.fitness(population[i].genes);
            ++evaluations;
        }

        //Sustituimos el peor de la actual por el mejor de la anterior
        population[lessFit().first] = best;
    }
    Cromosoma best = population[bestFit()];
    return ResultMH(best.genes, best.fitness, evaluations); // Devolver el mejor individuo encontrado
}

std::pair<Cromosoma, Cromosoma> crossover(const Cromosoma &parent1, const Cromosoma &parent2);