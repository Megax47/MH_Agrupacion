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
        for(int i=0; i<posicion.size(); ++i){
            ret += std::to_string(posicion[i]);
            ret += ",";
        }
        return ret;
    }

    float distanciaEuclidea(const Punto& other) const {
        assert(posicion.size() == other.posicion.size());
        float sum = 0;
        float dif = 0;
        for(int i=0; i<posicion.size(); ++i){ //?Desenrrollar
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
    Agrupacion (const char* dataFilename, const char* constraintFilename, int nCluster);

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
        do{
            for (int i = 0; i < solution.size(); ++i)
                solution[i] = Random::get<int>(0,nCluster-1);
        }
        while (!isValid(solution));

        return solution;
    }
    
    size_t getSolutionSize() { return nPuntos; }

    std::pair<int, int> getSolutionDomainRange() { 
        return std::make_pair(0, nCluster - 1); 
    }

    int getDimension() { return puntos[0].posicion.size(); }

    std::pair<float, float> getSpaceLimits(const int dimension) {
        float min = puntos[0].posicion[dimension];
        float max = puntos[0].posicion[dimension];
        for (int i = 1; i < nPuntos; ++i) {
            if (puntos[i].posicion[dimension] < min) {
                min = puntos[i].posicion[dimension];
            }
            if (puntos[i].posicion[dimension] > max) {
                max = puntos[i].posicion[dimension];
            }
        }
        return std::make_pair(min, max);
    }

    std::vector<std::vector<short>> getMatrizRestricciones() const { return restricciones; }

    std::vector<Punto> getPuntos() const { return puntos; }
    
    bool isValid(const tSolution<int> &solution) {
        //Comprobar que cada cluster tiene al menos un punto
        std::vector<bool> cluster_has_point(nCluster, false);
        for (int i = 0; i < solution.size(); ++i) {
            if (solution[i] < 0 || solution[i] >= nCluster) {
                return false; // Cluster fuera de rango
            }
            cluster_has_point[solution[i]] = true;
        }
        for (int i = 0; i < nCluster; ++i) {
            if (!cluster_has_point[i]) {
                return false; // Cluster sin puntos
            }
        }
        return true;
    }

    bool isValidChange(const tSolution<int> &solution, unsigned pos_change, int new_value) {
        // Comprobar que el nuevo valor es un cluster válido
        if (new_value < 0 || new_value >= nCluster) {
            return false; // Cluster fuera de rango
        }
        // Comprobar que el cambio no deja ningún cluster sin puntos
        int old_value = solution[pos_change];
        if (old_value == new_value) {
            return true; // No hay cambio, por lo que sigue siendo válido
        }
        std::vector<int> cluster_counts(nCluster, 0);
        for (int i = 0; i < solution.size(); ++i) {
            cluster_counts[solution[i]]++;
        }
        cluster_counts[old_value]--;
        cluster_counts[new_value]++;
        return cluster_counts[old_value] > 0; // El cluster antiguo aún tiene puntos
    }

    void fix(tSolution<int> &solution) {};

};