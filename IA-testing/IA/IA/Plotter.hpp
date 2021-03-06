#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include <vector>
#include <SFML\Graphics.hpp>

class Plotter : public sf::Rect<float>, public sf::Drawable, public sf::Transformable{
public:
	Plotter(sf::Vector2f pos, sf::Vector2f size) :
	_pos(pos),
	_size(size)
	{
		_font.loadFromFile("arial.ttf");
		_nombreX = "Eje X";
		_nombreY = "Eje Y";
	}

	void setNombreEjeX(std::string nombre){
		_nombreX = nombre;
	}

	void setNombreEjeY(std::string nombre){
		_nombreY = nombre;
	}

	void setEjeX(std::vector<double> dataX){
		_dataX = dataX;
	}

	void pushEjeY(std::vector<double> dataY, sf::Color color, std::string name){
		_datasY.push_back(dataY);
		_colorsY.push_back(color);
		_namesY.push_back(name);
	}

	void removeAllData(){
		_dataX.clear();
		removeDatasY();
	}

	void removeDatasY(){
		_datasY.clear();
		_colorsY.clear();
		_namesY.clear();
	}

	void removeDataY(unsigned int i){
		if (i < _datasY.size()){
			_datasY.erase(_datasY.begin() + i);
			_colorsY.erase(_colorsY.begin() + i);
			_namesY.erase(_namesY.begin() + i);
		}
	}

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		try{
			if (_dataX.size() == 0){
				return;
			}
			int margin = 50;
			int exceso = 15;

			int divisiones = 10;
			double factorXY = 1;
			double tamDivX = (_size.x - 2 * margin) / divisiones;
			double tamDivY = (_size.y - 2 * margin) / divisiones;
			double tamUnidadX = (_size.x - 2 * margin) / _dataX.back();
			double tamUnidadY = 1;
			states.transform *= getTransform();
			sf::Vector2f punto00 (_pos.x + margin, _pos.y + _size.y - margin);
			sf::VertexArray ejes;
			ejes.setPrimitiveType(sf::Lines);
			sf::Vertex topY(sf::Vector2f(_pos.x + margin, _pos.y + margin), sf::Color::Black);
			sf::Vertex bottomY(sf::Vector2f(_pos.x + margin, _pos.y + _size.y - margin), sf::Color::Black);
			sf::Vertex leftX(sf::Vector2f(_pos.x + margin, _pos.y + _size.y - margin), sf::Color::Black);
			sf::Vertex rightX(sf::Vector2f(_pos.x + _size.x - margin, _pos.y + _size.y - margin), sf::Color::Black);
		
			std::vector<sf::Text> datosEjeX;
			std::vector<sf::Text> datosEjeY;

			std::vector<sf::VertexArray> graficas;
			std::vector<sf::Text> nombres;
		
			double mayorX = _dataX.back();
			double mayorY = -1;
			double actual;
			for (size_t i = 0; i < _datasY.size(); ++i){
				for (size_t j = 0; j < _datasY[i].size(); ++j){
					actual = _datasY[i][j];
					if (actual > mayorY){
						mayorY = actual;
					}
				}
			}
			if (mayorY < 1) mayorY = 1;		// Ajuste s�lo necesario para que el eje Y quede al menos entre 0 y 1
			factorXY = mayorX / mayorY;
			tamUnidadY = (_size.y - 2 * margin) / mayorY;
			if(factorXY > 1 && tamUnidadY < 1) tamUnidadY *= factorXY;
			double top = _pos.y + margin;
			double bot = _pos.y + _size.y - margin;
			double left = _pos.x + margin;
			double right = _pos.x + _size.x - margin;
			for (size_t i = 0; i <= divisiones; ++i){
				sf::Vertex a(sf::Vector2f(left + (i*tamDivX), top), sf::Color(200, 200, 200));
				sf::Vertex b(sf::Vector2f(left + (i*tamDivX), bot + exceso), sf::Color(200, 200, 200));
				ejes.append(a);
				ejes.append(b);
				if (bot - (i*tamDivY) >= top-1){	// el -1 es para que entre en la ultima vuelta (por temas de redondeo no entraba)
					sf::Vertex c(sf::Vector2f(left - exceso, bot - (i*tamDivY)), sf::Color(200, 200, 200));
					sf::Vertex d(sf::Vector2f(right, bot - (i*tamDivY)), sf::Color(200, 200, 200));
					ejes.append(c);
					ejes.append(d);

					sf::Text t2(std::to_string(((float)i / (float)divisiones) * mayorY), _font, 9);
					/* No hay manera de dejarlo sin decimales, asi que trunco la cadena */
					std::string s = t2.getString();
					size_t posPunto = s.find_first_of(".");
					s = s.substr(0, posPunto+2);
					t2.setString(s);
					/***********/
					t2.setFillColor(sf::Color::Black);
					t2.setPosition(c.position);
					datosEjeY.push_back(t2);
				}

				sf::Text t(std::to_string(std::ceil(((double)i / (double)divisiones) * mayorX)), _font, 9);
				/* No hay manera de dejarlo sin decimales, asi que trunco la cadena */
				std::string s = t.getString();
				size_t posPunto = s.find_first_of(".");
				s = s.substr(0, posPunto);
				t.setString(s);
				/***********/
				t.setFillColor(sf::Color::Black);
				t.setPosition(b.position.x, b.position.y - exceso);
				datosEjeX.push_back(t);
			}
			graficas.clear();
			for (size_t i = 0; i < _datasY.size(); ++i){
				graficas.push_back(sf::VertexArray());
				graficas[i].setPrimitiveType(sf::LinesStrip);
				for (size_t j = 0; j < _datasY[i].size(); ++j){
					sf::Vector2f pos(_dataX[j] * tamUnidadX, -_datasY[i][j] * tamUnidadY);
					sf::Vertex v(punto00 + pos, _colorsY[i]);
					graficas[i].append(v);
				}
			}
			int offsetTexto = 25;
			int k = 0;
			nombres.clear();
			for (std::string n : _namesY){
				sf::Text t(n, _font, 15);
				t.setFillColor(_colorsY[k]);
				t.setPosition(_pos.x + _size.x - margin + offsetTexto, _pos.y + margin + k*offsetTexto);
				++k;
				nombres.push_back(t);
			}
			ejes.append(topY);
			ejes.append(bottomY);
			ejes.append(leftX);
			ejes.append(rightX);
			sf::Text textEjeX(_nombreX, _font, 15);
			textEjeX.setFillColor(sf::Color::Black);
			textEjeX.setPosition(_pos.x + (_size.x / 2), _pos.y + (_size.y - 25));
			sf::Text textEjeY(_nombreY, _font, 15);
			textEjeY.setFillColor(sf::Color::Black);
			textEjeY.setPosition(_pos.x + 3, _pos.y + (_size.y / 2));
			textEjeY.rotate(-90);
			target.draw(ejes, states);
			target.draw(textEjeX, states);
			target.draw(textEjeY, states);
			for (sf::Text t : datosEjeX)
				target.draw(t, states);
			for (sf::Text t : datosEjeY)
				target.draw(t, states);
			for (sf::VertexArray v : graficas)
				target.draw(v, states);
			for (sf::Text t : nombres)
				target.draw(t, states);
		}
		catch (std::out_of_range e){

		}
	}

	sf::Vector2f _pos;
	sf::Vector2f _size;
	std::string _nombreX;
	std::string _nombreY;
	sf::Font _font;
	std::vector<double> _dataX;
	std::vector<std::vector<double>> _datasY;
	std::vector<sf::Color> _colorsY;
	std::vector<std::string> _namesY;
};

#endif