#ifndef VENTANA_HPP
#define VENTANA_HPP

#include <SFML\Graphics.hpp>
#include <thread>
#include "Plotter.hpp"
#include "ProgressBar.hpp"
#include "Logger.hpp"
#include "Button.hpp"
#include "ScrollBar.hpp"
#include "Controlador.hpp"
#include "TabPane.hpp"

class Ventana : public IAGObserver{
public:
	Ventana(Controlador& c) :
		_window(sf::VideoMode::getFullscreenModes()[6], "AG"),		// Comentar esta linea y
		//_window(sf::VideoMode(1200,600), "AG"),					// descomentar esta si no se representa bien en pantalla
		_tabPane(sf::Vector2f(0, 0), sf::Vector2f(_window.getSize().x * 0.75, 25)),
		_plotter(sf::Vector2f(0, 0), sf::Vector2f(_window.getSize().x * 0.75, _window.getSize().y)),
		_logger(sf::Vector2f(_window.getSize().x * 0.8, 75), sf::Vector2f(_window.getSize().x * 0.19, 400)),
		_botonRun(sf::Vector2f(_window.getSize().x * 0.8 , 10), sf::Vector2f(_window.getSize().x * 0.1, 50), "RUN", sf::Color(100,200,200)),
		_progress(sf::Vector2f(_window.getSize().x *0.8, _window.getSize().y -50), sf::Vector2f(_window.getSize().x * 0.1, 30))
	{
		_font.loadFromFile("arial.ttf");
		_ctrl = &c;
		_generacion = 0;
		_ctrl->addObserver(*(this));
		_tabPane.addTab("Plotter", _plotter);
	}

	void run(unsigned int maxIter){
		_progress.setMaxProgress(maxIter);
		for (size_t i = 0; i < 10; ++i){
			_ejeX.push_back(i);
		}

		_plotter.setNombreEjeX("Generaci�n");
		_plotter.setNombreEjeY("Adaptaci�n");
		_plotter.setEjeX(_ejeX);

		while (_window.isOpen())
		{
			// handle events
			sf::Event event;
			while (_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					_window.close();
				else if (event.type == sf::Event::MouseButtonPressed){
					sf::Vector2f point = sf::Vector2f(sf::Mouse::getPosition(_window));
					if (_botonRun.contains(point)){
						_plotter.removeAllData();
						_ejeX.clear();
						_valorMedia.clear();
						_valorMejor.clear();
						_valorMejorGen.clear();

						_generacion = 0;
						_logger.clearLog();
						_logger.append("Ejecutando\n");
						_progress.clearProgress();
						_window.draw(_progress);
						_window.draw(_logger);
						_window.display();
						_ctrl->run();
					}
					else if (_tabPane.contains(point)){
						_tabPane.handleClick(point);
					}
					else if (_logger.contains(point)){
						_logger.handleClick(point);
					}
				}
				else if (event.type == sf::Event::MouseButtonReleased){

				}
				else if (event.type == sf::Event::KeyPressed){
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
				
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
						_plotter.removeAllData();
						_ejeX.clear();
						_valorMedia.clear();
						_valorMejor.clear();
						_valorMejorGen.clear();

						_generacion = 0;
						_logger.clearLog();
						_logger.append("Ejecutando AG\n");
						_progress.clearProgress();
						_window.draw(_progress);
						_window.draw(_logger);
						_window.display();
						_ctrl->run();
					}
				}
			}
			_window.clear(sf::Color::White);
			_window.draw(_tabPane);
			_window.draw(_logger);
			_window.draw(_botonRun);
			_window.draw(_progress);
			_window.display();
		}
	}

	void onGeneracionTerminada(double mejor, double mejorGen, double media){
		_ejeX.push_back(_generacion);
		_valorMejor.push_back(mejor);
		_valorMejorGen.push_back(mejorGen);
		_valorMedia.push_back(media);
		_window.draw(_logger);
		_progress.updateProgress(_generacion);
		_window.draw(_progress);
		_window.display();
		_generacion++;
	}

	void onAGTerminado(Cromosoma mejor, double total, double tmSel, double tmCruce, double tmMut, double tInit, double tmEval){
		_plotter.setEjeX(_ejeX);

		_plotter.pushEjeY(_valorMedia, sf::Color::Green, "Media Poblacion");
		_plotter.pushEjeY(_valorMejorGen, sf::Color::Red, "Mejor Gen");
		_plotter.pushEjeY(_valorMejor, sf::Color::Blue, "Mejor");
		
		std::string valoresText[] = {
			"Exploradas: ",
			"Golpes Evitados: ",
			"Heridas Recibidas: ",
			"Da�o: "
		};
		_logger.append("-- Nota mejor --\n");
		double* valores = mejor.getValores();
		double* pesos = mejor.getPesos();
		for (size_t i = 0; i < 4; ++i){
			_logger.append(valoresText[i] + std::to_string(valores[i]) + "/" + std::to_string(valores[i]*pesos[i]) + "\n");
		}
	}


private:
	sf::RenderWindow _window;
	sf::Font _font;
	ProgressBar _progress;
	TabPane _tabPane;
	Plotter _plotter;
	Logger _logger;
	TextButton _botonRun;
	Controlador* _ctrl;
	unsigned int _generacion;
	std::vector<double> _ejeX;
	std::vector<double> _valorMejor;
	std::vector<double> _valorMejorGen;
	std::vector<double> _valorMedia;
};

#endif