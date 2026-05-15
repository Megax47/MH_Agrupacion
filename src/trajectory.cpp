#include "trajectory.h"
#include <iostream>

ResultMH<int> BMB_Unif::optimize(Problem<int> &problem, int maxevals){
    int maxLSevals = maxevals/n_inicios;
    int evals = 0;
    ResultMH<int> best_solution = LocalSearch::optimize(problem, maxLSevals);
    evals += best_solution.evaluations;
    for(int i=1; i<n_inicios; ++i){
        ResultMH<int> solution = LocalSearch::optimize(problem, maxLSevals);
        evals += solution.evaluations;
        if(solution.fitness < best_solution.fitness) best_solution = solution;
    }
    best_solution.evaluations = evals;
    return best_solution;
}

ResultMH<int> BMB_NoUnif::optimize(Problem<int> &problem, int maxevals){
    int evals = 0;
    ResultMH<int> best_solution = LocalSearch::optimize(problem, maxevals);
    evals += best_solution.evaluations;
    while(evals < maxevals){
        ResultMH<int> solution = LocalSearch::optimize(problem, maxevals-evals);
        evals += solution.evaluations;
        if(solution.fitness < best_solution.fitness) best_solution = solution;
    }
    best_solution.evaluations = evals;
    return best_solution;
}

ResultMH<int> ES::optimize(Problem<int> &problem, int maxevals){
    return optimize(problem,maxevals,problem.createSolution());
}

ResultMH<int> ES::optimize(Problem<int> &problem, int maxevals, tSolution<int> solution_ini){
    tSolution<int> solution = solution_ini;
    SolutionFactoringInfo<int> *solution_info = problem.generateFactoringInfo(solution);
    float fitness = problem.fitness(solution);

    tSolution<int> best_solution = solution;
    float best_fitness = fitness;

    float T_ini = (mu*fitness)/(-log(phi));
    float T = T_ini;

    int m = problem.getSolutionSize();
    int max_vecinos = 10*m;
    int max_exitos = m;
    int M = maxevals/max_vecinos;

    if (M == 0) M = 1;
    float beta = (T_ini - T_fin)/(M*T_ini*T_fin);

    auto neighborhood = getMoves(problem);
    unsigned int evaluations = 1;
    int Nexitos = 0;
    int Nvecinos = 0;

    do {
        if(T < T_fin) T = T_fin;
        Nexitos = 0;
        Nvecinos = 0;
        // Barajamos el orden de las soluciones vecinas para evitar sesgos
        Random::shuffle(neighborhood.begin(), neighborhood.end());
        
        for (const auto &move : neighborhood) {
            if (evaluations >= maxevals || Nexitos >= max_exitos || Nvecinos >= max_vecinos) break;
            if (move.second == solution[move.first]) continue; // Si el movimiento no cambia la solución, lo saltamos
            int pos_change = move.first;
            int new_value = move.second;
            if (!problem.isValid(solution, pos_change, new_value)) continue; // Si el movimiento no es válido, lo saltamos
            tFitness new_fitness = problem.fitness(solution, solution_info, pos_change, new_value);
            float dif = new_fitness - fitness;
            evaluations++;
            Nvecinos++;
            //std::cout << exp(-dif/T) << std::endl;
            if ((dif < 0) || (Random::get<float>(0,1) <= exp(-dif/T))) { // Si encontramos una solución mejor, la aceptamos; si no, la aceptamos con cierta probabilidad
                problem.updateSolutionFactoringInfo(solution_info, solution, pos_change, new_value);
                solution[pos_change] = new_value;
                fitness = new_fitness;
                Nexitos++;

                if(new_fitness < best_fitness){
                    best_fitness = new_fitness;
                    best_solution = solution;
                }
            }
        }
        T /= 1+(beta*T);
    }while(evaluations < maxevals && Nexitos!=0);
   
    delete solution_info;

    return ResultMH<int>(best_solution, best_fitness, evaluations);
}
