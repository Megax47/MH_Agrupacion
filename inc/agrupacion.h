#pragma once

#include <problem.h>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <random.hpp>

struct Punto {
    std::vector<float> posicion;
    int dimension = 0;

    //Sobrecargar < para usar set
    bool operator<(const Punto& other) const {
        for(int i=0;i<posicion.size();++i){
            if (this->posicion.at(i) < other.posicion.at(i)) 
                return true;
        }
        return false;
    }

    std::string toString() {
        std::string ret;
        for(int i=0; i<dimension; ++i){
            ret += std::to_string(posicion[i]);
            ret += ",";
        }
        return ret;
    }

    float distanciaEuclidea(const Punto& other) const {
        assert(dimension == other.dimension);
        float sum = 0;
        float dif = 0;
        for(int i=0; i<dimension; ++i){ //?Desenrrollar
            dif = posicion[i]-other.posicion[i];
            sum += dif*dif;
        }
        return sqrt(sum);
    }
};

class Agrupacion: public Problem<int> {

    //Restricciones 
    std::vector<std::vector<short>> restricciones;
    std::vector<std::pair<int,int>> ML;
    std::vector<std::pair<int,int>> CL;

    //Puntos
    std::vector<Punto> puntos;
    int nPuntos;

    //Clusters
    short nCluster;

    //Parametros
    float lambda; //mayor distancia entre numero de restricciones

public:
    Agrupacion (const char* dataFilename, const char* constraintFilename);

    std::string toString(){
        std::string ret;
        ret += "Puntos:\n";
        for(int i=0; i<nPuntos; ++i){
            ret += puntos[i].toString();
            ret += "\n";
        }

        ret += "Restricciones\n";
        ret += "ML:\n";
        for(int i=0; i<ML.size(); ++i){
            ret += "(" + std::to_string(ML[i].first) + "," 
                + std::to_string(ML[i].second) + ")\n";  
        }

        ret += "CL:\n";
        for(int i=0; i<CL.size(); ++i){
            ret += "(" + std::to_string(CL[i].first) + "," 
                + std::to_string(CL[i].second) + ")\n";
        }

        ret += "Lambda = " + std::to_string(lambda) + "\n";
        
        return ret;
    }

    tFitness fitness(const tSolution<int> &solution);

    tFitness fitness(const tSolution<int> &solution,
                           SolutionFactoringInfo<int> *solution_info,
                           unsigned pos_change, int new_value) {
    auto newsol(solution);
    newsol[pos_change] = new_value;
    return fitness(newsol);
    }

    SolutionFactoringInfo<int> *
  generateFactoringInfo(const tSolution<int> &solution) {
    return new SolutionFactoringInfo<int>();
  }

    void updateSolutionFactoringInfo(SolutionFactoringInfo<int> *solution_info,
                                           const tSolution<int> &solution,
                                           unsigned pos_change,
                                           int new_value) {}

    tSolution<int> createSolution(){
        tSolution<int> solution(nPuntos);
        for (int i = 0; i < solution.size(); ++i) {
            solution[i] = Random::get<int>(0,nCluster);
        }
        return solution;
    }
    
    size_t getSolutionSize() {};

    std::pair<int, int> getSolutionDomainRange() {};

    bool isValid(const tSolution<int> &solution) {};

    void fix(tSolution<int> &solution) {};

};