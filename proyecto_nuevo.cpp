#include <condition_variable>
#include <iostream>
#include <queue>
#include <fstream>
#include <dirent.h>
#include <mutex>
#include <thread>
#include <utility>
#include <semaphore>
#include <cstring>

using namespace std;
#define epsilon 1e-6

float calcularCG(string fileName);
void leerDirectorio(string directorio, vector<pair<string,string>> &genomas);

class ColaCompartidaMutex {
private:
    mutex mtx;
    condition_variable cv;
    queue<pair<string,float>> cola;
    bool terminado = false;
public:
    ColaCompartidaMutex() {}
    ~ColaCompartidaMutex() {}

    void pushGenoma(string directorio, string archivo, float umbral){
        float CG_ratio = calcularCG(directorio+archivo);

        if (CG_ratio - umbral < -epsilon) {
            return;
        } 

        unique_lock<mutex> lock(mtx);
        cola.push({archivo,CG_ratio});
        cv.notify_one();
    }

    void printGenoma(){
        unique_lock<mutex> lock(mtx);

        while (!terminado) {
            cv.wait(lock, [this] { return !cola.empty() || terminado; });
            if (!cola.empty()) {
                pair<string, float> genoma = cola.front();
                cola.pop();
                cout << genoma.first << " " << genoma.second << endl;
            }
        }
    }

    void terminar(){
        unique_lock<mutex> lock(mtx);
        terminado = true;
        cv.notify_all();
    }
};

class ColaCompartidaSemaforo {
private:
    counting_semaphore<1> sem;
    queue<pair<string,float>> cola;
    bool terminado = false;
public:
    ColaCompartidaSemaforo() : sem(1){}
    ~ColaCompartidaSemaforo() {}

    void pushGenoma(string directorio, string archivo, float umbral){
        float CG_ratio = calcularCG(directorio+archivo);

        if (CG_ratio - umbral < -epsilon) {
            return;
        } 

        sem.acquire();
        cola.push({archivo,CG_ratio});
        sem.release();
    }

    void printGenoma(){
        while(!terminado){
            if(!cola.empty()){
                sem.acquire();
                pair<string, float> genoma = cola.front();
                cola.pop();
                cout << genoma.first << " " << genoma.second << endl;
                sem.release();
            }
        }
    }

    void terminar(){
        terminado = true;
    }
};

int main(int argc, char const *argv[]){
    if(argc != 3){
		cout << "Fallo en ingreso de parametros" << endl;
		exit(EXIT_FAILURE);
    }
    float umbral = atof(argv[2]);
    if( 0 > umbral || umbral > 1){
        cout << "Error: Umbral invalido" << endl;
        exit(EXIT_FAILURE);
    }
	char* directorio;
    vector<pair<string,string>> genomas;

    leerDirectorio(string(argv[1]), genomas);

    int genomaSize = genomas.size();
    /*****************************************************************************************/
    cout << "Ingrese opcion, 1: Mutex, 2: Semaforo: ";
    int choice; cin >> choice;
    if (choice == 1){
        cout << "Cola Compartida Mutex" << endl;
        ColaCompartidaMutex ccm;
        thread printThreadMut(&ColaCompartidaMutex::printGenoma, &ccm);
        vector<thread> threads_ccm;
        for (int i = 0; i < genomaSize; i++) {
            threads_ccm.emplace_back(&ColaCompartidaMutex::pushGenoma, &ccm, genomas[i].first,genomas[i].second, umbral);
        }

        for (auto& t : threads_ccm) {
            t.join();
        }
        
        ccm.terminar();
        printThreadMut.join();
    }else{
        cout << "Cola Compartida Semaforo" << endl;
        ColaCompartidaSemaforo ccs;
        thread printThreadSem(&ColaCompartidaSemaforo::printGenoma, &ccs);
        vector<thread> threads_ccs;
        for (int i = 0; i < genomaSize; i++) {
            threads_ccs.emplace_back(&ColaCompartidaSemaforo::pushGenoma, &ccs, genomas[i].first,genomas[i].second, umbral);
        }

        for (auto& t : threads_ccs) {
            t.join();
        }

        ccs.terminar();
        printThreadSem.join();
    }

    return 0;
}

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

void leerDirectorio(string directorio, vector<pair<string,string>> &genomas) {
    DIR *dir;
    struct dirent *diread;
    if ((dir = opendir(directorio.c_str())) != NULL) {
        while ((diread = readdir(dir)) != NULL){
            // Filtra las entradas "." y ".."
            if (diread->d_name != string(".") && diread->d_name != string("..")) {
                string s = diread->d_name;
                genomas.push_back({directorio + "/",s});
            }
        }
        closedir(dir);
    } else {
        perror("opendir");
        return;
    }
}

/*
g++ programa2.cpp -o programa2
/usr/bin/time -v ./programa1 > output1.txt 2>&1
echo "Programa 1:"
cat output1.txt
*/