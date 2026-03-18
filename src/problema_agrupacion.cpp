#include "agrupacion.h"

Agrupacion::Agrupacion (const char* dataFilename, const char* constraintFilename){
    std::ifstream file;
    file.open(dataFilename);
    nPuntos = 0;
    if(!file){

        std::string err;
        std::string errFile = dataFilename;

        err += "Agrupacion\n";
        err += "No se ha podido abrir el archivo " + errFile + "\n";
        throw std::ios_base::failure(err);
    }
    else{
        std::string line, word;
        while(getline(file, line)){
            if(line.empty() || line.find_last_not_of(" \r\n\t") == std::string::npos) continue;
            Punto newPunto;
            std::stringstream s(line);
            while(getline(s, word, ',')){
                ++newPunto.dimension;
                newPunto.posicion.push_back(stof(word));
            }
            cluster.push_back(-1);
            ++nPuntos;
            puntos.push_back(newPunto);
        }
    }

    file.close();
    file.clear();
    file.open(constraintFilename);

    if(!file){

        std::string err;
        std::string errFile = constraintFilename;

        err += "Agrupacion\n";
        err += "No se ha podido abrir el archivo " + errFile + "\n";
        throw std::ios_base::failure(err);
    }
    else{
        std::string line, word;
        int i = 0;
        int j = 0;
        restricciones.resize(nPuntos);
        for(int i=0; i<nPuntos; ++i) restricciones[i].resize(nPuntos);

        while(getline(file, line)){
            if(line.empty() || line.find_last_not_of(" \r\n\t") == std::string::npos) continue;
            std::stringstream s(line);
            while(getline(s, word, ',')){ //Se puede hacer mas eficiente (matriz simetrica)
                restricciones[i][j] = stoi(word);
                ++j;
            }
            j=0;
            ++i;
        }
    }

    file.close();
}
