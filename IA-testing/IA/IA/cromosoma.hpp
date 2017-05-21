#ifndef CROMOSOMA_HPP
#define CROMOSOMA_HPP

#define NOMINMAX

#include <iostream>
#include <stack>
#include <algorithm>
#include <Windows.h>
#include "Arbol.hpp"
#include "myrandom.hpp"
#include "ICromosomaObserver.hpp"

class Cromosoma {
public:

	Cromosoma() {
		_punt = 0;
		_puntAcum = 0;
		_adaptacion = 0;
		double pesos[6] = { 0.2, 0.1, 0.35, 0.05, 0.2, 0.05 };
		for (size_t i = 0; i < 6; ++i){
			_pesos[i] = pesos[i];
			_mediaValores[i] = 0;
		}

		_descartado = false;
	}

	Cromosoma(int profMin, int profMax) {
		this->_genotipo[0].creaArbolAleatorio(profMin, profMax, TipoArbol::Patrulla);
		this->_genotipo[1].creaArbolAleatorio(profMin, profMax, TipoArbol::Ataque);
		double pesos[6] = { 0.2, 0.1, 0.35, 0.05, 0.2, 0.05 };
		for (size_t i = 0; i < 6; ++i){
			_pesos[i] = pesos[i];
			_mediaValores[i] = 0;
		}

		_descartado = false;
	}

	void crear(int profMin, int profMax){
		this->_genotipo[0].creaArbolAleatorio(profMin, profMax, TipoArbol::Patrulla);
		this->_genotipo[1].creaArbolAleatorio(profMin, profMax, TipoArbol::Ataque);
		double pesos[6] = { 0.2, 0.1, 0.35, 0.05, 0.2, 0.05 };
		for (size_t i = 0; i < 6; ++i){
			_pesos[i] = pesos[i];
			_mediaValores[i] = 0;
		}

		_descartado = false;
	}

	Arbol getGenotipo(int pos) const{
		return _genotipo[pos];
	}

	void setGenotipo(Arbol genotipo, int pos) {
		this->_genotipo[pos] = genotipo;
		this->_genotipo[pos].actualizaNumNodos();
	}

	double getPunt() const{
		return _punt;
	}

	void setPunt(double punt) {
		this->_punt = punt;
	}

	double getPuntAcum() const{
		return this->_puntAcum;
	}

	void setPuntAcum(double p) {
		this->_puntAcum = p;
	}

	double getAdaptacion() const{
		return this->_adaptacion;
	}

	void setAdaptacion(double a) {
		this->_adaptacion = a;
	}

	Nodo getTerminalAleatorio(int pos) {
		return _genotipo[pos].getTerminalAleatorio();
	}

	Nodo getNodoFuncionAleatorio(int pos) {
		return _genotipo[pos].getNodoFuncionAleatorio();
	}

	bool bloating(int prof) {
		bool cambios = false;
		for (std::size_t i = 0; i < 2; ++i) {
			cambios |= _genotipo[i].bloating(prof, (TipoArbol)i);
		}
		return cambios;
	}

	double evalua(std::vector<Mapa> maps) {
		int x, y;
		double actual = 1;
		double media = 0;
		_descartado = false;
		notifySimulacionInciada();
		for (std::size_t i = 0; i < maps.size() && actual != 0; ++i) {
			x = maps[i].getX();
			y = maps[i].getY();
			if (i == 0){
				actual = evaluaMapa(maps[i], x, y);
				media += actual;
			}
			else{
				media += evaluaMapa(maps[i], x, y);
			}
		}

		_mediaValores[0] /= maps.size();
		_mediaValores[1] /= maps.size();
		_mediaValores[2] /= maps.size();
		_mediaValores[3] /= maps.size();
		_mediaValores[4] /= maps.size();

		if (actual != 0){
			this->_adaptacion = media / maps.size();
			_descartado = false;
		}
		else{
			this->_adaptacion = 0;
			_descartado = true;
		}

		

		notifySimulacionTerminada();
		return _adaptacion;
	}

	bool eliminaIntrones() {
		bool cambios = false;
		for (std::size_t i = 0; i < 2; ++i) {
			cambios |= _genotipo[i].eliminaIntrones((TipoArbol)i);
		}
		return cambios;
	}

	const double* getMediaValores() const{
		return _mediaValores;
	}

	const double* getPesos() const{
		return _pesos;
	}

	bool getDescartado() const{
		return _descartado;
	}

	void addObserver(ICromosomaObserver& obs){
		_obs.push_back(&obs);
	}

	void getCopia(Cromosoma &ret) const{
		Arbol ap, aa;
		_genotipo[0].getCopia(ap);
		_genotipo[1].getCopia(aa);
		ap.reparaReferencias();
		aa.reparaReferencias();
		ret.setGenotipo(ap, 0);
		ret.setGenotipo(aa, 1);
		ret.setPunt(_punt);
		ret.setPuntAcum(_puntAcum);
		ret.setAdaptacion(_adaptacion);
		for (size_t i = 0; i < _obs.size(); ++i){
			ret.addObserver(*(_obs[i]));
		}
	}

private:

	double evaluaMapa(Mapa m, int posX, int posY) {
		double dim = m.getHeight() * m.getWidth();
		int maxTurnos = (dim*30/100); //Los turnos son el 30% del total de casillas del mapa

		std::stack<Nodo*> pila;
		Mapa explorado = m;
		Mapa andado = m;
		Mapa andadoAtaque = m;
		npc jugador(posX, posY, m.getHeight(), m.getWidth());
		npc enemigo(0, 0, m.getHeight(), m.getWidth());
		setPositionInRange(jugador, enemigo, 5, 10, m);
		bool ataque = false;
		Nodo* actual;
		int x = 0, y = 0;
		int incx = 0, incy = 0;
		bool fin = false;
		int cont = 0;
		int turnosIni;

		bool siDetectado = false;
		bool encontrado = false;

		while (enemigo.turnos < maxTurnos && !ataque && enemigo.heridas < 3) {
			if (pila.empty()) {
				pila.push(_genotipo[0].getRaiz());
				siDetectado = false;
			}
			turnosIni = enemigo.turnos;
			actual = pila.top();
			pila.pop();
			Operacion op = actual->getElem();
			switch (op) {
			case ProgN2:
				pila.push(&actual->getHijos()[1]); //Se introducen en orden inverso que es como se quieren ejecutar.
				pila.push(&actual->getHijos()[0]);
				break;
			case ProgN3:
				pila.push(&actual->getHijos()[2]);
				pila.push(&actual->getHijos()[1]); //Se introducen en orden inverso que es como se quieren ejecutar.
				pila.push(&actual->getHijos()[0]);
				break;
			/*case ProgN4:
				pila.push(&actual->getHijos()[3]);
				pila.push(&actual->getHijos()[2]);
				pila.push(&actual->getHijos()[1]); //Se introducen en orden inverso que es como se quieren ejecutar.
				pila.push(&actual->getHijos()[0]);
				break;*/
			case SiBloqueado:
				x = 0; y = 0;
				if (!enemigo.getCasillaDelante(x, y)) {
					pila.push(&actual->getHijos()[0]);
				}
				else if (m.estaBloqueado(x, y)) {
					pila.push(&actual->getHijos()[0]);
				}
				else {
					pila.push(&actual->getHijos()[1]);
				}
				break;
			case SiDetectado:
				x = 0; y = 0;
				incx = 0; incy = 0;
				fin = false;
				if (enemigo.getCasillaDelante(x, y)) {
					switch (enemigo.f) {
					case NORTE:
						incy--;
						break;
					case SUR:
						incy++;
						break;
					case ESTE:
						incx++;
						break;
					case OESTE:
						incx--;
						break;
					default:
						break;
					}
					while (!fin && m.coordValidas(x,y)) {
						if (jugador.estaEn(x, y)) {
							fin = true;
							siDetectado = true;
							pila.push(&actual->getHijos()[0]);
							explorado.setCasilla(x, y, 5);
						}
						else if (m.estaBloqueado(x, y)) {
							fin = true;
							pila.push(&actual->getHijos()[1]);
						}
						else {
							if (explorado.getCasilla(x, y) != 5) {
								explorado.setCasilla(x, y, 1);
							}
							x += incx;
							y += incy;
							if (!m.coordValidas(x,y)) {
								fin = true;
								pila.push(&actual->getHijos()[1]);
							}
						}
					}
				}
				else {
					pila.push(&actual->getHijos()[1]);
				}
				break;
			case Avanza:
				x = 0; y = 0;
				if (enemigo.getCasillaDelante(x, y)) {
					if (!m.estaBloqueado(x, y)) {
						enemigo.avanza();
						andado.setCasilla(enemigo.posX, enemigo.posY, 1);
					}
				}
				enemigo.turnos++;
				break;
			case GiraIz:
				enemigo.izquierda();
				enemigo.turnos++;
				break;
			case GiraDer:
				enemigo.derecha();
				enemigo.turnos++;
				break;
			case CambiarEst:
				if (siDetectado){
					encontrado = true;
				}
				while (!pila.empty()) {
					pila.pop(); //vaciamos la pila por si quedasen nodos sin evaluar pero ya hemos cambiado a ataque.
				}
				enemigo.turnos++;
				ataque = true;
				break;
			default:
				std::cerr << "El arbol de patrulla tenia nodos indebidos (" << op << ")" << std::endl;
				exit(-1);
				break;
			}
			if(turnosIni != enemigo.turnos) mueveJugador(jugador, enemigo, m);
			notifyTurno(jugador, enemigo, m, explorado, andado, andadoAtaque);
		}

		enemigo.turnosPatrulla = enemigo.turnos;

		while (enemigo.turnos < maxTurnos && jugador.heridas < 3 && enemigo.heridas < 3) {
			turnosIni = enemigo.turnos;
			if (pila.empty()) {
				pila.push(_genotipo[1].getRaiz());
			}
			actual = pila.top();
			pila.pop();
			Operacion op = actual->getElem();
			switch (op) {
			case ProgN2:
				pila.push(&actual->getHijos()[1]); //Se introducen en orden inverso que es como se quieren ejecutar.
				pila.push(&actual->getHijos()[0]);
				break;
			case ProgN3:
				pila.push(&actual->getHijos()[2]);
				pila.push(&actual->getHijos()[1]); //Se introducen en orden inverso que es como se quieren ejecutar.
				pila.push(&actual->getHijos()[0]);
				break;
			/*case ProgN4:
				pila.push(&actual->getHijos()[3]);
				pila.push(&actual->getHijos()[2]);
				pila.push(&actual->getHijos()[1]); //Se introducen en orden inverso que es como se quieren ejecutar.
				pila.push(&actual->getHijos()[0]);
				break;*/
			case SiJugador:
				x = 0; y = 0;
				enemigo.getCasillaDelante(x, y);
				if (jugador.estaEn(x, y)) {
					pila.push(&actual->getHijos()[0]);
				}
				else {
					pila.push(&actual->getHijos()[1]);
				}
				break;
			case SiBloqueado:
				x = 0; y = 0;
				if (!enemigo.getCasillaDelante(x, y)) {
					pila.push(&actual->getHijos()[0]);
				}
				else if (m.estaBloqueado(x, y)) {
					pila.push(&actual->getHijos()[0]);
				}
				else {
					pila.push(&actual->getHijos()[1]);
				}
				break;
			case SiRango:
				x = 0; y = 0;
				cont = 0;
				fin = false;
				if (enemigo.getCasillaDelante(x, y)) {
					switch (enemigo.f) {
					case NORTE:
						incy--;
						break;
					case SUR:
						incy++;
						break;
					case ESTE:
						incx++;
						break;
					case OESTE:
						incx++;
						break;
					default:
						break;
					}
					while (cont < enemigo.rango && !fin && m.coordValidas(x,y)) {
						if (jugador.estaEn(x, y)) {
							fin = true;
							pila.push(&actual->getHijos()[0]);
						}
						else if (!m.estaBloqueado(x, y)) {
							fin = true;
							pila.push(&actual->getHijos()[1]);
						}
						else {
							x += incx;
							y += incy;
							if (!m.coordValidas(x,y)) {
								fin = true;
								pila.push(&actual->getHijos()[1]);
							}
						}
						cont++;
					}
				}
				else {
					pila.push(&actual->getHijos()[1]);
				}
				break;
			case Avanza:
				x = 0; y = 0;
				if (enemigo.getCasillaDelante(x, y)) {
					if (!m.estaBloqueado(x, y)) {
						enemigo.avanza();
						andadoAtaque.setCasilla(enemigo.posX, enemigo.posY, 1);
					}
				}
				enemigo.turnos++;
				break;
			case GiraIz:
				enemigo.izquierda();
				enemigo.turnos++;
				break;
			case GiraDer:
				enemigo.derecha();
				enemigo.turnos++;
				break;
			case BloquearN:
				enemigo.bloqueando = true;
				enemigo.turnos++;
				break;
			case Atacar:
				x = 0; y = 0;
				enemigo.getCasillaDelante(x, y);
				if (jugador.estaEn(x, y)) {
					if (enemigo.turnosGolpeo == -1)
						enemigo.turnosGolpeo = enemigo.turnos - enemigo.turnosPatrulla;
					enemigo.golpes++;
					if (!jugador.bloqueando) {
						jugador.heridas++;
					}
				}
				enemigo.turnos++;
				break;
			case Retroceder:
				x = 0; y = 0;
				if (enemigo.getCasillaDetras(x, y)) {
					if (!m.estaBloqueado(x, y)) {
						enemigo.retroceder();
						andadoAtaque.setCasilla(enemigo.posX, enemigo.posY, 1);
					}
				}
				enemigo.turnos++;
				break;
			default:
				std::cerr << "El arbol de ataque tenia nodos indebidos (" << op << ")" << std::endl;
				exit(-1);
				break;
			}
			if (turnosIni != enemigo.turnos) mueveJugador(jugador, enemigo, m);
			notifyTurno(jugador, enemigo, m, explorado, andado, andadoAtaque);
		}

		if (enemigo.turnosGolpeo == -1)
			enemigo.turnosGolpeo = enemigo.turnos - enemigo.turnosPatrulla;

		double evaluacion = 0;
		int cAndadas = casillasAndadas(andado);
		int cAndadasAtaque = casillasAndadas(andadoAtaque);
		int cExpl = casillasExploradas(explorado);
		double distancia = calculaDistancia(jugador, enemigo);

		double factorPatrulla = (encontrado) ? 1 : -1;
		int turnosRestantes = (maxTurnos - enemigo.turnosPatrulla);
		int turnosQueValen = (encontrado) ? turnosRestantes : enemigo.turnosPatrulla;
		double factorAtaque = jugador.heridas + 1;
		_mediaValores[0] += (cExpl + cAndadas + turnosQueValen)*factorPatrulla;
		_mediaValores[1] += (cAndadasAtaque + enemigo.golpesEvitados + enemigo.golpes) * factorAtaque;
		_mediaValores[2] += distancia;
		_mediaValores[3] += enemigo.turnosGolpeo;
		evaluacion = 1000 + factorPatrulla*(cExpl*0.1 + cAndadas*0.25 + turnosQueValen) + (factorAtaque * factorAtaque)*(cAndadasAtaque*0.45 + enemigo.golpesEvitados + enemigo.golpes) - distancia - enemigo.turnosGolpeo;
		if (_genotipo[0].getNumNodos() - _genotipo[1].getNumNodos() > 5 || _genotipo[1].getNumNodos() < 5) {
			evaluacion -= 500;
		}

		notifyMapaTerminado(evaluacion, factorPatrulla, cExpl, cAndadas, turnosQueValen, factorAtaque, cAndadasAtaque, enemigo.golpesEvitados, enemigo.golpes, distancia, enemigo.turnosGolpeo);

		return evaluacion;
	}

	int casillasExploradas(Mapa m) {
		int ret = 0;
		for (std::size_t i = 0; i < m.getWidth(); ++i) {
			for (std::size_t j = 0; j < m.getHeight(); ++j) {
				if (m.getCasilla(i, j) > 0) {
					ret += m.getCasilla(i, j);
				}
			}
		}
		return ret;
	}

	double calculaDistancia(npc jugador, npc enemigo) {
		return sqrt(((enemigo.posX - jugador.posX)*(enemigo.posX - jugador.posX)) + ((enemigo.posY - jugador.posY)*(enemigo.posY - jugador.posY)));
	}

	int casillasAndadas(Mapa m) {
		int ret = 0;
		for (std::size_t i = 0; i < m.getWidth(); ++i) {
			for (std::size_t j = 0; j < m.getHeight(); ++j) {
				if (m.getCasilla(i, j) == 1) {
					ret += m.getCasilla(i, j);
				}
			}
		}
		return ret;
	}

	void mueveJugador(npc &jugador, npc &enemigo, Mapa m) {
		jugador.bloqueando = false;
		int x, y;
		//Operacion op = (Operacion)myRandom::getRandom(Operacion::Avanza, Operacion::Retroceder);
		int intentos = 2;
		Operacion op = Operacion::ProgN3;
		jugador.getCasillaDelante(x, y);
		if (enemigo.estaEn(x, y)) {
			int n = myRandom::getRandom(0, 3);
			if (n < 2) {
				op = Operacion::Atacar;
			}
			else if(n == 2){
				op = Operacion::BloquearN;
			}
			else{
				op = Operacion::Retroceder;	
				// Le damos la oportunidad de retirarse, sino, se queda siempre delante atacando o bloqueando
			}
		}
		if (jugador.getCasillaDelante(x,y)) {
			if (m.estaBloqueado(x, y)) {
				jugador.izquierda();
				jugador.izquierda();
			}
			int giro = myRandom::getRandom(0, 100);
			if (giro != 23) {
				op = Operacion::Avanza;
			}
			else {
				giro = myRandom::getRandom(0, 1);
				if (giro == 0) {
					op = Operacion::GiraIz;
				}
				else {
					op = Operacion::GiraDer;
				}
			}
		}
		if (!jugador.getCasillaDelante(x, y)) {
			jugador.izquierda();
			jugador.izquierda();
		}
		switch (op) {
		case Avanza:
			x = 0; y = 0;
			if (jugador.getCasillaDelante(x, y)) {
				if (!m.estaBloqueado(x, y)) {
					jugador.avanza();
				}
			}
			break;
		case GiraIz:
			jugador.izquierda();
			break;
		case GiraDer:
			jugador.derecha();
			break;
		case BloquearN:
			jugador.bloqueando = true;
			break;
		case Atacar:
			x = 0; y = 0;
			jugador.getCasillaDelante(x, y);
			if (enemigo.estaEn(x, y)) {
				if (!enemigo.bloqueando) {
					enemigo.heridas++;
				}
				else {
					enemigo.golpesEvitados++;
				}
			}
			break;
		case Retroceder:
			x = 0; y = 0;
			if (jugador.getCasillaDetras(x, y)) {
				if (!m.estaBloqueado(x, y)) {
					jugador.retroceder();
				}
			}
			break;
		default:
			break;
		}
		enemigo.bloqueando = false;
	}
	/*
	npc setRandomPosition(Mapa &m) {
		int x, y;
		do {
			x = myRandom::getRandom(0, m.getWidth() - 1);
			y = myRandom::getRandom(0, m.getHeight() - 1);
		} while (m.getCasilla(x, y) != m.VACIO);
		npc enemigo(x, y, m.getHeight(), m.getWidth());
		return enemigo;
	}*/

	void setPositionInRange(npc jugador, npc &enemigo, int minRan, int maxRan, Mapa m){
		int xJug = jugador.posX;
		int yJug = jugador.posY;
		std::vector<std::pair<int, int>> posibles;
		for (int i = xJug - maxRan; i <= xJug + maxRan; ++i){
			for (int j = yJug - maxRan; j <= yJug + maxRan; ++j){
				if (i < xJug - minRan || i > xJug + minRan){
					if (j < yJug - minRan || j > yJug + minRan){
						if (m.coordValidas(i, j) && !m.estaBloqueado(i,j)){
							posibles.emplace_back(i, j);
						}
					}
				}
			}
		}
		std::random_shuffle(posibles.begin(), posibles.end());
		enemigo.posX = posibles.front().first;
		enemigo.posY = posibles.front().second;
		return;
	}

	void notifySimulacionInciada() const{
		for (ICromosomaObserver* i : _obs){
			i->onSimulacionIniciada(this);
		}
	}

	void notifyMapaTerminado(double fitness, double factorPatrulla, int cExpl, int cAndadas,int turnosQueValen, double factorAtaque, int cAndadasAtaque, int golpesEvitados, int golpes, double distancia, int turnosGolpeo) const{
		for (ICromosomaObserver* i : _obs){
			i->onMapaTerminado(fitness, factorPatrulla, cExpl, cAndadas, turnosQueValen, factorAtaque, cAndadasAtaque, golpesEvitados, golpes, distancia, turnosGolpeo);
		}
	}

	void notifyTurno(npc jugador, npc enemigo, Mapa m, Mapa explorado, Mapa andado, Mapa andadoAtaque) const{
		for (ICromosomaObserver* i : _obs){
			i->onTurno(this, jugador, enemigo, m, explorado, andado, andadoAtaque);
		}
	}

	void notifySimulacionTerminada() const{
		for (ICromosomaObserver* i : _obs){
			i->onSimulacionTerminada(this);
		}
	}

	Arbol _genotipo[2]; //la primera posici�n es el �rbol de patrulla y la segunda el de ataque.
	double _punt;
	double _puntAcum;
	double _adaptacion;
	double _valores[6];
	double _mediaValores[6];
	double _pesos[6]; //casillas exploradas, casillas andadas, golpes realizados, heridasBloqueadas, casillas andadas en ataque, distancia final entre jugador y enemigo

	bool _descartado;

	std::vector<ICromosomaObserver*> _obs;
};

#endif