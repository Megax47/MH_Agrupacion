#include <genetic.h>

ResultMH<int> AGE::optimize(Problem<int> &problem, int maxevals) {
    int evaluations = population_size;

    // Inicializar la población
    for (int i = 0; i < population_size; ++i) {
        tSolution<int> solution = problem.createSolution();
        tFitness fitness = problem.fitness(solution);
        population.emplace(solution, fitness);
    }

    while (evaluations < maxevals) {
        // Seleccionar padres
        Cromosoma parent1 = select();
        Cromosoma parent2 = select();

        // Cruzar padres para generar hijos
        auto [child1, child2] = crossover(parent1, parent2);

        // Mutar hijos
        mutate(child1, problem);
        mutate(child2, problem);

        // Evaluar hijos
        tFitness fitness1 = problem.fitness(child1.genes);
        tFitness fitness2 = problem.fitness(child2.genes);
        evaluations += 2;

        //?MEJORABLE
        // Agregar hijos a la población
        population.emplace(child1,fitness1);
        population.emplace(child2,fitness2);

        //Eliminamos los dos peores
        population.erase(*population.rbegin());
        population.erase(*population.rbegin());

    }

    Cromosoma best = *population.begin();
    return ResultMH(best.genes, best.fitness, evaluations); // Devolver el mejor individuo encontrado
}