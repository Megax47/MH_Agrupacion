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
        for (int i = 0; i < posicion.size(); ++i) {
        if (posicion[i] < other.posicion[i]) return true;
        if (posicion[i] > other.posicion[i]) return false;
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

     float distanciaEuclidea2(const Punto& other) const {
        assert(posicion.size() == other.posicion.size());
        float sum = 0;
        float dif = 0;
        for(int i=0; i<posicion.size(); ++i){ //?Desenrrollar
            dif = posicion[i]-other.posicion[i];
            sum += dif*dif;
        }
        return sum;
    }
};

class AgrupacionFactoringInfo : public SolutionFactoringInfo<int> {

    int dimension;
    int penalizacion;
    std::vector<std::vector<int>> clusters;
    std::vector<Punto> centroides;

    friend class Agrupacion;

public:
    AgrupacionFactoringInfo(int dimension, int penalizacion, std::vector<std::vector<int>> clusters,
    std::vector<Punto> centroides) : dimension(dimension), penalizacion(penalizacion), clusters(clusters), centroides(centroides) {}
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
                           unsigned pos_change, int new_value);

    SolutionFactoringInfo<int> *
  generateFactoringInfo(const tSolution<int> &solution);

    void updateSolutionFactoringInfo(SolutionFactoringInfo<int> *solution_info,
                                           const tSolution<int> &solution,
                                           unsigned pos_change,
                                           int new_value);

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

    //Devuelve 0, nCluster
    std::pair<int, int> getSolutionDomainRange() { 
        return std::make_pair(0, nCluster); 
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

    bool isValid(const tSolution<int> &solution, unsigned pos_change, int new_value) {
        //Comprobar que el cluster al que se va a mover el punto tiene al menos un punto

        if (new_value < 0 || new_value >= nCluster) return false; // Cluster fuera de rango
        int old_value = solution[pos_change];
        for (int i = 0; i < solution.size(); ++i) {
            if (i != pos_change) {  
               if(solution[i] == old_value) return true; // El cluster al que se va a mover el punto tiene al menos un punto
            }
        }
        return false;
    }

    std::string EvaluateSolution(tSolution<int> &solution);

    void fix(tSolution<int> &solution) {
        //Anotar elementos por cluster
        std::vector<int> point_per_cluster(nCluster, 0);
        for (int i = 0; i < solution.size(); ++i) {
            ++point_per_cluster[solution[i]];
        }
        for (int i = 0; i < nCluster; ++i) {
            while (point_per_cluster[i] == 0) { //Cluster sin puntos
                int p = Random::get<int>(0,nPuntos-1);
                if(point_per_cluster[solution[p]] > 1){ //Garantizamos que arreglando uno no rompemos otro
                    int old_cluster = solution[p];
                    solution[p] = i;    //Actualizamos todo
                    --point_per_cluster[old_cluster];
                    ++point_per_cluster[i];
                }
            }
        }
        
    }

    int getPenalizacion(const tSolution<int> &solution) const{
        int penalizacion = 0;
        for(int i=0; i<ML.size(); ++i){
            auto ML_pair = ML[i];
            if(solution[ML_pair.first] != solution[ML_pair.second]) ++penalizacion;
        }
        for(int i=0; i<CL.size(); ++i){
            auto CL_pair = CL[i];
            if(solution[CL_pair.first] == solution[CL_pair.second]) ++penalizacion;
        }
        return penalizacion;
    }

    //Calcula la diferencia en la penalización por restricciones incumplidas de un solo punto, para usar en el factoring
    int getDiferenciaPenalizacion(const tSolution<int> &solution, unsigned pos_change, int new_value) const {
        int penalizacion = 0;
        int old_value = solution[pos_change];
        for(int i=0; i<nPuntos; ++i){
            if(i != pos_change){
                if(restricciones[pos_change][i] == -1){ // CL
                    if(solution[i] == new_value) ++penalizacion;
                    if(solution[i] == old_value) --penalizacion;
                }
                else if(restricciones[pos_change][i] == 1){ // ML
                    if(solution[i] != new_value) ++penalizacion;
                    if(solution[i] != old_value) --penalizacion;
                }
            }
        }
        return penalizacion;
    }
};