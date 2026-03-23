#include <localsearch.h>
#include <cassert>

std::vector<std::pair<int,int>> getMoves(Problem<int> &problem) {
    std::vector<std::pair<int,int>> neighborhood;
    for (int i = 0; i < problem.getSolutionSize(); ++i) {
        for(int j = problem.getSolutionDomainRange().first; j < problem.getSolutionDomainRange().second; ++j) {   
            neighborhood.push_back(std::make_pair(i, j));
        }
    }
    return neighborhood;
}

ResultMH<int> LocalSearch::optimize(Problem<int> &problem, int maxevals) {
    assert(maxevals > 0);
    tSolution<int> solution = problem.createSolution();
    tFitness fitness = problem.fitness(solution);
    auto neighborhood = getMoves(problem);
    unsigned int evaluations = 0;
    bool improved = true;

    do {
        improved = false;
        // Barajamos el orden de las soluciones vecinas para evitar sesgos
        Random::shuffle(neighborhood.begin(), neighborhood.end());
        
        for (const auto &move : neighborhood) {
            if (evaluations >= maxevals) break;
            if (move.second == solution[move.first]) continue; // Si el movimiento no cambia la solución, lo saltamos
            int pos_change = move.first;
            int new_value = move.second;
            if (!problem.isValid(solution, pos_change, new_value)) continue; // Si el movimiento no es válido, lo saltamos
            tFitness new_fitness = problem.fitness(solution, nullptr, pos_change, new_value);
            evaluations++;
            if (new_fitness < fitness) { // Si encontramos una solución mejor, la aceptamos
                solution[pos_change] = new_value;
                fitness = new_fitness;
                improved = true;
                break; // Salimos del bucle para generar un nuevo vecindario
            }
        }
    }while(evaluations < maxevals && improved);

    return ResultMH<int>(solution, fitness, evaluations);
}