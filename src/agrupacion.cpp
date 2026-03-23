#include <agrupacion.h>

Agrupacion::Agrupacion (const char* dataFilename, const char* constraintFilename, int nCluster) : nCluster(nCluster) {
    std::ifstream file;

    //Lectura Datos

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
                newPunto.posicion.push_back(stof(word));
            }
            ++nPuntos;
            puntos.push_back(newPunto);
        }
    }

    file.close();
    file.clear();

    //Lectura restricciones

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
            j=0;
            if(line.empty() || line.find_last_not_of(" \r\n\t") == std::string::npos) continue;
            std::stringstream s(line);
            while(getline(s, word, ',')){
                restricciones[i][j] = stoi(word);
                ++j;
            }
            ++i;
        }
    }

    file.close();

    //Pasar matriz a lista

    for(int i=0; i<nPuntos; ++i){
        for(int j=i+1; j<nPuntos; ++j){
            std::pair<int,int> rest(i,j);
            //Inicializar listas aquí ¿¿¿ y pasar de la matriz??
            if(restricciones[i][j] == -1) CL.push_back(rest);
            else if(restricciones[i][j] == 1) ML.push_back(rest);
        }
    }

    //Calcular Lambda

        //Distancia maxima (¿Precalcular matriz de distancias?)
    float max_dist = 0;
    float dist_act = 0;
    for(int i=0; i<nPuntos; ++i){
        for(int j=i+1; j<nPuntos; ++j){
            dist_act = puntos[i].distanciaEuclidea(puntos[j]);
            if(dist_act > max_dist) max_dist = dist_act; 
        }
    }

    lambda = max_dist/(ML.size()+CL.size());

    //std::cout << max_dist << " , " << ML.size()+CL.size() << std::endl;
}

tFitness Agrupacion::fitness(const tSolution<int> &solution){

    float fitness = 0;

    int dimension = getDimension();

    //Separar en clusters
    std::vector<std::vector<int>> clusters(nCluster);
    for(int i=0; i<nPuntos; ++i) clusters[solution[i]].push_back(i);

    //Calcular centróides
    std::vector<Punto> centroides(nCluster);
    for(int i=0; i<nCluster; ++i){ //Paralelizar este bucle

        for(int k=0; k<dimension; ++k) centroides[i].posicion.push_back(0); //Inicializar posiciones de los centroides a 0

        for(int j=0; j<clusters[i].size(); ++j){ //Acumular posiciones de los puntos del cluster i
            
            for(int k=0; k<dimension; ++k){
                centroides[i].posicion[k] += puntos[clusters[i][j]].posicion[k];
            }
        }
        for(int k=0; k<dimension; ++k){ //Dividir por el número de puntos del cluster i
            centroides[i].posicion[k] /= clusters[i].size();
        }
    }

    //Calcular distancia intra-cluster
    std::vector<float> dist_intra(nCluster, 0);
    for(int i=0; i<nPuntos; ++i){
        dist_intra[solution[i]] += puntos[i].distanciaEuclidea(centroides[solution[i]]);
    }
    for(int i=0; i<nCluster; ++i){
        dist_intra[i] /= clusters[i].size();
    }

    //Calcular desviacion media
    float desviacion_media = 0;
    for(int i=0; i<nCluster; ++i){
        desviacion_media += dist_intra[i];
    }
    desviacion_media /= nCluster;

    //Calcular penalización por restricciones incumplidas
    int penalizacion = 0;
    for(int i=0; i<ML.size(); ++i){
        if(solution[ML[i].first] != solution[ML[i].second]) ++penalizacion;
    }
    for(int i=0; i<CL.size(); ++i){
        if(solution[CL[i].first] == solution[CL[i].second]) ++penalizacion;
    }

    fitness += desviacion_media + lambda * penalizacion;

    return fitness;
}

tFitness Agrupacion::fitness(const tSolution<int> &solution,
                           SolutionFactoringInfo<int> *solution_info,
                           unsigned pos_change, int new_value){
    auto newsol(solution);
    newsol[pos_change] = new_value;
    return fitness(newsol);
}

std::string Agrupacion::EvaluateSolution(tSolution<int> &solution){
    float fitness = this->fitness(solution);
    //Calculamos el número de restricciones incumplidas como media
    float penalizacion = 0;
    for(int i=0; i<ML.size(); ++i){
        if(solution[ML[i].first] != solution[ML[i].second]) ++penalizacion;
    }
    for(int i=0; i<CL.size(); ++i){
        if(solution[CL[i].first] == solution[CL[i].second]) ++penalizacion;
    }
    penalizacion = penalizacion/(ML.size()+CL.size());

    int dimension = getDimension();

    //Separar en clusters
    std::vector<std::vector<int>> clusters(nCluster);
    for(int i=0; i<nPuntos; ++i) clusters[solution[i]].push_back(i);

    //Calcular centróides
    std::vector<Punto> centroides(nCluster);
    for(int i=0; i<nCluster; ++i){ //Paralelizar este bucle

        for(int k=0; k<dimension; ++k) centroides[i].posicion.push_back(0); //Inicializar posiciones de los centroides a 0

        for(int j=0; j<clusters[i].size(); ++j){ //Acumular posiciones de los puntos del cluster i
            
            for(int k=0; k<dimension; ++k){
                centroides[i].posicion[k] += puntos[clusters[i][j]].posicion[k];
            }
        }
        for(int k=0; k<dimension; ++k){ //Dividir por el número de puntos del cluster i
            centroides[i].posicion[k] /= clusters[i].size();
        }
    }

    //Calcular distancia intra-cluster
    std::vector<float> dist_intra(nCluster, 0);
    for(int i=0; i<nPuntos; ++i){
        dist_intra[solution[i]] += puntos[i].distanciaEuclidea(centroides[solution[i]]);
    }
    for(int i=0; i<nCluster; ++i){
        dist_intra[i] /= clusters[i].size();
    }

    //Calcular desviacion media
    float desviacion_media = 0;
    for(int i=0; i<nCluster; ++i){
        desviacion_media += dist_intra[i];
    }
    desviacion_media /= nCluster;

    return "Fitness: " + std::to_string(fitness) 
    + "\nPenalización: " + std::to_string(penalizacion)
    + "\nDistancia media: " + std::to_string(desviacion_media);
}