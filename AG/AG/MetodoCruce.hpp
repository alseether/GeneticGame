#ifndef METODOCRUCE_HPP
#define METODOCRUCE_HPP

#include "RandomGen.hpp"
#include"Cromosoma.hpp"

class MetodoCruce {
public:

	virtual void cruzar(Cromosoma*, Cromosoma*, ParametrosEval p) = 0;

};

class CruceMonopunto : public MetodoCruce{
public:
	void cruzar(Cromosoma* a, Cromosoma* b, ParametrosEval p){
		Grafo<Gen> grafoA = a->getGenotipo();
		Grafo<Gen> grafoB = a->getGenotipo();
		std::vector<Grafo<Gen>> subsA = grafoA.divideGrafo(RandomGen::getRandom(1u, grafoA.size() - 2));	// comienza en 1 y termina en size-2 para evitar subgrafos sin nodos
		std::vector<Grafo<Gen>> subsB = grafoB.divideGrafo(RandomGen::getRandom(1u, grafoB.size() - 2));

		Grafo<Gen> hijo1 = Grafo<Gen>::unirGrafo(subsA[0], subsB[1]);
		Grafo<Gen> hijo2 = Grafo<Gen>::unirGrafo(subsB[0], subsA[1]);

		// Valorar si se debe comprabar que los hijos sean mejores que los padres.

		a->setGenotipo(hijo1,p);
		b->setGenotipo(hijo2,p);
	}

private:
};

#endif