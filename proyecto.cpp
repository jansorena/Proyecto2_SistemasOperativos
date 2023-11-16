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

    // Open the file
    ifstream inputFile(filePath);
	//cout << "archivo a leer: "<< archivo << endl;
    // Check if the file is opened successfully
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
		cerr << "Error code: " << strerror(errno) << endl;
        return;
    }

    // Variables to store counts
    int countC = 0;
    int countG = 0;
    int countCGAT = 0;

    // Read the file line by line
    string line;
    while (getline(inputFile, line)) {
        // Check if the line starts with '>'
        if (line[0] == '>') {
            // Ignore this line
            continue;
        }
        // Iterate through the characters in the line
        for (char currentChar : line) {
            // Check if the character is 'C' or 'G' and update counts
            if (currentChar == 'C') {
                countC++;
            } else if (currentChar == 'G') {
                countG++;
            }
        }
        countCGAT += line.size();
    }

    // Cierra el archivo
    inputFile.close();

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
        threads.emplace_back(leer_genoma_semaforo, genomas[i],umbral);
    }

    for(auto &t : threads){
		t.join();
	}
    
    

    return 0;
}
