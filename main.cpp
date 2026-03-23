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
  // Create the specific problem
  Agrupacion rproblem = Agrupacion(argv[1], argv[2], atoi(argv[3]));
  // Solve using evaluations
  vector<pair<string, MH<int> *> > algoritmos = {make_pair("RandomSearch", &ralg),
                                           make_pair("Greedy", &rgreedy),
                                           make_pair("LocalSearch", &rlocal)};
  Problem<int> *problem = dynamic_cast<Problem<int> *>(&rproblem);

  for (int i = 0; i < algoritmos.size(); i++) {
    Random::seed(seed);
    cout << algoritmos[i].first << endl;
    auto mh = algoritmos[i].second;
    auto momentoInicio = std::chrono::high_resolution_clock::now();
    ResultMH result = mh->optimize(*problem, 100000);
    auto momentoFin = std::chrono::high_resolution_clock::now();
    //cout << "Best solution: " << result.solution << endl;
    //cout << "Is valid: " << problem->isValid(result.solution) << endl;
    cout << problem->EvaluateSolution(result.solution) << endl;
    cout << "Evaluations: " << result.evaluations << endl;
    std::chrono::milliseconds tiempo = std::chrono::duration_cast<std::chrono::milliseconds>(momentoFin - momentoInicio);
    cout <<"Tiempo Pasado: " <<tiempo.count() <<endl;
    cout << "-----------------------------" << endl;
  }

  return 0;
}
