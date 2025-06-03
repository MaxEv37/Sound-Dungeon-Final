#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "game.hpp"
class Game;

enum menuType
{
    mainMenu,
    helpMenu,
    pauseMenu,
    endMenu
};


class Menu {
public:
    Menu(sf::Font& font, sf::RenderWindow& window, Game* gamePtr);  // Constructor
    void handleInput(const sf::Event::KeyReleased keyReleased);  // Handle user input
    void render(sf::RenderWindow& window);  // Draw menu
    void help(sf::Event::KeyReleased keyReleased);
    void startMenu(sf::Event::KeyReleased keyReleased);
    void end(sf::Event::KeyReleased keyReleased, int finalScore);
    void pause(sf::Event::KeyReleased keyReleased);
    void setMenu(menuType newMenu);

    void clickNoise(sf::Event::KeyReleased keyReleased); // Plays a click!
    void toggleMusic(bool play);
    void playDescription(std::string fileName);
    void playGameStart(sf::Vector3f listnerPos);

private:
    Game* gamePtr;
    sf::RenderWindow& window;
    std::unique_ptr<sf::Text> menuText;
    std::unique_ptr<sf::Sound> clickSound;
    std::unique_ptr<sf::Sound> musicSound;
    std::unique_ptr<sf::Sound> descSound;

    menuType currentMenu = menuType::mainMenu;
    menuType previousMenu = menuType::mainMenu;

    size_t selectedIndex = 0;

    sf::Font menuFont;
    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer musicBuffer;
    sf::SoundBuffer descBuffer;

    std::vector<sf::Text> menuOptions;

    std::vector<std::string> clickSounds = {
        "../Sound_Dungeon/audio/menu_click_1.wav",
        "../Sound_Dungeon/audio/menu_click_2.wav",
        "../Sound_Dungeon/audio/menu_click_3.wav"
    };

    std::vector<std::string> menuDescriptions = {
    "../Sound_Dungeon/audio/menu_play.wav",
    "../Sound_Dungeon/audio/menu_help.wav",
    "../Sound_Dungeon/audio/menu_tutorial.wav",
    "../Sound_Dungeon/audio/menu_resume.wav",
    "../Sound_Dungeon/audio/menu_restart.wav",
    "../Sound_Dungeon/audio/menu_ExitToMainMenu.wav",
    "../Sound_Dungeon/audio/menu_ExitToDesktop.wav",
    "../Sound_Dungeon/audio/menu_Controls.wav",
    "../Sound_Dungeon/audio/menu_GameControls.wav",
    };

    std::vector<std::string> fileNames;

    std::string menuMusic = "../Sound_Dungeon/audio/menu_music.wav";

    bool checkDev = false;
};