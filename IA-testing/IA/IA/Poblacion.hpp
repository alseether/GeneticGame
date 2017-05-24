#ifndef POBLACION_HPP
#define POBLACION_HPP

#include <vector>
#include <algorithm>
#include <functional>
#include <array>
#include <iterator>
#include "cromosoma.hpp"
#include "Mapa.hpp"

class poblacion {
public:
	int _tam;
	Cromosoma* individuos;

	poblacion() {}

	void generaPoblacionAleatoria(int tam, int profMin, int profMax) {
		this->_tam = tam;
		this->individuos = new Cromosoma[tam];
		for (std::size_t i = 0; i < tam; ++i) {
			this->individuos[i].crear(profMin, profMax);// = Cromosoma(profMin, profMax);
		}
	}

	void evalua(std::vector<Mapa> m, bool paralelizar) {
		for (std::size_t i = 0; i < _tam; ++i) {
			this->individuos[i].evalua(m, paralelizar);
		}
	}

	void evaluaMarcados(std::vector<Mapa> m, std::set<unsigned int> marcados, bool paralelizar) {
		for (auto it = marcados.begin(); it != marcados.end(); ++it){
			this->individuos[(*it)].evalua(m, paralelizar);
		}
	}

	void ordenar(){
		std::sort(individuos, individuos + _tam, [](Cromosoma a, Cromosoma b){
			return a.getAdaptacion() > b.getAdaptacion();
		});
	}

	void bloating(unsigned int maxNodos, std::set<unsigned int> marcados){
		for (size_t i = 0; i < _tam; ++i){
			if (individuos[i].bloating(maxNodos)){
				marcados.insert(i);
			}
		}
	}

	void eliminaIntrones(std::set<unsigned int> marcados){
		for (size_t i = 0; i < _tam; ++i){
			if (individuos[i].eliminaIntrones()){
				marcados.insert(i);
			}
		}
	}

	void addCromosomaObserver(ICromosomaObserver& o){
		for (std::size_t i = 0; i < _tam; ++i) {
			this->individuos[i].addObserver(o);
		}
	}
};

#endif