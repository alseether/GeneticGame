#include "GameOverState.hpp"
#include "MenuState.hpp"
#include "LoadingState.hpp"
#include "Button.hpp"
#include "ResourceHolder.hpp"

GameOverState::GameOverState(StateStack& stack, Context context)
	: State(stack, context)
	, _selectedButton(0)
{
	_text.setFont(context.fonts->get(Fonts::Alagard));
	_text.setCharacterSize(60);
	_text.setString("GAME OVER");
	centerOrigin(_text);
	_text.setPosition(WINDOW_WIDTH*0.5, WINDOW_HEIGHT*0.3);

	_statsText.setFont(context.fonts->get(Fonts::Alagard));
	_statsText.setCharacterSize(30);

	sf::Texture& texture = context.textures->get(Textures::GameOverScreen);
	_bgSprite.setTexture(texture);

	_buttonRestart = std::make_shared<Button>(*context.fonts, *context.textures);
	_buttonRestart->setText("Try Again");
	_buttonRestart->setPosition(WINDOW_WIDTH*0.33, WINDOW_HEIGHT*0.5);
	_buttonRestart->setCallback([this]()
	{
		requestStackPop();
		auto loadState = std::make_shared<LoadingState>(*_stack, _context);
		loadState->launch(LOADINGMSG[1]);
		requestStackPush(loadState);
	});
	_buttonRestart->select();

	_buttonExit = std::make_shared<Button>(*context.fonts, *context.textures);
	_buttonExit->setPosition(WINDOW_WIDTH*0.33, WINDOW_HEIGHT*0.6);
	_buttonExit->setText("Exit");
	_buttonExit->setCallback([this]()
	{
		requestStateClear();
		auto menuState = std::make_shared<MenuState>(*_stack, _context);
		requestStackPush(menuState);
	});

	sf::SoundBuffer& buffer = context.sounds->get(Sounds::Lose);
	_loseSound.setBuffer(buffer);
	_loseSound.play();
}

void GameOverState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(_bgSprite);
	window.draw(_text);
	window.draw(_statsText);
	window.draw(*_buttonRestart);
	window.draw(*_buttonExit);
}

bool GameOverState::update(sf::Time)
{
	return false;
}

bool GameOverState::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape){
			requestStackPop();
		}
	}
	if (event.type == sf::Event::KeyReleased)
	{
		if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
		{
			_selectedButton = (_selectedButton != 1) ? 1 : 0;
			if (_selectedButton == 0){
				_buttonRestart->select();
				_buttonExit->deselect();
			}
			else if (_selectedButton == 1){
				_buttonRestart->deselect();
				_buttonExit->select();
			}
		}
		else if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space)
		{
			if (_selectedButton == 0){
				_buttonRestart->activate();
			}
			else if (_selectedButton == 1){
				_buttonExit->activate();
			}
		}
	}
	return false;
}

void GameOverState::setPlayerStats(Stats s){
	_playerStats = s;
	std::string str = "Points: " + std::to_string(_playerStats.rawPoints) + "\n";
	str += "Time: " + std::to_string((int)_playerStats.timeAlive.asSeconds()) + "s\n";
	_statsText.setString(str);
	centerOrigin(_statsText);
	_statsText.setPosition(WINDOW_WIDTH*0.5, WINDOW_HEIGHT*0.45);
}

Stats GameOverState::getPlayerStats() const{
	return _playerStats;
}