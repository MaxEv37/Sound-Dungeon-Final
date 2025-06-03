#include "..\include\menu.hpp"
#include <iostream>

Menu::Menu(sf::Font& font, sf::RenderWindow& window, Game* gamePtr)
    : window(window), gamePtr(gamePtr) {

    menuFont = font;
    menuText = std::make_unique<sf::Text>(font);
    std::vector<std::string> options = { "Play", "Help", "Exit to Desktop"};

    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text text(font);
        text.setString(options[i]);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::Blue);
        text.setPosition(sf::Vector2f(200, 200 + static_cast<float>(i) * 60));

        menuOptions.push_back(text);
    }
    
    menuOptions[selectedIndex].setFillColor(sf::Color::Red);

    if (!clickBuffer.loadFromFile("../Sound_Dungeon/audio/menu_click_1.wav"))
    {
        std::cerr << "Error: Failed to load menu click sound!" << std::endl;
        return;
    }

    if (!musicBuffer.loadFromFile(menuMusic))
    {
        std::cerr << "Error: Failed to load menu music sound!" << std::endl;
        return;
    }

    if (!descBuffer.loadFromFile("../Sound_Dungeon/audio/menu_Controls.wav"))
    {
        std::cerr << "Error: Failed to load menu description!" << std::endl;
        return;
    }

    clickSound = std::make_unique<sf::Sound>(clickBuffer);
    musicSound = std::make_unique<sf::Sound>(musicBuffer);
    descSound = std::make_unique<sf::Sound>(descBuffer);
    descSound->setVolume(100);
    fileNames = { "menu_play.wav", "menu_help.wav", "menu_ExitToDesktop.wav" };
}

void Menu::handleInput(sf::Event::KeyReleased keyReleased)
{
    if ((keyReleased.scancode == sf::Keyboard::Scan::J) || (keyReleased.scancode == sf::Keyboard::Scan::F) || (keyReleased.scancode == sf::Keyboard::Scan::Space))
    {
        clickNoise(keyReleased);
        switch (currentMenu)
        {
        case mainMenu:
            startMenu(keyReleased);
            break;
        case helpMenu:
            help(keyReleased);
            break;
        case pauseMenu:
            pause(keyReleased);
            break;
        case endMenu:
            end(keyReleased, 0);
            break;
        default:
            break;
        };
    }

    // Used to toggle developer mode
    if (keyReleased.scancode == sf::Keyboard::Scan::Enter || keyReleased.scancode == sf::Keyboard::Scan::P)
    {
        if (currentMenu == menuType::helpMenu)
        {
            help(keyReleased);
        }
    }
}

void Menu::render(sf::RenderWindow& window)
{
    for (auto option : menuOptions) {
        window.draw(option);
    }
}

void Menu::help(sf::Event::KeyReleased keyReleased)
{
    if (currentMenu != menuType::helpMenu)
    {
        currentMenu = menuType::helpMenu;
        menuOptions.clear();
        selectedIndex = 0;
        std::vector<std::string> options = { "Tutorial", "Back" };
        fileNames = { "menu_tutorial.wav", "menu_back.wav" };

        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text text(menuFont);
            text.setString(options[i]);
            text.setCharacterSize(40);
            text.setFillColor(sf::Color::Blue);
            text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, 200 + static_cast<float>(i) * 60));

            menuOptions.push_back(text);
        }

        menuOptions[selectedIndex].setFillColor(sf::Color::Red);
        playDescription(fileNames[0]);
        return;
    }
    else
    {
        if (keyReleased.scancode == sf::Keyboard::Scan::J) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex + 1) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::F) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex - 1 + menuOptions.size()) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::Enter)
        {
            checkDev = !checkDev;
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::P)
        {
            gamePtr->devMode = checkDev;
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::Space) {
            if (gamePtr != nullptr) {
                if (selectedIndex == 0) playDescription("menu_GameControls.wav");
                if (selectedIndex == 1) {
                    switch (previousMenu)
                    {
                    case mainMenu:
                        startMenu(keyReleased);
                        break;
                    case helpMenu:
                        help(keyReleased);
                        break;
                    case pauseMenu:
                        pause(keyReleased);
                        break;
                    case endMenu:
                        end(keyReleased, 0);
                        break;
                    default:
                        break;
                    };
                }

            }
        }
    }
}

void Menu::startMenu(sf::Event::KeyReleased keyReleased)
{
    if (currentMenu != menuType::mainMenu)
    {
        menuOptions.clear();
        selectedIndex = 0;
        std::vector<std::string> options = { "Play", "Help", "Exit to Desktop" };
        fileNames = { "menu_play.wav", "menu_help.wav", "menu_ExitToDesktop.wav"};

        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text text(menuFont);
            text.setString(options[i]);
            text.setCharacterSize(40);
            text.setFillColor(sf::Color::Blue);
            text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, 200 + static_cast<float>(i) * 60));

            menuOptions.push_back(text);
        }

        menuOptions[selectedIndex].setFillColor(sf::Color::Red);
        currentMenu = menuType::mainMenu;
        playDescription(fileNames[0]);
        return;
    }
    else
    {
        if (keyReleased.scancode == sf::Keyboard::Scan::J) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex + 1) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::F) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex - 1 + menuOptions.size()) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::Space) {
            if (gamePtr != nullptr) {  // Prevent crashes due to null pointer
                if (selectedIndex == 0)
                {
                    toggleMusic(false);
                    gamePtr->currentState = GameState::Playing;
                    gamePtr->beginLevel(false);  // Begin Game
                }
                if (selectedIndex == 1) {
                    previousMenu = currentMenu;
                    help(keyReleased); //go to help menu
                }
                if (selectedIndex == 2) window.close();
            }
        }
    }
}

void Menu::end(sf::Event::KeyReleased keyReleased, int finalScore)
{
    if (currentMenu != menuType::endMenu)
    {
        toggleMusic(true);
        gamePtr->currentState = GameState::Victory;
        currentMenu = menuType::endMenu;
        menuOptions.clear();
        selectedIndex = 0;
        std::vector<std::string> options = { "Exit to Main Menu", "Exit to Desktop" };

        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text text(menuFont);
            text.setString(options[i]);
            text.setCharacterSize(40);
            text.setFillColor(sf::Color::Blue);
            text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, 200 + static_cast<float>(i) * 60));

            menuOptions.push_back(text);
        }

        menuOptions[selectedIndex].setFillColor(sf::Color::Red);


        fileNames = { "score_0.wav", "score_1.wav", "score_2.wav" ,"score_3.wav", "score_4.wav", "score_5.wav", "score_6.wav", "score_7.wav", "score_8.wav", "score_9.wav"};

        std::string scoreStr = std::to_string(finalScore);

        char hundreds = scoreStr[0];
        char tens = scoreStr[1];
        char ones = scoreStr[2];

        // Play victory line
        playDescription("menu_victory.wav");
        while (descSound->getStatus() == sf::SoundSource::Status::Playing)
        {
            sf::sleep(sf::milliseconds(100));
        }

        while (scoreStr.size() < 3) {
            scoreStr = "0" + scoreStr;
        }

        // Read final score aloud
        for (char digit : scoreStr) {
            int index = digit - '0'; 
            playDescription(fileNames[index]);

            while (descSound->getStatus() == sf::SoundSource::Status::Playing) {
                sf::sleep(sf::milliseconds(100));
            }
        }

        fileNames = { "menu_ExitToMainMenu.wav", "menu_ExitToDesktop.wav" };
        return;
    }
    else
    {
        if (keyReleased.scancode == sf::Keyboard::Scan::J) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex + 1) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::F) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex - 1 + menuOptions.size()) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::Space) {
            if (gamePtr != nullptr) {
                if (selectedIndex == 0) startMenu(keyReleased); gamePtr->currentState = GameState::MainMenu;
                if (selectedIndex == 1) window.close();
            }
        }
    }
}

void Menu::pause(sf::Event::KeyReleased keyReleased)
{
    if (currentMenu != menuType::pauseMenu)
    {
        previousMenu = currentMenu;
        menuOptions.clear();
        selectedIndex = 0;
        std::vector<std::string> options = { "Resume", "Help", "Restart", "Exit to Main Menu", "Exit to Desktop"};
        fileNames = {"menu_resume.wav", "menu_help.wav", "menu_restart.wav", "menu_ExitToMainMenu.wav", "menu_ExitToDesktop.wav" };

        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text text(menuFont);
            text.setString(options[i]);
            text.setCharacterSize(40);
            text.setFillColor(sf::Color::Blue);
            text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, 200 + static_cast<float>(i) * 60));  // Offset vertically
            
            menuOptions.push_back(text);
        }

        menuOptions[selectedIndex].setFillColor(sf::Color::Red);
        currentMenu = menuType::pauseMenu;
        toggleMusic(true);
        playDescription(fileNames[0]);
        return;
    }
    else
    {
        if (keyReleased.scancode == sf::Keyboard::Scan::J) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex + 1) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::F) {
            menuOptions[selectedIndex].setFillColor(sf::Color::Blue);
            selectedIndex = (selectedIndex - 1 + menuOptions.size()) % menuOptions.size();
            menuOptions[selectedIndex].setFillColor(sf::Color::Red);
            playDescription(fileNames[selectedIndex]);
        }

        if (keyReleased.scancode == sf::Keyboard::Scan::Space) {
            if (gamePtr != nullptr) {
                if (selectedIndex == 0) {
                    currentMenu = menuType::mainMenu;
                    gamePtr->currentState = GameState::Playing;
                    toggleMusic(false);
                }
                if (selectedIndex == 1) {
                    previousMenu = currentMenu;
                    help(keyReleased);                                         
                }                          
                if (selectedIndex == 2) {
                    gamePtr->beginLevel(true);
                    currentMenu = menuType::mainMenu;
                }
                if (selectedIndex == 3) startMenu(keyReleased);                      // Quit to main menu
                if (selectedIndex == 4) window.close();                              // Exit program
            }
        }
    }
}

void Menu::setMenu(menuType newMenu)
{
    currentMenu = newMenu;
}

void Menu::clickNoise(sf::Event::KeyReleased keyReleased)
{
    if (clickSound->getStatus() != sf::SoundSource::Status::Playing)
    {
        sf::Listener::setPosition(sf::Vector3f(0.0f, 0.0f, 0.0f));
        std::string selectedSound = clickSounds[rand() % clickSounds.size()];

        clickBuffer.loadFromFile(selectedSound);
        clickSound->setBuffer(clickBuffer);
        clickSound->setVolume(50);
        clickSound->setPitch(5.0f + static_cast<float>(rand() % 20) / 100.0f);
        if (keyReleased.scancode == sf::Keyboard::Scan::Space) clickSound->setPitch(3.0f + static_cast<float>(rand() % 20) / 100.0f);
        clickSound->setPosition(sf::Vector3f(0, 0, 0));
        clickSound->play();
    }
}

void Menu::toggleMusic(bool play)
{
    if (play && musicSound->getStatus() != sf::SoundSource::Status::Playing)
    {
        sf::Listener::setPosition(sf::Vector3f(0.0f, 0.0f, 0.0f));

        musicBuffer.loadFromFile(menuMusic);
        musicSound->setBuffer(musicBuffer);
        musicSound->setVolume(50);
        musicSound->setPosition(sf::Vector3f(0, 0, 0));
        musicSound->setLooping(true);
        musicSound->play();
        return;
    }
    else { 
        if (musicSound->getStatus() == sf::SoundSource::Status::Playing) {
            musicSound->pause();
        }
    }

}

void Menu::playDescription(std::string fileName)
{
        sf::Listener::setPosition(sf::Vector3f(0.0f, 0.0f, 0.0f));
        std::string selectedSound = "../Sound_Dungeon/audio/" + fileName;
        descBuffer.loadFromFile(selectedSound);
        descSound->setBuffer(descBuffer);
        descSound->setVolume(100);
        descSound->setPosition(sf::Vector3f(0, 0, 0));
        descSound->play();
}

void Menu::playGameStart(sf::Vector3f listnerPos) {
    fileNames = {"menu_welcome.wav", "EndRoom_1.wav" ,"menu_reviewControls.wav"};
    descSound->setPosition(listnerPos);

    for (size_t i = 0; i < fileNames.size(); i++)
    {
        std::cout << "Now playing: " << fileNames[i] << std::endl;
        std::string selectedSound = "../Sound_Dungeon/audio/" + fileNames[i];
        descBuffer.loadFromFile(selectedSound);
        descSound->setBuffer(descBuffer);
        descSound->setVolume(100);
        descSound->play();

        while (descSound->getStatus() == sf::SoundSource::Status::Playing)
        {
            sf::sleep(sf::milliseconds(100));
        }
    }
}