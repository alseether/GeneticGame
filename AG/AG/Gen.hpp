#ifndef GEN_HPP
#define GEN_HPP


#include <time.h>
#include "RandomGen.hpp"

class Gen{
public:
	Gen(){
		std::srand(time(NULL));
		_ancho = RandomGen::getRandom(_MINSIZE, _MAXSIZE);
		_alto = RandomGen::getRandom(_MINSIZE, _MAXSIZE);
		_nEnemigos = RandomGen::getRandom(_MINENEMIGOS, _MAXENEMIGOS);
		_nCofres = RandomGen::getRandom(_MINCOFRES, _MAXCOFRES);
	}

	Gen& operator=(Gen other){
		_ancho = other._ancho;
		_alto = other._alto;
		_nEnemigos = other._nEnemigos;
		_nCofres = other._nCofres;
		return *this;
	}

	unsigned int getAlto(){
		return _alto;
	}

	unsigned int getAncho(){
		return _ancho;
	}

	unsigned int getEnemigos(){
		return _nEnemigos;
	}

	unsigned int getCofres(){
		return _nCofres;
	}

private:
	unsigned int _ancho;
	unsigned int _alto;
	unsigned int _nEnemigos;
	unsigned int _nCofres;
	const unsigned int _MAXSIZE = 40;
	const unsigned int _MINSIZE = 5;
	const unsigned int _MINENEMIGOS = 0;
	const unsigned int _MAXENEMIGOS = 4;
	const unsigned int _MINCOFRES = 0;
	const unsigned int _MAXCOFRES = 2;
};

#endif