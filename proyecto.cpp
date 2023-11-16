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

int main(int argc, char const *argv[]){
    if(argc != 3){
		cout << "Fallo en ingreso de parametros" << endl;
		exit(EXIT_FAILURE);
	}
    float umbral = atof(argv[2]);
	char* directorio;
	DIR *dir;
  	struct dirent *diread;
    vector<string> genomas;
    if ((dir = opendir(argv[1])) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            // Filtra las entradas "." y ".."
            if (diread->d_name != string(".") && diread->d_name != string("..")) {
                string s = diread->d_name;
                genomas.push_back(string(argv[1]) + "/" + s);
            }
        }
        closedir(dir);
    } else {
        perror("opendir");
        return EXIT_FAILURE;
    }
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
