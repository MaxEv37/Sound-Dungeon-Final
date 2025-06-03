#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include "dungeon.hpp"

enum class Direction {
    North,
    East,
    South,
    West
};

class Player {
public:
    Player(sf::Vector3f startPosition, std::unique_ptr<sf::Sprite>& spritePtr, Dungeon* dungeon);

    const sf::Vector3f& getPosition() const;
    const sf::Sprite& getSprite() const;
    const sf::Vector3f& getListenerPosition() const;
    sf::Vector3f getListenerDirection() const;

    sf::Vector2i getImageCoords(sf::Vector2f screenPos, sf::Vector2f dungeonScale);
    bool isPixelRed(const sf::Image& image, sf::Vector2i pixelPos);
    bool isPixelBrown(const sf::Image& image, sf::Vector2i pixelPos);
    bool inEndRoom() { return isEndRoom; };
    void setPosition(const sf::Vector3f& newPosition);
    void rotate(bool clockwise);
    void handleMovement(float SCALE_FACTOR, sf::Clock& rotationCooldown, const sf::Image& dungeonImage, sf::Vector2f dungeonScale);
    int getScore();
    void stepForwardAndInteract(float SCALE_FACTOR, const sf::Image& dungeonImage, sf::Vector2f dungeonScale);
    void knock(bool isLeft, const sf::Image& dungeonImage, sf::Vector2f dungeonScale);

private:
    Dungeon* dungeonPtr;
    std::unique_ptr<sf::Sprite>& sprite;
    std::unique_ptr<sf::Sound> footstepSound;
    std::unique_ptr<sf::Sound> knockSound;
    std::unique_ptr<sf::Sound> knockSound2;
    std::unique_ptr<sf::Sound> doorSound;
    std::unique_ptr<sf::Sound> voiceSound;

    std::vector<std::string> footstepSounds = {
        "../Sound_Dungeon/audio/footstep_1.wav", "../Sound_Dungeon/audio/footstep_2.wav",
        "../Sound_Dungeon/audio/footstep_3.wav", "../Sound_Dungeon/audio/footstep_4.wav",
        "../Sound_Dungeon/audio/footstep_5.wav", "../Sound_Dungeon/audio/footstep_6.wav",
        "../Sound_Dungeon/audio/footstep_7.wav", "../Sound_Dungeon/audio/footstep_8.wav"
    };

    std::vector<std::string> knockSoundsStone = {
        "../Sound_Dungeon/audio/knock_stone_1.wav", "../Sound_Dungeon/audio/knock_stone_2.wav",
        "../Sound_Dungeon/audio/knock_stone_3.wav", "../Sound_Dungeon/audio/knock_stone_4.wav"
    };

    std::vector<std::string> knockSoundsWood = {
        "../Sound_Dungeon/audio/knock_wood_1.wav", "../Sound_Dungeon/audio/knock_wood_2.wav",
        "../Sound_Dungeon/audio/knock_wood_3.wav"
    };

    std::vector<std::string> clickSounds = {
        "../Sound_Dungeon/audio/finger_click_1.wav", "../Sound_Dungeon/audio/finger_click_2.wav",
        "../Sound_Dungeon/audio/finger_click_3.wav", "../Sound_Dungeon/audio/finger_click_4.wav"
    };

    std::vector<std::string> doorOpenSounds = {
        "../Sound_Dungeon/audio/door_open_1.wav", "../Sound_Dungeon/audio/door_open_2.wav",
        "../Sound_Dungeon/audio/door_open_3.wav"
    };

    std::vector<std::string> doorSlamSounds = {
        "../Sound_Dungeon/audio/door_slam_1.wav", "../Sound_Dungeon/audio/door_slam_2.wav"
    };

    std::vector<std::string> firePainNoises = {
        "../Sound_Dungeon/audio/Fire_Noises_1.wav", "../Sound_Dungeon/audio/Fire_Noises_2.wav",
        "../Sound_Dungeon/audio/Fire_Noises_3.wav", "../Sound_Dungeon/audio/Fire_Noises_4.wav",
        "../Sound_Dungeon/audio/Fire_Noises_5.wav"
    };

    std::vector<std::string> poisonPainNoises = {
        "../Sound_Dungeon/audio/Poison_Sounds_1.wav", "../Sound_Dungeon/audio/Poison_Sounds_2.wav",
        "../Sound_Dungeon/audio/Poison_Sounds_3.wav", "../Sound_Dungeon/audio/Poison_Sounds_4.wav",
        "../Sound_Dungeon/audio/Poison_Sounds_5.wav"
    };

    std::vector<std::string> reliefNoises = {
        "../Sound_Dungeon/audio/Relief_1.wav", "../Sound_Dungeon/audio/Relief_2.wav",
        "../Sound_Dungeon/audio/Relief_3.wav"
    };

    const std::map<Direction, sf::Vector2f> DIRECTION_MAP_SPRITE = {
        {Direction::North, {0.0f, -1.0f}}, {Direction::South, {0.0f, 1.0f}},
        {Direction::East, {1.0f, 0.0f}}, {Direction::West, {-1.0f, 0.0f}}
    };

    const std::map<Direction, sf::Vector3f> DIRECTION_MAP = {
        {Direction::North, {0.0f, 0.0f, -1.0f}}, {Direction::South, {0.0f, 0.0f, 1.0f}},
        {Direction::East, {-1.0f, 0.0f, 0.0f}}, {Direction::West, {1.0f, 0.0f, 0.0f}}
    };

    sf::Vector3f position;
    sf::Vector3f listenerPosition;
    sf::Vector3f listenerDirection;

    sf::SoundBuffer footstepBuffer;
    sf::SoundBuffer knockBuffer;
    sf::SoundBuffer doorBuffer;
    sf::SoundBuffer voiceBuffer;

    int gameScore = 0;
    Direction currentDirection = Direction::North;
    bool isLeft = true;
    bool onDoor = false;
    bool isEndRoom = false;

    sf::Clock footstepCooldown;
    sf::Clock knockCooldown;
    sf::Clock damageCooldown;
};

#endif