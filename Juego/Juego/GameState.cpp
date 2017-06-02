#include "GameState.hpp"
#include "ResourceHolder.hpp"
#include "AG.hpp"
#include "Utils.hpp"

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context),
	_playerHasKey(false),
	_player(context.textures->get(Textures::Player), 3, 3, sf::Vector2f(0,0), 1)
{
	_debugInfo.setFont(context.fonts->get(Fonts::Main));
	_debugInfo.setFillColor(sf::Color::White);
	_debugInfo.setCharacterSize(12);
	_debugInfo.setPosition(0, WINDOW_HEIGHT*0.8);

	sf::Texture& texture = context.textures->get(Textures::Title);
	_buttonMenu.setTexture(texture);

	_buttonMenu.setScale(0.4, 0.4);
	_buttonMenu.setPosition(10, 10);

	sf::Texture& texture2 = context.textures->get(Textures::Key);
	_key.setTexture(texture2);
	_key.setTextureRect(LOCKED_KEY);

	_key.setScale(0.6, 0.6);
	_key.setPosition(WINDOW_WIDTH - LOCKED_KEY.width, WINDOW_HEIGHT - LOCKED_KEY.height);

	std::cout << "Arrancando" << std::endl;
	Parametros p;
	p.tamPob = 30;							// Tamano de la poblacion
	p.iteraciones = 30;						// Numero maximo de generaciones
	p.minNodos = 10;						// Numero minimo de nodos iniciales
	p.maxNodos = 40;						// Numero maximo de nodos iniciales
	p.densidad = 0.03;						// Densidad de aristas inciales
	p.elitismo = false;						// Elitismo (guarda a los mejores)
	p.bloating = true;						// Bloating	(evita grafos demasiado grandes)
	p.contractividad = false;				// Contractividad (descarta generaciones malas)
	p.probCruce = 0.6;						// Probabilidad de cruce
	p.probMutacion = 0.02;					// Probabilidad de mutacion
	p.seleccion = new SeleccionTorneo();	// Metodo de seleccion (Ver "MetodoSeleccion.hpp")
	p.cruce = new CruceMonopunto();			// Metodo de cruce (Ver "MetodoCruce.hpp")
	p.mutacion = new MutacionCombinada();		// Metodo de mutacion (Ver "MetodoMutacion.hpp")
	AG ag(p);
	Cromosoma mejor = ag.ejecuta();
	std::cout << "Terminado" << std::endl;
	_dungeon.generateRooms(mejor);
	Dungeon::Matrix room = _dungeon.getRoom(_dungeon.getSelectedRoom());
	_tiles.load("Media/Textures/TileMap.png", TILESIZE, room.getCells(), room.width, room.height);
	sf::Vector2f spawnCoords = _tiles.getCoordsFromCell(room.width / 2, room.height / 2);
	_player.setPosition(spawnCoords);

}

void GameState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());
	window.draw(_tiles);
	window.draw(_buttonMenu);
	window.draw(_key);
	window.draw(_player);

	window.draw(_debugInfo);
}

bool GameState::update(sf::Time dt)
{
	_player.update(dt);
	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		sf::Vector2u cell;
		if (event.key.code == sf::Keyboard::D){
			sf::Vector2f actualSpeed = _player.getSpeed();
			actualSpeed.x = std::max(NORMALSPEED, std::abs(actualSpeed.x));
			_player.setSpeed(actualSpeed);
			cell = _tiles.getCellFromCoords(_player.getPosition());
			if ()
		}
		else if (event.key.code == sf::Keyboard::A){
			sf::Vector2f actualSpeed = _player.getSpeed();
			actualSpeed.x = std::min(-NORMALSPEED, -std::abs(actualSpeed.x));
			_player.setSpeed(actualSpeed);
		}
		else if (event.key.code == sf::Keyboard::S){
			sf::Vector2f actualSpeed = _player.getSpeed();
			actualSpeed.y = std::max(NORMALSPEED, std::abs(actualSpeed.y));
			_player.setSpeed(actualSpeed);
		}
		else if (event.key.code == sf::Keyboard::W){
			sf::Vector2f actualSpeed = _player.getSpeed();
			actualSpeed.y = std::min(-NORMALSPEED, -std::abs(actualSpeed.y));
			_player.setSpeed(actualSpeed);
		}
		else if (event.key.code == sf::Keyboard::Q){
			_key.setTextureRect(UNLOCKED_KEY);
		}
	}
	else if (event.type == sf::Event::KeyReleased)
	{
		if ((event.key.code == sf::Keyboard::D) || (event.key.code == sf::Keyboard::A)){
			sf::Vector2f actualSpeed = _player.getSpeed();
			actualSpeed.x = 0;
			_player.setSpeed(actualSpeed);
		}
		else if ((event.key.code == sf::Keyboard::S) || (event.key.code == sf::Keyboard::W)){
			sf::Vector2f actualSpeed = _player.getSpeed();
			actualSpeed.y = 0;
			_player.setSpeed(actualSpeed);
		}
	
		else if (event.key.code == sf::Keyboard::Q){
			_key.setTextureRect(UNLOCKED_KEY);
		}
	}
	return false;
}