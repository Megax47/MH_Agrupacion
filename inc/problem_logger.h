#pragma once

#include <problem.h>
#include <vector>
#include <utility>
#include <limits>

/**
 * Wrapper around Problem<int> that intercepts every fitness evaluation
 * and records the best fitness seen so far at each evaluation count.
 *
 * Usage:
 *   ProblemLogger logger(real_problem);
 *   logger.reset();
 *   mh.optimize(logger, maxevals);
 *   auto &history = logger.getHistory(); // vector of (eval_number, best_fitness)
 */
class ProblemLogger : public Problem<int> {

    Problem<int> &inner;
    tFitness best_fitness;
    unsigned int eval_count;

    // Each entry: (evaluation number, best fitness so far)
    std::vector<std::pair<unsigned int, tFitness>> history;

    void record(tFitness f) {
        ++eval_count;
        if (f < best_fitness) best_fitness = f;
        history.emplace_back(eval_count, best_fitness);
    }

public:
    explicit ProblemLogger(Problem<int> &problem) : inner(problem) {
        reset();
    }

    void reset() {
        best_fitness = std::numeric_limits<tFitness>::max();
        eval_count   = 0;
        history.clear();
    }

    const std::vector<std::pair<unsigned int, tFitness>> &getHistory() const {
        return history;
    }

    // ----------------------------------------------------------------
    // Problem<int> interface — delegate everything, intercept fitness
    // ----------------------------------------------------------------

    tFitness fitness(const tSolution<int> &solution) override {
        tFitness f = inner.fitness(solution);
        record(f);
        return f;
    }

    tFitness fitness(const tSolution<int> &solution,
                     SolutionFactoringInfo<int> *info,
                     unsigned pos_change, int new_value) override {
        tFitness f = inner.fitness(solution, info, pos_change, new_value);
        record(f);
        return f;
    }

    SolutionFactoringInfo<int> *
    generateFactoringInfo(const tSolution<int> &solution) override {
        return inner.generateFactoringInfo(solution);
    }

    void updateSolutionFactoringInfo(SolutionFactoringInfo<int> *info,
                                     const tSolution<int> &solution,
                                     unsigned pos_change,
                                     int new_value) override {
        inner.updateSolutionFactoringInfo(info, solution, pos_change, new_value);
    }

    tSolution<int> createSolution() override {
        return inner.createSolution();
    }

    size_t getSolutionSize() override {
        return inner.getSolutionSize();
    }

    std::pair<int, int> getSolutionDomainRange() override {
        return inner.getSolutionDomainRange();
    }

    bool isValid(const tSolution<int> &solution) override {
        return inner.isValid(solution);
    }

    bool isValid(const tSolution<int> &solution,
                 unsigned pos_change, int new_value) override {
        return inner.isValid(solution, pos_change, new_value);
    }

    std::string EvaluateSolution(tSolution<int> &solution) override {
        return inner.EvaluateSolution(solution);
    }

    void fix(tSolution<int> &solution) override {
        inner.fix(solution);
    }
};
