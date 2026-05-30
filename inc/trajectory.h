#pragma once

#include <localsearch.h>

class ES: public MHTrayectory<int> {

private:
    float const phi = 0.3;
    float const mu = 0.2;
    float T_fin = 0.0001; //! 0.001 es mucha temperatura para la explotación final
public:
    ES(float T_fin=0.0001): T_fin(T_fin){};
    ResultMH<int> optimize(Problem<int> &problem, int maxevals){
        tSolution<int> initial = problem.createSolution();
        tFitness fitness = problem.fitness(initial);
        return optimize(problem, initial, fitness, maxevals-1);
    }
    ResultMH<int> optimize(Problem<int> &problem, const tSolution<int> &solution_ini, tFitness fitness_ini, int maxevals);
};

class BMB_Unif : public LocalSearch {

private:
    int const n_inicios = 5;

public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

class BMB_NoUnif : public LocalSearch {

public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals);
};

class ILS : public LocalSearch {

private:
    float const mutation_rate = 0.2;
    int const n_busquedas = 5;

public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals){
        int maxLSevals = maxevals/n_busquedas;
        int evals = 0;
        ResultMH<int> best_solution = LocalSearch::optimize(problem, maxLSevals);
        evals += best_solution.evaluations;
        for(int i=1; i<n_busquedas; ++i){
            tSolution<int> mutated_solution = mutate(best_solution.solution, problem);
            tFitness mutated_fitness = problem.fitness(mutated_solution);
            ResultMH<int> solution = LocalSearch::optimize(problem, mutated_solution, mutated_fitness, maxLSevals-1);
            evals += solution.evaluations;
            if(solution.fitness < best_solution.fitness) best_solution = solution;
        }
        best_solution.evaluations = evals;
        return best_solution;
    }

    //! Cuidado la referencia
    tSolution<int> mutate(tSolution<int> &old_solution, Problem<int> &problem){
        auto neighborhood = getMoves(problem);
        Random::shuffle(neighborhood.begin(), neighborhood.end());
        int num_changes = mutation_rate*problem.getSolutionSize();
        int changes = 0;
        tSolution<int> solution = old_solution;

        for(const auto &move : neighborhood){
            if(changes >= num_changes) break;
            if (move.second == solution[move.first]) continue;
            solution[move.first] = move.second;
            changes++;
        }
        problem.fix(solution);
        return solution;
    }
};

class ILS_ES : public ES {

private:
    float const mutation_rate = 0.2;
    int const n_busquedas = 5;

public:
    ResultMH<int> optimize(Problem<int> &problem, int maxevals){
        int maxLSevals = maxevals/n_busquedas;
        int evals = 0;
        ResultMH<int> best_solution = ES::optimize(problem, maxLSevals);
        evals += best_solution.evaluations;
        for(int i=1; i<n_busquedas; ++i){
            tSolution<int> mutated_solution = mutate(best_solution.solution, problem);
            tFitness mutated_fitness = problem.fitness(mutated_solution);
            ResultMH<int> solution = ES::optimize(problem, mutated_solution, mutated_fitness, maxLSevals-1);
            evals += solution.evaluations;
            if(solution.fitness < best_solution.fitness) best_solution = solution;
        }
        best_solution.evaluations = evals;
        return best_solution;
    }

    //! Cuidado la referencia
    tSolution<int> mutate(tSolution<int> &old_solution, Problem<int> &problem){
        auto neighborhood = getMoves(problem);
        Random::shuffle(neighborhood.begin(), neighborhood.end());
        int num_changes = mutation_rate*problem.getSolutionSize();
        int changes = 0;
        tSolution<int> solution = old_solution;

        for(const auto &move : neighborhood){
            if(changes >= num_changes) break;
            if (move.second == solution[move.first]) continue;
            solution[move.first] = move.second;
            changes++;
        }
        problem.fix(solution);
        return solution;
    }
};