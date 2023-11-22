# Proyecto2_SistemasOperativos

Para compilar el código, se debe realizar make en el directorio correspondiente:

	$ make

En caso de no contar con makefile utilizar lo siguiente:

	$ g++ -std=c++20 -o proyecto.out proyecto_nuevo.cpp
Y ejecutar con:

	$ /usr/bin/time -v ./proyecto.out [directorio] [umbral]

 En donde "directorio" corresponde a alguno de los directorios con genomas (genomas, bacterias, invertebrate, sample) y "umbral" corresponde a un float entre 0 y 1.
 
