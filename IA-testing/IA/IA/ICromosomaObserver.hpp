#ifndef ICROMOSOMAOBSERVER_HPP
#define ICROMOSOMAOBSERVER_HPP


#include "npc.hpp"
#include "Mapa.hpp"
#include "Cromosoma.hpp"

class Cromosoma;

class ICromosomaObserver{
public:
	virtual ~ICromosomaObserver() {};

	virtual void onSimulacionIniciada(const Cromosoma*) = 0;

	virtual void onTurno(const Cromosoma*, npc, npc, Mapa, Mapa, Mapa, Mapa) = 0;	// jugador, enemigo, mapa, explorado, andado

	virtual void onMapaTerminado(double fitness, double factorPatrulla, int cExpl, int cAndadas, int turnosQueValen, double factorAtaque, int cAndadasAtaque, int golpesEvitados, int golpes, int encontradoAtaque, int turnosAtaque, int intentos, double distancia, int turnosGolpeo) = 0;

	virtual void onSimulacionTerminada(const Cromosoma*) = 0;
};

#endif