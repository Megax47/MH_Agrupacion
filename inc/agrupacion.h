#pragma once

#include <problem.h>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>

struct Punto {
    std::vector<float> posicion;
    int dimension;

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
};

class Agrupacion: public Problem<int> {

    //Restricciones (Matriz o lista?) matriz
    std::vector<std::vector<short>> restricciones;

    //Puntos
    std::vector<Punto> puntos;
    int nPuntos;

    //Clusters
    std::vector<short> cluster;
    short nCluster;

public:
    Agrupacion (const char* dataFilename, const char* constraintFilename);

    std::string toString(){
        std::string ret;
        ret += "Puntos:\n";
        for(int i=0; i<nPuntos; ++i){
            ret += puntos[i].toString();
            ret += "\n";
        }

        ret += "Matriz de restricciones\n";
        for(int i=0; i<nPuntos; ++i){
            for(int j=0; j<nPuntos; ++j){
                ret += std::to_string(restricciones[i][j]);
                ret += ",";
            }
            ret += "\n";
        }
        
        return ret;
    }

    
};