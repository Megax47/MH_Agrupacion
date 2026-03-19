#include <cassert>
#include <greedy.h>
#include <iostream>

using namespace std;

template <class T> void print_vector(string name, const vector<T> &sol) {
  cout << name << ": ";

  for (auto elem : sol) {
    cout << elem << ", ";
  }
  cout << endl;
}

/**
 * Create random solutions until maxevals has been achieved, and returns the
 * best one.
 *
 * @param problem The problem to be optimized
 * @param maxevals Maximum number of evaluations allowed
 * @return A pair containing the best solution found and its fitness
 */
ResultMHInt GreedySearch::optimize(ProblemInt &problem, int maxevals) {
  assert(maxevals > 0);
  Agrupacion realproblem = dynamic_cast<Agrupacion &>(problem);
  tSolution<int> sol(realproblem.getSolutionSize(), -1); //Inicializamos la solución con -1 (sin asignar)
  tSolution<int> sol_anterior(realproblem.getSolutionSize());

  //Genera nCluster centroides aleatorios
  int nCluster = realproblem.getSolutionDomainRange().second +1;
  int dimension = realproblem.getDimension();
  vector<Punto> centroides(nCluster);
  for(int i=0; i<nCluster; ++i){
    centroides[i].posicion.reserve(dimension);
    for(int j=0; j<dimension; ++j){
        centroides[i].posicion.push_back(
          Random::get<float>(realproblem.getSpaceLimits(j).first, realproblem.getSpaceLimits(j).second));
    }
  }

  //Genera un vector de indices y los baraja
  vector<int> indices(realproblem.getSolutionSize());
  for(int i=0; i<indices.size(); ++i) indices[i] = i;
  Random::shuffle(indices);

  //Estructuras auxiliares para el algoritmo

  std::vector<std::vector<short>> restricciones = realproblem.getMatrizRestricciones();
  std::vector<Punto> puntos = realproblem.getPuntos();

  //Bucle principal del algoritmo, se repite hasta que no haya cambios en la solución
  do{

    sol_anterior = sol;

    //De las asignaciones que violan menos restricciones, asigna el punto al cluster más cercano
    for(int i=0; i<indices.size(); ++i){
      int idx = indices[i];

      //Calculamos el número de restricciones incumplidas para cada cluster
      vector<int> infeasabilities(nCluster, 0);
      for(int j=0; j<nCluster; ++j){
          for(int k=0; k<indices.size(); ++k){
              if(restricciones[idx][k] == 1 && (sol[k] != j && sol[k] != -1)) { //Si hay restricción de ML y el punto k no está en el mismo cluster
                  infeasabilities[j]++;
              }
              else if(restricciones[idx][k] == -1 && sol[k] == j) { //Si hay restricción de CL y el punto k está en el mismo cluster
                  infeasabilities[j]++;
              }
          }
      } 
      //Buscamos el número mínimo de restricciones incumplidas
      int min_infeasability = infeasabilities[0];
      for(int j=1; j<nCluster; ++j){
          if(infeasabilities[j] < min_infeasability) min_infeasability = infeasabilities[j];
      }

      //De los clusters con el número mínimo de restricciones incumplidas, asignamos el punto al cluster más cercano
      int best_cluster = -1;
      float best_dist = std::numeric_limits<float>::max();
      for(int j=0; j<nCluster; ++j){
          if(infeasabilities[j] == min_infeasability){
              float dist = puntos[idx].distanciaEuclidea(centroides[j]);
              if(dist < best_dist){
                  best_dist = dist;
                  best_cluster = j;
              }
          }
      }
      sol[idx] = best_cluster;
    }

    //Actualizamos los centroides

    //Separar en clusters
    std::vector<std::vector<int>> clusters(nCluster);

    for(int i=0; i<realproblem.getSolutionSize(); ++i) clusters[sol[i]].push_back(i);

    //Calcular centróides
    for(int i=0; i<nCluster; ++i){ //Paralelizar este bucle

        for(int k=0; k<dimension; ++k) centroides[i].posicion[k] = 0;

        for(int j=0; j<clusters[i].size(); ++j){ //Acumular posiciones de los puntos del cluster i
            
            for(int k=0; k<dimension; ++k){
                centroides[i].posicion[k] += puntos[clusters[i][j]].posicion[k];
            }
        }

        for(int k=0; k<dimension; ++k){ //Dividir por el número de puntos del cluster i
            centroides[i].posicion[k] /= clusters[i].size();
        }
    }
  }while(sol != sol_anterior);
  
  tFitness fitness = problem.fitness(sol);
  return ResultMH(sol, fitness, 1);
}
