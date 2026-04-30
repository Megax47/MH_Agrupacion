#include <iostream>
#include <problem.h>
#include <random.hpp>
#include <string>
#include <util.h>
#include <chrono>

// Real problem class
#include "agrupacion.h"

// All all algorithms
#include "greedy.h"
#include "randomsearch.h"
#include "localsearch.h"
#include "genetic.h"

using namespace std;

int main(int argc, char *argv[]) {
  
  long int seed;
  // Seed the random number generator
  if(argc < 4){
    cerr << "Usage: " << argv[0] << " <data_file> <constraint_file> <nCluster> <seed>(optional)" << endl;
    return 1;
  }
  if (argc == 4) {
    // Use a fixed seed for reproducibility
    seed = 42;

  } else {
    seed = atoi(argv[4]);
  }

  // Create the algorithms
  RandomSearch<int> ralg = RandomSearch<int>();
  GreedySearch rgreedy = GreedySearch();
  LocalSearch rlocal = LocalSearch();
  AGE<UniformCO> age_un = AGE<UniformCO>();
  AGE<FixSegmentCO> age_sf = AGE<FixSegmentCO>();
  AGG<UniformCO> agg_un = AGG<UniformCO>();
  AGG<FixSegmentCO> agg_sf = AGG<FixSegmentCO>();

  // Create the specific problem
  Agrupacion rproblem = Agrupacion(argv[1], argv[2], atoi(argv[3]));
  // Solve using evaluations
  vector<pair<string, MH<int> *> > algoritmos = {make_pair("RandomSearch", &ralg),
                                           make_pair("Greedy", &rgreedy),
                                           make_pair("LocalSearch", &rlocal),
                                           make_pair("AGE_UN", &age_un),
                                           make_pair("AGE_SF", &age_sf),
                                           make_pair("AGG_UN", &agg_un),
                                           make_pair("AGG_SF", &agg_sf)};
  Problem<int> *problem = dynamic_cast<Problem<int> *>(&rproblem);
  //cout << "Algorithm,Fitness,Evaluations,Time(s)" << endl;
  for (int i = 0; i < algoritmos.size(); i++) {
    Random::seed(seed);
    cout << algoritmos[i].first << ",";
    auto mh = algoritmos[i].second;
    auto momentoInicio = std::chrono::high_resolution_clock::now();
    ResultMH result = mh->optimize(*problem, 10000);
    auto momentoFin = std::chrono::high_resolution_clock::now();
    //cout << "Best solution: " << result.solution << endl;
    //cout << "Is valid: " << problem->isValid(result.solution) << endl;
    cout << problem->EvaluateSolution(result.solution) << ",";
    cout << result.evaluations << ",";
    std::chrono::milliseconds tiempo = std::chrono::duration_cast<std::chrono::milliseconds>(momentoFin - momentoInicio);
    cout <<tiempo.count()/1000.0 <<endl;
  }

  return 0;
}
