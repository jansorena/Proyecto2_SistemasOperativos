#include <condition_variable>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <thread>
#include <semaphore>
#include <dirent.h>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;
#define epsilon 1e-6

// Variables Globales
std::counting_semaphore<1> sem(1);
queue<string> colaGenomas;
mutex mtx;
condition_variable cv;

float calcularCG(string fileName){
    ifstream inputFile(fileName);
	
    if (!inputFile.is_open()) {
        cerr << "Error al abrir el archivo: " << fileName << endl;
		cerr << strerror(errno) << endl;
        return 1;
    }

    // Variables para guardar bases nitrogenadas
    int countC = 0;
    int countG = 0;
    int countCGAT = 0;

    // Leer el archivo linea por linea
    string line;
    while (getline(inputFile, line)) {
        if (line[0] == '>') continue; // Saltar las lineas que empiezan con '>'

        for (char currentChar : line) {
            if (currentChar == 'C') countC++;
            else if (currentChar == 'G') countG++;
        }

        countCGAT += line.size();
    }

    inputFile.close(); // Cierra el archivo

    return float(countC+countG)/float(countCGAT);
}

void leerDirectorio(string directorio, vector<string> &genomas) {
	DIR *dir;
	struct dirent *diread;
	if ((dir = opendir(directorio.c_str())) != NULL) {
        while ((diread = readdir(dir)) != NULL) {
            // Filtra las entradas "." y ".."
            if (diread->d_name != string(".") && diread->d_name != string("..") && diread->d_type == DT_REG) {
                string s = diread->d_name;
                genomas.push_back(string(directorio) + "/" + s);
            } else if (diread->d_type == DT_DIR && diread->d_name != string(".") && diread->d_name != string("..")) {
				cout << "directorios: " << diread->d_name << endl;
				string subDirPath = directorio + "/" + diread->d_name;
                leerDirectorio(subDirPath, genomas);
			}
        }
        closedir(dir);
    } else {
        perror("opendir");
        return;
    }
}

void leer_genoma_semaforo(string archivo, float umbral) {
    float CG_ratio = calcularCG(archivo);

    if (CG_ratio - umbral < -epsilon) {
        cout << archivo << ": no entra" << endl;
        return;
    } 

    sem.acquire();
    colaGenomas.push(archivo);
    cout << archivo << " % CG: " << CG_ratio << endl;
    sem.release();
    
}

void leer_genoma_mutex_cv(string archivo, float umbral){
    float CG_ratio = calcularCG(archivo);

    if (CG_ratio - umbral < -epsilon) {
        cout << archivo << ": no entra" << endl;
        return;
    } 

    unique_lock<mutex> lck(mtx);
    colaGenomas.push(archivo);
    cout << archivo << " % CG: " << CG_ratio << endl;
    cv.notify_one();
}

int main(int argc, char const *argv[]){
    if(argc != 3){
		cout << "Fallo en ingreso de parametros" << endl;
		exit(EXIT_FAILURE);
	}
    float umbral = atof(argv[2]);
	char* directorio;
    vector<string> genomas;
	
    leerDirectorio(string(argv[1]),genomas);
	
    int genomaSize = genomas.size();
    vector <thread> threads;
    for(int i = 0; i < genomaSize; i++){
        //cout << genomas[i] << endl;
        threads.emplace_back(leer_genoma_semaforo, genomas[i], umbral);
        //threads.emplace_back(leer_genoma_mutex_cv, genomas[i], umbral);
    }
    for(auto &t : threads){
		t.join();
	}
    return 0;
}

