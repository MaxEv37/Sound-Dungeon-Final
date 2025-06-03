#include "../include/player.hpp"
#include <iostream>

Player::Player(sf::Vector3f startPosition, std::unique_ptr<sf::Sprite>& spritePtr, Dungeon* dungeon)
    : position(startPosition), listenerPosition(startPosition), currentDirection(Direction::North), sprite(spritePtr), dungeonPtr(dungeon)  // Assign raw pointer
{
    if (!footstepBuffer.loadFromFile("../Sound_Dungeon/audio/footstep_1.wav")) {
        std::cerr << "Error: Failed to load footstep sound!" << std::endl;
        return;
    }
    if (!knockBuffer.loadFromFile("../Sound_Dungeon/audio/knock_stone_1.wav")) {
        std::cerr << "Error: Failed to load knock sound!" << std::endl;
        return;
    }
    if (!doorBuffer.loadFromFile("../Sound_Dungeon/audio/door_open_1.wav"))
    {
        std::cerr << "Error: Failed to load door sound!" << std::endl;
        return;
    }
    footstepSound = std::make_unique<sf::Sound>(footstepBuffer);
    knockSound = std::make_unique<sf::Sound>(knockBuffer);
    knockSound2 = std::make_unique<sf::Sound>(knockBuffer);
    doorSound = std::make_unique<sf::Sound>(doorBuffer);
    voiceSound = std::make_unique<sf::Sound>(voiceBuffer);

}
const sf::Vector3f& Player::getPosition() const { return position; }
const sf::Sprite& Player::getSprite() const { return *sprite; }
const sf::Vector3f& Player::getListenerPosition() const { return listenerPosition; }
sf::Vector3f Player::getListenerDirection() const { return sf::Vector3f(); }
void Player::setPosition(const sf::Vector3f& newPosition) {
    position = newPosition;
    listenerPosition.x = newPosition.x;
    listenerPosition.y = newPosition.y;
    sprite->setPosition(sf::Vector2f(position.x, position.y));
}

void Player::rotate(bool clockwise) {
    if (clockwise) {
        switch (currentDirection) {
        case Direction::North: currentDirection = Direction::East; break;
        case Direction::East:  currentDirection = Direction::South; break;
        case Direction::South: currentDirection = Direction::West; break;
        case Direction::West:  currentDirection = Direction::North; break;
        }
    }
    else {
        switch (currentDirection) {
        case Direction::North: currentDirection = Direction::West; break;
        case Direction::West:  currentDirection = Direction::South; break;
        case Direction::South: currentDirection = Direction::East; break;
        case Direction::East:  currentDirection = Direction::North; break;
        }
    }
    sprite->setRotation(sf::degrees(static_cast<float>(static_cast<int>(currentDirection) * 90)));
    std::cerr << "Player is now facing: " << static_cast<int>(currentDirection) << std::endl;
}

bool Player::isPixelRed(const sf::Image& image, sf::Vector2i pixelPos) {
    if (pixelPos.x < 0 || pixelPos.y < 0 || pixelPos.x >= image.getSize().x || pixelPos.y >= image.getSize().y)
        return true;
    sf::Color pixelColor = image.getPixel(sf::Vector2u(pixelPos.x, pixelPos.y));
    return (pixelColor.r > 200 && pixelColor.g < 100 && pixelColor.b < 100);
}

bool Player::isPixelBrown(const sf::Image& image, sf::Vector2i pixelPos) {
    if (pixelPos.x < 0 || pixelPos.y < 0 || pixelPos.x >= image.getSize().x || pixelPos.y >= image.getSize().y)
        return false;

    sf::Color pixelColor = image.getPixel(sf::Vector2u(pixelPos.x, pixelPos.y));
    return (pixelColor.r == 139 && pixelColor.g == 69 && pixelColor.b == 19);
}

sf::Vector2i Player::getImageCoords(sf::Vector2f screenPos, sf::Vector2f dungeonScale) {
    return sf::Vector2i(
        static_cast<int>(screenPos.x / dungeonScale.x),
        static_cast<int>(screenPos.y / dungeonScale.y)
    );
}

void Player::knock(bool isLeft, const sf::Image& dungeonImage, sf::Vector2f dungeonScale) {
    const float KNOCK_DELAY = 0.9f + static_cast<float>(rand() % 50) / 100.0f;
    const float MIN_KNOCK_RANGE = 1.0f;
    const float MAX_KNOCK_RANGE = 0.9f * (dungeonScale.x + dungeonScale.y) / 2;

    if (knockCooldown.getElapsedTime().asSeconds() < KNOCK_DELAY) return;

    for (float knockOffset = MIN_KNOCK_RANGE; knockOffset <= MAX_KNOCK_RANGE; knockOffset += 0.1f) {
        sf::Vector2i knockPosition = sf::Vector2i(position.x, position.y);
        if (isLeft) {
            switch (currentDirection) {
            case Direction::North: knockPosition.x -= knockOffset; break;
            case Direction::South: knockPosition.x += knockOffset; break;
            case Direction::East:  knockPosition.y -= knockOffset; break;
            case Direction::West:  knockPosition.y += knockOffset; break;
            }
        }
        else {
            switch (currentDirection) {
            case Direction::North: knockPosition.x += knockOffset; break;
            case Direction::South: knockPosition.x -= knockOffset; break;
            case Direction::East:  knockPosition.y += knockOffset; break;
            case Direction::West:  knockPosition.y -= knockOffset; break;
            }
        }

        sf::Vector2i imageCoords = getImageCoords(sf::Vector2f(knockPosition.x, knockPosition.y), dungeonScale);
        if (isPixelRed(dungeonImage, imageCoords)) {
            std::string selectedKnock = knockSoundsStone[rand() % knockSoundsStone.size()];
            knockBuffer.loadFromFile(selectedKnock);
            knockSound->setBuffer(knockBuffer);
            knockSound->setVolume(50);
            knockSound->setPitch(0.6f + static_cast<float>(rand() % 20) / 100.0f);
            knockSound->setPosition(sf::Vector3f(knockPosition.x, 0.0f, knockPosition.y));
            knockSound->setAttenuation(0.1f);
            knockSound->play();

            knockCooldown.restart();
            return;
        }
        else if (isPixelBrown(dungeonImage, imageCoords) && !onDoor)
        {
            std::string selectedKnock = knockSoundsWood[rand() % knockSoundsWood.size()];
            knockBuffer.loadFromFile(selectedKnock);
            knockSound->setBuffer(knockBuffer);
            knockSound->setVolume(100);
            knockSound->setPitch(0.9f + static_cast<float>(rand() % 20) / 100.0f);
            knockSound->setPosition(sf::Vector3f(knockPosition.x, 0.0f, knockPosition.y));
            knockSound->setAttenuation(0.1f);
            knockSound->play();

            knockCooldown.restart();
            return;
        }
    }
    std::string selectedClick = clickSounds[rand() % clickSounds.size()];
    knockBuffer.loadFromFile(selectedClick);
    knockSound->setBuffer(knockBuffer);
    knockSound->setVolume(50);
    knockSound->setPitch(0.5f + static_cast<float>(rand() % 20) / 100.0f);
    float clickOffset = 1.0f;
    sf::Vector2i knockPosition = sf::Vector2i(position.x, position.y);
    if (isLeft) {
        switch (currentDirection) {
        case Direction::North: knockPosition.x -= clickOffset; break;  
        case Direction::South: knockPosition.x += clickOffset; break;
        case Direction::East:  knockPosition.y -= clickOffset; break;
        case Direction::West:  knockPosition.y += clickOffset; break;
        }
    }
    else {
        switch (currentDirection) {
        case Direction::North: knockPosition.x += clickOffset; break;  
        case Direction::South: knockPosition.x -= clickOffset; break;
        case Direction::East:  knockPosition.y += clickOffset; break;
        case Direction::West:  knockPosition.y -= clickOffset; break;
        }
    }
    knockSound->setPosition(sf::Vector3f(knockPosition.x, 0.0f, knockPosition.y));
    knockSound->setAttenuation(0.1f);
    knockSound->play();
    knockCooldown.restart();
}

void Player::stepForwardAndInteract(float SCALE_FACTOR, const sf::Image& dungeonImage, sf::Vector2f dungeonScale) {
    const float MOVE_RATE = static_cast<float>(SCALE_FACTOR) * 0.05;
    const float FOOTSTEP_DELAY = 0.9f + static_cast<float>(rand() % 50) / 100.0f;
    const float damage_DELAY = 5.0f + static_cast<float>(rand() % 50) / 100.0f;
    sf::Vector2f nextPosition = sf::Vector2f(position.x, position.y);
    sf::Vector2f directionVector = DIRECTION_MAP_SPRITE.at(currentDirection);
    nextPosition.x += static_cast<float>(directionVector.x * MOVE_RATE);
    nextPosition.y += static_cast<float>(directionVector.y * MOVE_RATE);
    sf::Vector2i imageCoords = getImageCoords(nextPosition, dungeonScale);
    const float KNOCK_DELAY = 0.9f + static_cast<float>(rand() % 50) / 100.0f;
    if (!isPixelRed(dungeonImage, imageCoords)) {
        position.x = nextPosition.x;
        position.y = nextPosition.y;
        listenerPosition.x = position.x;
        listenerPosition.y = position.y;
        sprite->setPosition(sf::Vector2f(position.x, position.y));
        if (!onDoor && isPixelBrown(dungeonImage, imageCoords))
        {
            onDoor = true;
            std::string selectedSound = doorOpenSounds[rand() % doorOpenSounds.size()];
            doorBuffer.loadFromFile(selectedSound);
            doorSound->setBuffer(doorBuffer);
            doorSound->setPitch(0.8f + static_cast<float>(rand() % 20) / 100.0f);
            doorSound->setVolume(100);
            doorSound->setPosition(sf::Vector3f(listenerPosition.x, -0.5f, listenerPosition.y));
            doorSound->setAttenuation(0);
            doorSound->play();
        }
        else if (onDoor && !isPixelBrown(dungeonImage, imageCoords))
        {
            onDoor = false;
            std::string selectedSound = doorSlamSounds[rand() % doorSlamSounds.size()];
            doorBuffer.loadFromFile(selectedSound);
            doorSound->setBuffer(doorBuffer);
            doorSound->setPitch(0.8f + static_cast<float>(rand() % 20) / 100.0f);
            doorSound->setVolume(100);
            doorSound->setAttenuation(0);
            sf::Vector3f slamPosition = listenerPosition;
            switch (currentDirection) {
            case Direction::North: slamPosition.y += 2.0f; break;
            case Direction::South: slamPosition.y -= 2.0f; break;
            case Direction::East:  slamPosition.x -= 2.0f; break;
            case Direction::West:  slamPosition.x += 2.0f; break;
            }
            doorSound->setPosition(slamPosition);
            doorSound->play();
        }
        if (footstepCooldown.getElapsedTime().asSeconds() >= FOOTSTEP_DELAY) {
            std::string selectedSound = footstepSounds[rand() % footstepSounds.size()];
            footstepBuffer.loadFromFile(selectedSound);
            footstepSound->setBuffer(footstepBuffer);
            footstepSound->setPitch(0.8f + static_cast<float>(rand() % 20) / 100.0f);
            footstepSound->setVolume(150);
            float offsetX = (static_cast<float>(rand() % 200) / 100.0f) - 1.0f;
            float offsetZ = (static_cast<float>(rand() % 200) / 100.0f) - 1.0f;
            footstepSound->setPosition(sf::Vector3f(listenerPosition.x + offsetX, -5.0f, listenerPosition.y + offsetZ));
            footstepSound->setAttenuation(1);
            footstepSound->play();
            footstepCooldown.restart();
        }
        if (dungeonPtr->getGridToRoom()->find(imageCoords) != dungeonPtr->getGridToRoom()->end()) 
        {
            Room* currentRoom = dungeonPtr->getGridToRoom()->at(imageCoords);  
            std::string selectedSound;

            if (!currentRoom->isExplored) {
                currentRoom->isExplored = true;
                gameScore += 10;
                std::cout << "Entered a new room! Score: " << gameScore << std::endl;
                if (currentRoom->type == RoomType::Beneficial)
                {
                    gameScore += 25;
                    selectedSound = reliefNoises[rand() % reliefNoises.size()];
                    if (!voiceBuffer.loadFromFile(selectedSound))
                    {
                        std::cerr << "Error: Failed to load " << selectedSound << std::endl;
                        return;
                    }
                    voiceSound->setAttenuation(0);
                    voiceSound->setBuffer(voiceBuffer);
                    voiceSound->setPitch(0.8f + static_cast<float>(rand() % 20) / 100.0f);
                    voiceSound->setVolume(150);
                    voiceSound->setPosition(sf::Vector3f(listenerPosition.x, 0.0f, listenerPosition.y));
                    voiceSound->play();
                    std::cout << "Sound is playing!" << std::endl;
                }
            }

            if (currentRoom->type == RoomType::End)
            {
                isEndRoom = true;
            }

            if (currentRoom->type == RoomType::Hazardous && damageCooldown.getElapsedTime().asSeconds() >= damage_DELAY)
            {
                damageCooldown.restart();
                gameScore -= 10;
                

                switch (currentRoom->subType)
                {
                case RoomSubType::h_FlamingRoom:
                    selectedSound = firePainNoises[rand() % firePainNoises.size()];
                    if (!voiceBuffer.loadFromFile(selectedSound))
                    {
                        std::cerr << "Error: Failed to load " << selectedSound << std::endl;
                        return;
                    }
                    voiceSound->setAttenuation(0);
                    voiceSound->setBuffer(voiceBuffer);
                    voiceSound->setPitch(0.8f + static_cast<float>(rand() % 20) / 100.0f);
                    voiceSound->setVolume(150);
                    voiceSound->setPosition(sf::Vector3f(listenerPosition.x, 0.0f, listenerPosition.y));
                    voiceSound->play();
                    std::cout << "Sound is playing!" << std::endl;
                    break;

                case RoomSubType::h_GasRoom:
                    selectedSound = poisonPainNoises[rand() % poisonPainNoises.size()];
                    if (!voiceBuffer.loadFromFile(selectedSound))
                    {
                        std::cerr << "Error: Failed to load " << selectedSound << std::endl;
                        return;
                    }
                    voiceSound->setAttenuation(0);
                    voiceSound->setBuffer(voiceBuffer);
                    voiceSound->setPitch(0.8f + static_cast<float>(rand() % 20) / 100.0f);
                    voiceSound->setVolume(150);
                    voiceSound->setPosition(sf::Vector3f(listenerPosition.x, 0.0f, listenerPosition.y));
                    voiceSound->play();
                    std::cout << "Sound is playing!" << std::endl;
                    break;
                default:
                    break;
                }
            }
        }
    }
    else if (footstepCooldown.getElapsedTime().asSeconds() >= FOOTSTEP_DELAY && isPixelRed(dungeonImage, imageCoords) && (knockCooldown.getElapsedTime().asSeconds() >= KNOCK_DELAY)) {
        std::string selectedKnockLeft = knockSoundsStone[rand() % knockSoundsStone.size()];
        std::string selectedKnockRight = knockSoundsStone[rand() % knockSoundsStone.size()];
        knockBuffer.loadFromFile(selectedKnockLeft);
        footstepBuffer.loadFromFile(selectedKnockRight);
        knockSound->setBuffer(knockBuffer);
        knockSound2->setBuffer(footstepBuffer);
        for (int i = -1; i <= 1; i += 2) {
            sf::Vector2f knockPosition = nextPosition;
            switch (currentDirection) {
            case Direction::North: knockPosition.x += i; break;
            case Direction::South: knockPosition.x -= i; break;
            case Direction::East:  knockPosition.y += i; break;
            case Direction::West:  knockPosition.y -= i; break;
            }
            float knockOffsetX = (static_cast<float>(rand() % 200) / 100.0f) - 1.0f;
            if (i == -1) { 
                knockSound->setPitch(0.6f + static_cast<float>(rand() % 20) / 100.0f);
                knockSound->setPosition(sf::Vector3f(knockPosition.x + knockOffsetX, 0.0f, knockPosition.y));
                knockSound->setVolume(40);
                knockSound->setAttenuation(1);
                knockSound->play();
            }
            else {
                sf::sleep(sf::milliseconds(50));
                knockSound2->setPitch(0.6f + static_cast<float>(rand() % 20) / 100.0f);
                knockSound2->setPosition(sf::Vector3f(knockPosition.x - knockOffsetX, 0.0f, knockPosition.y));
                knockSound2->setVolume(40);
                knockSound2->setAttenuation(1);
                knockSound2->play();
            }
        }
        footstepCooldown.restart();
        knockCooldown.restart();
    }
}

void Player::handleMovement(float SCALE_FACTOR, sf::Clock& rotationCooldown, const sf::Image& dungeonImage, sf::Vector2f dungeonScale) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        stepForwardAndInteract(SCALE_FACTOR, dungeonImage, dungeonScale);
    }
    if (rotationCooldown.getElapsedTime().asSeconds() >= 1.0f) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J)) { rotate(true); rotationCooldown.restart(); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) { rotate(false); rotationCooldown.restart(); }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { knock(true, dungeonImage, dungeonScale); }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) { knock(false, dungeonImage, dungeonScale); }
    sf::Listener::setPosition(sf::Vector3f(listenerPosition.x, 0.0f, listenerPosition.y));
    sf::Listener::setDirection(sf::Vector3f(DIRECTION_MAP_SPRITE.at(currentDirection).x, 0.0f, DIRECTION_MAP_SPRITE.at(currentDirection).y));
}

int Player::getScore() { return gameScore; }