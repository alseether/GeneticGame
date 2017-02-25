#ifndef METODOMUTACION_HPP
#define METODOMUTACION_HPP

#include "RandomGen.hpp"
#include "Cromosoma.hpp"

class MetodoMutacion {
public:

	virtual void mutar(Cromosoma* c, ParametrosEval p) = 0;

};

class MutacionArista : public MetodoMutacion {
public:
	void mutar(Cromosoma* c, ParametrosEval p){
		Grafo<Gen> g = c->getGenotipo();
		auto ady = g.getAdyacencia();
		unsigned int randNodo, randArista;
		unsigned int vecinos;
		unsigned intentos = 5;	// Se limita el numero de intentos por si entrase un grafo totalmente inconexo
		
		if (0){//RandomGen::getRandom(0u, 1u)){
			// Se crea una arista aleatoria
		}
		else{
			// Se borra una arista aleatoria
			do{
				randNodo = RandomGen::getRandom(0u, ady.size()-1);
				vecinos = ady[randNodo].size();
				if (vecinos > 0){
					randArista = RandomGen::getRandom(0u, vecinos-1);
					auto otroNodo = ady[randNodo].begin();
					std::advance(otroNodo, randArista);
					unsigned int idOtroNodo = *otroNodo;
					ady[randNodo].erase(otroNodo);
					auto esteNodo = ady[idOtroNodo].find(randNodo);
					ady[idOtroNodo].erase(esteNodo);
				}
				else{
					intentos--;
				}
			} while (vecinos == 0 && intentos > 0);
		}
		g.setAdyacencia(ady);
		c->setGenotipo(g,p);	
	}

private:
	

};

#endif