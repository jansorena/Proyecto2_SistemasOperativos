#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <thread>
#include <semaphore>
#include <dirent.h>
#include <queue>

using namespace std;
std::counting_semaphore<1> sem(1);
queue<string> colaGenomas;

void leer_genoma_semaforo(string archivo, float umbral) {
    string filePath = archivo;
    ifstream inputFile(filePath); // Abrir el archivo
	//cout << "archivo a leer: "<< archivo << endl;
    if (!inputFile.is_open()) {
        cerr << "Error al abrir el archivo: " << filePath << endl;
		cerr << strerror(errno) << endl;
        return;
    }
    // Variables to guardar CGAT
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

    float calculoCG = float(countC+countG)/float(countCGAT);
    const float epsilon = 1e-6; 
    if (calculoCG - umbral < -epsilon) {
        cout << archivo << ": no entra" << endl;
    } else{ // mayor o igual
        sem.acquire();
        colaGenomas.push(archivo);
        cout << archivo << " % CG: " << calculoCG << endl;
        //cout << "Count of C: " << countC << endl;
        //cout << "Count of G: " << countG << endl;
        //cout << "Cantidad de bases nitrogenadas: " << countCGAT << endl;
        sem.release();
    }
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
    }
    for(auto &t : threads){
		t.join();
	}
    return 0;
}

