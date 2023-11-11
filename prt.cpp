#include <bits/stdc++.h>
#include <dirent.h>
#include <semaphore.h>
#include <pthread.h>
#include <string>

using namespace std;

struct Genoma{
	string nombre;
	float contCG;
};

queue <Genoma> colaSem;
sem_t sem_genoma;

queue <Genoma> colaVC;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void leer_genoma(string archivo) {
	string filePath = archivo;

    // Open the file
    ifstream inputFile(filePath);
	cout << "archivo a leer: "<< archivo << endl;
    // Check if the file is opened successfully
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
		cerr << "Error code: " << strerror(errno) << endl;
        return;
    }

    // Variables to store counts
    int countC = 0;
    int countG = 0;

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
    }

    // Close the file
    inputFile.close();

    // Output the results
    cout << "Count of C: " << countC << endl;
    cout << "Count of G: " << countG << endl;
}

int main(int argc, char const *argv[]){
	if(argc != 3){
		cout << "Fallo en ingreso de parametros" << endl;
		exit(EXIT_FAILURE);
	}
	char* directorio;
	float umbral = atof(argv[2]);
	DIR *dir;
  	struct dirent *diread;
 	vector<char *> files;

  	if ((dir = opendir(argv[1])) != nullptr) {
	    while ((diread = readdir(dir)) != nullptr) {
	     	files.push_back(diread->d_name);
	    }
	    closedir(dir);
	} else {
		perror("opendir");
	    return EXIT_FAILURE;
	}

  	for (auto file : files) cout << file << endl;
	cout << endl;
	
  	leer_genoma("GCF_000629185.1_Pseu_aeru_3580_V1_genomic.fna");

	/*ifstream gen(directorio);
	if(!gen.is_open()){
		cout << "Error al abrir " << directorio << endl;
		exit(EXIT_FAILURE);
	}
	string line;
	while(getline(gen,line)){
		cout << line << endl;		
	}
	cout << "hola grupo" << endl;*/

	return 0;
}
