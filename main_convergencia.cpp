#include <iostream>
#include <fstream>
#include <problem.h>
#include <random.hpp>
#include <string>
#include <util.h>
#include <chrono>

// Real problem class
#include "agrupacion.h"

// All algorithms
#include "greedy.h"
#include "randomsearch.h"
#include "localsearch.h"
#include "genetic.h"
#include "memetic.h"
#include "trajectory.h"

// Convergence logger
#include "problem_logger.h"

using namespace std;

int main(int argc, char *argv[]) {
  
  long int seed;
  if(argc < 4){
    cerr << "Usage: " << argv[0] << " <data_file> <constraint_file> <nCluster> <seed>(optional)" << endl;
    return 1;
  }
  seed = (argc == 4) ? 42 : atoi(argv[4]);

  // Create algorithms
  RandomSearch<int> ralg;
  GreedySearch     rgreedy;
  LocalSearch      rlocal;
  AM_Rand          am_rand;
  BMB_Unif         bmb_unif;
  BMB_NoUnif       bmb_NoUnif;
  ILS              ils;
  ILS_ES           ils_es;
  ES               es;
  ES               es_t(0.001);

  Agrupacion rproblem(argv[1], argv[2], atoi(argv[3]));
  Problem<int> *base_problem = dynamic_cast<Problem<int> *>(&rproblem);

  vector<pair<string, MH<int> *>> algoritmos = {
    {"LocalSearch",  &rlocal},
    {"AM_Rand",      &am_rand},
    {"BMB_Unif",     &bmb_unif},
    {"BMB_NoUnif",   &bmb_NoUnif},
    {"ILS",          &ils},
    {"ILS_ES",       &ils_es},
    {"ES",           &es},
    {"ES(T=0.001)",  &es_t}
  };

  // ---- Open convergence CSV ----
  ofstream csv("convergence.csv");
  csv << "algorithm,evaluation,best_fitness\n";

  // ---- Run each algorithm ----
  // cout << "Algorithm,Fitness,Evaluations,Time(s)" << endl;
  ProblemLogger logger(*base_problem);

  for (auto &[name, mh] : algoritmos) {
    Random::seed(seed);
    logger.reset();

    auto t0 = chrono::high_resolution_clock::now();
    ResultMH result = mh->optimize(logger, 100000);
    auto t1 = chrono::high_resolution_clock::now();

    // Standard output (same format as before)
    cout << name << ",";
    cout << base_problem->EvaluateSolution(result.solution) << ",";
    cout << result.evaluations << ",";
    cout << chrono::duration_cast<chrono::milliseconds>(t1-t0).count()/1000.0 << "\n";

    // Write convergence data
    for (auto &[eval, fit] : logger.getHistory()) {
      csv << name << "," << eval << "," << fit << "\n";
    }
  }

  csv.close();
  cout << "\n[INFO] Convergence data written to convergence.csv\n";

  return 0;
}
