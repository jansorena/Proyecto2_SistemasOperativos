#include <bits/stdc++.h>
#include <dirent.h>
#include <semaphore.h>
#include <pthread.h>

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

  	for (auto file : files) cout << file << "| ";
  	cout << endl;

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