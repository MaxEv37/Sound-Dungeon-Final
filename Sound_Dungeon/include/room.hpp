#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum RoomType {
    Start,
    End,
    Neutral,
    Hazardous,
    Beneficial,
    Cave
};

enum RoomSubType {
    None,         // Neutral rooms or generic cases
    b_Treasure,     // Beneficial - coins & gems
    b_Rest,         // Beneficial - crackling campfire
    b_Garden,       // Beneficial - nature ambiance
    h_FlamingRoom,  // Hazardous - sections are on fire
    h_GasRoom,      // Hazardous - poison gas fills the air
    c_EchoCave,     // Cave - eerie echoing sounds
    v_startRoom,
    v_endRoom
};

class Room {
public:
    std::vector<sf::Vector2i> gridPosition;
    std::vector<sf::Vector2i*> exits; // List of door positions
    bool isValid = false;
    bool connected = false;
    bool isExplored = false;
    RoomType type = RoomType::Neutral;
    RoomSubType subType = RoomSubType::None;
    int soundRatio = 100;
    std::vector<std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    std::vector<std::unique_ptr<sf::Sound>> activeSounds;  // Store playing sounds inside Room
};