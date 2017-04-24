#ifndef CONTROLADOR_HPP
#define CONTROLADOR_HPP

#include "AG.hpp"
#include "IAGObserver.hpp"
#include "ICromosomaObserver.hpp"

class Controlador {
public:
	Controlador(AG& ag)
	{
		_ag = &ag;
	}

	Cromosoma run(){
		return _ag->ejecuta();
	}

	void addObserver(IAGObserver& o){
		_ag->addObserver(o);
	}

	void addCromosomaObserver(ICromosomaObserver& o){
		_ag->addCromosomaObserver(o);
	}

private:
	AG* _ag;

};

#endif