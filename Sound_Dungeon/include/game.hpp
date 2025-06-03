#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "dungeon.hpp"
#include "player.hpp"
#include "menu.hpp"
class Menu;

//Used to manage overall game state
enum class GameState {
    MainMenu,
    Playing,
    Paused,
    Victory,
    Defeat
};

class Game {
public:
    Game(sf::RenderWindow& window);
    void run(bool debug);
    void beginLevel(bool newLevel);

    sf::Vector2f dungeonScale;
    GameState currentState = GameState::MainMenu;  // Start with Main Menu
    bool devMode;

private:
    void processInput();
    void update(float deltaTime);
    void render();

    std::unique_ptr<Dungeon> dungeon;
    std::unique_ptr<Player> player;
    std::unique_ptr<Menu> menu;
    std::unique_ptr<sf::Sprite> dungeonSprite;
    std::unique_ptr<sf::Sprite> playerSprite;
    std::unique_ptr<sf::Font> menuFont;

    sf::Texture dungeonTexture;
    sf::Image dungeonImage;
    sf::Texture playerTexture;

    sf::Clock clock;
    sf::Clock rotationCooldown;

    sf::RenderWindow& window;

    bool isRunning = true;
    bool bFirstGo = false;
};

#endif