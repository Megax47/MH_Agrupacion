#include <iostream>
#include <problem.h>
#include <random.hpp>
#include <string>
#include <util.h>
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
  LocalSearch<int> rlocal = LocalSearch<int>();
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
    ResultMH result = mh->optimize(*problem, 5000);
    cout << "Best solution: " << result.solution << endl;
    cout << "Best fitness: " << result.fitness << endl;
    cout << "Evaluations: " << result.evaluations << endl;
  }

  return 0;
}
