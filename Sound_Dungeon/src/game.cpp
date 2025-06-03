#include "..\include\game.hpp"
#include "..\..\WFC\include\imageGen.hpp"
#include <iostream>

Game::Game(sf::RenderWindow& windowRef) : window(windowRef) {
    window.setFramerateLimit(60);
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    const int GRID_CELL_WIDTH = windowWidth / 64;
    const int GRID_CELL_HEIGHT = windowHeight / 64;
    const float SCALE_FACTOR = 5.0f; 
    menuFont = std::make_unique<sf::Font>();
    if (!menuFont->openFromFile("../Sound_Dungeon/fonts/Menu_font.ttf")) {
        std::cerr << "Error: Could not load menu font!" << std::endl;
    }
    menu = std::make_unique<Menu>(*menuFont, window, this);
    if (!playerTexture.loadFromFile("../Sound_Dungeon/build/results/playerSprite.png")) {
        std::cout << "Error: Failed to load player texture!" << std::endl;
        return;
    }
    playerSprite = std::make_unique<sf::Sprite>(playerTexture);
    const std::string imagePath = "../WFC/assets/Dungeon_Gen_Style_9.png";
    genImage(imagePath, "Dungeon_Gen_");
    if (!dungeonTexture.loadFromFile("../Sound_Dungeon/build/results/Dungeon_Gen_0.png")) {
        std::cout << "Error: Failed to load dungeon texture!" << std::endl;
        return;
    }
    dungeonImage = dungeonTexture.copyToImage();
    dungeon = std::make_unique<Dungeon>(dungeonImage);
    dungeonScale = sf::Vector2f(
        window.getSize().x / dungeonTexture.getSize().x,
        window.getSize().y / dungeonTexture.getSize().y
    );
    if (!dungeonTexture.loadFromFile("../Sound_Dungeon/build/results/Dungeon_Gen_Modified_Dev.png")) {
        std::cout << "Error: Failed to load dungeon texture!" << std::endl;
        return;
    }
    dungeonSprite = std::make_unique<sf::Sprite>(dungeonTexture);
    dungeonImage = dungeonTexture.copyToImage();
    dungeonSprite->setScale(sf::Vector2f(dungeonScale));
    playerSprite->setScale(sf::Vector2f(dungeonScale.x / playerTexture.getSize().x, dungeonScale.y / playerTexture.getSize().y) * 0.75f);
    player = std::make_unique<Player>(dungeon->getStartPosition(dungeonScale), playerSprite, dungeon.get());
    sf::Vector2u playerSize = playerTexture.getSize();
    sf::Vector2f playerAdjustment = sf::Vector2f(playerSize.x / 2.0f, playerSize.y / 2.0f);
    player->setPosition(sf::Vector3f(player->getPosition().x + playerAdjustment.x, player->getPosition().y + playerAdjustment.y, player->getPosition().z));
    playerSprite->setPosition(sf::Vector2f(player->getPosition().x, player->getPosition().y));
    playerSprite->setOrigin(playerAdjustment);
    sf::Listener::setPosition(player->getListenerPosition());
    sf::Listener::setDirection(player->getListenerDirection());
    dungeon->popRoomAudio(dungeonScale);
    menu->toggleMusic(true);
    menu->playDescription("menu_controls.wav");
}

void Game::run(bool debug) {
    isRunning = true;
    bool devMode = debug;
    while (isRunning) {
        processInput();
        update(clock.restart().asSeconds());
        render();
    }
}

void Game::beginLevel(bool newLevel)
{
    if (newLevel)
    {
        const std::string imagePath = "../WFC/assets/Dungeon_Gen_Style_9.png";
        genImage(imagePath, "Dungeon_Gen_");
        if (!dungeonTexture.loadFromFile("../Sound_Dungeon/build/results/Dungeon_Gen_0.png")) {
            std::cout << "Error: Failed to load dungeon texture!" << std::endl;
            return;
        }
        dungeonImage = dungeonTexture.copyToImage();
        dungeon->regenerateDungeon(dungeonImage);
        dungeonSprite = std::make_unique<sf::Sprite>(dungeonTexture);
        if (!dungeonTexture.loadFromFile("../Sound_Dungeon/build/results/Dungeon_Gen_Modified_Dev.png")) {
            std::cout << "Error: Failed to load dungeon texture!" << std::endl;
            return;
        }
        dungeonImage = dungeonTexture.copyToImage();
        dungeonSprite->setScale(sf::Vector2f(dungeonScale));
        dungeon->popRoomAudio(dungeonScale);
        player->setPosition(dungeon->getStartPosition(dungeonScale));
    }

    if (!bFirstGo)
    {
        sf::Listener::setPosition(sf::Vector3f(player->getPosition().x, 0.0f, player->getPosition().y));
        menu->playGameStart(player->getListenerPosition());
        bFirstGo = true;
    }
    dungeon->startUpAudio();
}

void Game::processInput() {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                isRunning = false;
            }
            if (currentState != GameState::Playing)
            {
                if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
                {
                    menu->handleInput(*keyReleased);
                }
            }
        }
        if (!window.isOpen())
        {
            exit(0);
        }
        if (currentState == GameState::Playing)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                menu->pause(sf::Event::KeyReleased(sf::Keyboard::Key::A));
                currentState = GameState::Paused;
            }
            player->handleMovement(5.0f, rotationCooldown, dungeonImage, dungeonScale);
            if (player->inEndRoom())
            {
                menu->end(sf::Event::KeyReleased(sf::Keyboard::Key::A), player->getScore());
            }
        }
}

void Game::update(float deltaTime)
{
    // Needed for later expansions like monster AI
}

void Game::render() {
    window.clear();

    // Game is running a menu
    if (currentState == GameState::MainMenu || currentState == GameState::Paused || currentState == GameState::Victory || currentState == GameState::Defeat) {
        menu->render(window);
    }

    else if (devMode){ 
        // Game is running normally
        window.draw(*dungeonSprite);
        window.draw(player->getSprite());
    }

    window.display();
}