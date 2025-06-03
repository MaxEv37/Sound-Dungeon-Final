#pragma once

#include <vector>
#include <memory>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "room.hpp"
#include <map>

class Dungeon {

public:
    Dungeon(sf::Image& image) {
        generateDungeon(image);
    }
    ~Dungeon();

    // Scoring system
    struct HashVector2i {
        std::size_t operator()(const sf::Vector2i& v) const noexcept {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };

    const std::vector<std::unique_ptr<Room>>& getRooms();
    sf::Vector3f getStartPosition(sf::Vector2f scaleFactor) const;
    const std::unordered_map<sf::Vector2i, Room*, HashVector2i>* getGridToRoom() const {
        return &gridToRoom; 
    }
    void popRoomAudio(sf::Vector2f dungeonScale);
    void startUpAudio();
    void regenerateDungeon(sf::Image& image);

private:
    void generateDungeon(sf::Image& image);
    std::unordered_set<Room*> cullDisconnectedRooms(sf::Image& image);
    void popRoomList(sf::Image& image);
    void setStartAndEndRooms(sf::Image& image);
    void floodFill(sf::Image& image, int x, int y, sf::Color targetColor, Room* room, bool& touchesBorder);
    void removeEdgeWalls(sf::Image& image);
    void restoreValidRoomWalls(sf::Image& image, Room* room);
    void removeInvalidRoomPixels(sf::Image& image, Room* room);
    void placeDoors(sf::Image& image);
    void classifyRooms();
    void colourRooms(sf::Image& image);
    void convertToCaveAndDrunkardsWalk(sf::Image& image, Room* room);
    void deadEndsToCaves(sf::Image& image);

    Room* startRoom;
    Room* endRoom;

    std::vector<std::unique_ptr<Room>> validRooms;

    std::vector<Room*> audioRooms;

    sf::Image dungeonTexture;

    std::map<RoomSubType, std::vector<std::string>> roomSounds = {
        {RoomSubType::b_Treasure, {"treasure_1.wav", "treasure_2.wav", "treasure_3.wav"}},
        {RoomSubType::b_Rest, {"campfire_crackle_1.wav"}},
        {RoomSubType::b_Garden, {"garden_1.wav"}},
        {RoomSubType::h_GasRoom, {"gas_hiss_1.wav"}},
        {RoomSubType::h_FlamingRoom, {"flamingRoom_1.wav"}},
        {RoomSubType::c_EchoCave, {"cave_1.wav", "cave_2.wav", "cave_3.wav"}},
        {RoomSubType::v_startRoom, {"startRoom_1.wav"}},
        {RoomSubType::v_endRoom, {"endRoom_1.wav"}}
    };

    std::unordered_map<sf::Vector2i, Room*, HashVector2i> gridToRoom;
};