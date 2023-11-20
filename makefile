default:
	g++ -std=c++20 -o proyecto.out proyecto_nuevo.cpp
	/usr/bin/time -v ./proyecto.out genomas/invertebrate 0.3
	rm proyecto.out