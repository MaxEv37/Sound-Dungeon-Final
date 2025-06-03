#include "../include/dungeon.hpp"
#include "../include/pathfinding.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <SFML/Audio.hpp>

Dungeon::~Dungeon()
{ 
}

const std::vector<std::unique_ptr<Room>>& Dungeon::getRooms()
{
    return validRooms; 
}

sf::Vector3f Dungeon::getStartPosition(sf::Vector2f scaleFactor) const {
    if (!startRoom || startRoom->gridPosition.empty()) {
        std::cerr << "Error: No valid start room!" << std::endl;
        return sf::Vector3f(0, 0, 0);  // Return default position if invalid
    }

    // Pick any pixel from the `gridPosition` (first pixel for simplicity)
    sf::Vector2i startPixel = startRoom->gridPosition.front();

    return sf::Vector3f(static_cast<float>(startPixel.x) * scaleFactor.x, static_cast<float>(startPixel.y) * scaleFactor.y, 0.0f);
}

void Dungeon::generateDungeon(sf::Image& image) {
    popRoomList(image);
    cullDisconnectedRooms(image);
    setStartAndEndRooms(image);
    classifyRooms();
    placeDoors(image);
    std::vector<sf::Vector2i> path1;
    std::vector<sf::Vector2i> path2;
    std::vector<sf::Vector2i> path3;
    sf::Vector2u textureSize = image.getSize();
    sf::Vector2i topLeftRoom = Pathfinding::findClosestRoom(sf::Vector2i(0, 0), validRooms);
    sf::Vector2i topRightRoom = Pathfinding::findClosestRoom(sf::Vector2i(static_cast<int>(textureSize.x) - 1, 0), validRooms);
    sf::Vector2i bottomLeftRoom = Pathfinding::findClosestRoom(sf::Vector2i(0, static_cast<int>(textureSize.y) - 1), validRooms);
    sf::Vector2i bottomRightRoom = Pathfinding::findClosestRoom(sf::Vector2i(static_cast<int>(textureSize.x) - 1, static_cast<int>(textureSize.y) - 1), validRooms);
    path1 = Pathfinding::findPath(image, topLeftRoom, bottomRightRoom);
    path2 = Pathfinding::findPath(image, topRightRoom, bottomLeftRoom);
    path3 = Pathfinding::findPath(image, startRoom->gridPosition.front(), endRoom->gridPosition.front());
    colourRooms(image);
    sf::Color red(255, 0, 0); 
    sf::Color black(0, 0, 0);   
    sf::Color white(255, 255, 255); 
    for (const sf::Vector2i& pixel : path1) {
        sf::Color currentColor = image.getPixel(sf::Vector2u(pixel.x, pixel.y));
        if (currentColor == red) {
            image.setPixel(sf::Vector2u(pixel.x, pixel.y), white);
            std::vector<sf::Vector2i> directions = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
                {1, 1}, {-1, -1}, {1, -1}, {-1, 1} 
            };
            for (const sf::Vector2i& direction : directions) {
                sf::Vector2i neighbor = pixel + direction;
                if (image.getPixel(sf::Vector2u(neighbor.x, neighbor.y)) == black) {
                    image.setPixel(sf::Vector2u(neighbor.x, neighbor.y), red);
                }
            }
        }
    }
    for (const sf::Vector2i& pixel : path2) {
        sf::Color currentColor = image.getPixel(sf::Vector2u(pixel.x, pixel.y));
        if (currentColor == red) {
            image.setPixel(sf::Vector2u(pixel.x, pixel.y), white);
            std::vector<sf::Vector2i> directions = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
                {1, 1}, {-1, -1}, {1, -1}, {-1, 1} 
            };
            for (const sf::Vector2i& direction : directions) {
                sf::Vector2i neighbor = pixel + direction;
                if (image.getPixel(sf::Vector2u(neighbor.x, neighbor.y)) == black) {
                    image.setPixel(sf::Vector2u(neighbor.x, neighbor.y), red);
                }
            }
        }
    }
    for (const sf::Vector2i& pixel : path3) {
        sf::Color currentColor = image.getPixel(sf::Vector2u(pixel.x, pixel.y));

        if (currentColor == red) {
            image.setPixel(sf::Vector2u(pixel.x, pixel.y), white);

            std::vector<sf::Vector2i> directions = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
                {1, 1}, {-1, -1}, {1, -1}, {-1, 1} 
            };
            for (const sf::Vector2i& direction : directions) {
                sf::Vector2i neighbor = pixel + direction;
                if (image.getPixel(sf::Vector2u(neighbor.x, neighbor.y)) == black) {
                    image.setPixel(sf::Vector2u(neighbor.x, neighbor.y), red);
                }
            }
        }
    }
    image.saveToFile("../Sound_Dungeon/build/results/Dungeon_Gen_Modified_Dev.png");
    for (const auto& room : validRooms) {
        for (const sf::Vector2i& pixel : room->gridPosition) {
            image.setPixel(sf::Vector2u(pixel.x, pixel.y), white);
            gridToRoom[pixel] = room.get();
        }
    }
    image.saveToFile("../Sound_Dungeon/build/results/Dungeon_Gen_Modified.png");

    std::cout << "Dungeon generation complete with " << validRooms.size() << " rooms!\n";
}

std::unordered_set<Room*> Dungeon::cullDisconnectedRooms(sf::Image& image) {
    const int REQUIRED_ROOMS = 25;
    bool bEnoughRooms = false;
    while (!bEnoughRooms) {
        std::unordered_set<Room*> connectedRooms;
        Room* primarySeed = nullptr;

        for (const auto& room : validRooms) {
            if (!room->connected) {
                primarySeed = room.get();
                break;
            }
        }
        if (!primarySeed) return connectedRooms;
        connectedRooms.insert(primarySeed);
        primarySeed->connected = true;
        std::vector<Room*> queue = { primarySeed };
        while (!queue.empty()) {
            Room* currentRoom = queue.back();
            queue.pop_back();
            for (auto& otherRoom : validRooms) {
                Room* roomPtr = otherRoom.get();
                if (roomPtr->connected) continue;
                for (const sf::Vector2i& pixel : currentRoom->gridPosition) {
                    for (const sf::Vector2i& otherPixel : roomPtr->gridPosition) {
                        if ((std::abs(pixel.x - otherPixel.x) <= 3 && pixel.y == otherPixel.y) ||
                            (std::abs(pixel.y - otherPixel.y) <= 3 && pixel.x == otherPixel.x) ||
                            (std::abs(pixel.x - otherPixel.x) <= 3 && std::abs(pixel.y - otherPixel.y) <= 3)) {
                            roomPtr->connected = true;
                            connectedRooms.insert(roomPtr);
                            queue.push_back(roomPtr);
                            break;
                        }
                    }
                }
            }
        }
        if (connectedRooms.size() >= REQUIRED_ROOMS) {
            bEnoughRooms = true;
            validRooms.erase(std::remove_if(validRooms.begin(), validRooms.end(),
                [&](std::unique_ptr<Room>& room) {
                    if (!room->connected) {
                        removeInvalidRoomPixels(image, room.get());
                        return true;
                    }
                    return false;
                }), validRooms.end());
            return connectedRooms;
        }
        else {
            std::cerr << "Connectivity failed with " << connectedRooms.size() << " rooms in cluster.\n";
            validRooms.erase(std::remove_if(validRooms.begin(), validRooms.end(),
                [&](std::unique_ptr<Room>& room) {
                    if (room->connected) {
                        removeInvalidRoomPixels(image, room.get());
                        return true;
                    }
                    return false;
                }), validRooms.end());
            if (validRooms.size() < REQUIRED_ROOMS) {
                std::cerr << "Regenerating dungeon due to insufficient connectivity...\n";
                regenerateDungeon(image);
            }
        }
    }
}

void Dungeon::popRoomList(sf::Image& image) {
    validRooms.clear();
    sf::Color white(255, 255, 255);
    std::vector<std::unique_ptr<Room>> tempRooms;
    for (int y = 0; y < image.getSize().y; ++y) {
        for (int x = 0; x < image.getSize().x; ++x) {
            if (image.getPixel(sf::Vector2u(x, y)) == white) {
                auto room = std::make_unique<Room>();
                bool touchesBorder = false;
                floodFill(image, x, y, white, room.get(), touchesBorder);
                if (!touchesBorder) {
                    room->isValid = true;
                    tempRooms.push_back(std::move(room)); 
                }
                else {
                    removeInvalidRoomPixels(image, room.get());
                }
            }
        }
    }
    removeEdgeWalls(image);
    for (const auto& room : tempRooms) { 
        restoreValidRoomWalls(image, room.get());
    }
    validRooms = std::move(tempRooms); 
}

void Dungeon::setStartAndEndRooms(sf::Image& image) {
    if (validRooms.size() < 2) {
        std::cerr << "Error: Not enough rooms to set start and end!" << std::endl;
        return;
    }
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> dist(0, validRooms.size() - 1);
    size_t startIndex = dist(rng);
    size_t endIndex;
    do {
        endIndex = dist(rng);
    } while (startIndex == endIndex);  // Ensure different rooms
    startRoom = validRooms[startIndex].get();
    endRoom = validRooms[endIndex].get();
    if (!startRoom || !endRoom) {
        std::cerr << "Error: startRoom or endRoom is NULL!" << std::endl;
        return;
    }
    startRoom->type = RoomType::Start;
    endRoom->type = RoomType::End;
}

void Dungeon::floodFill(sf::Image& image, int x, int y, sf::Color targetColor, Room* room, bool& touchesBorder) {
    if (image.getPixel(sf::Vector2u(x, y)) != targetColor) return;
    std::vector<sf::Vector2i> stack;
    stack.push_back({ x, y });
    sf::Vector2u imageSize = image.getSize();
    while (!stack.empty()) {
        sf::Vector2i pixel = stack.back();
        stack.pop_back();
        if (image.getPixel(sf::Vector2u(pixel.x, pixel.y)) == targetColor) {
            image.setPixel(sf::Vector2u(pixel.x, pixel.y), sf::Color(255, 105, 180)); // Mark room as done (pink)
            room->gridPosition.push_back(pixel);
            if (pixel.x == 0 || pixel.x == imageSize.x - 1 || pixel.y == 0 || pixel.y == imageSize.y - 1) {
                touchesBorder = true;
            }
            if (pixel.x > 0) stack.push_back({ pixel.x - 1, pixel.y });
            if (pixel.x < imageSize.x - 1) stack.push_back({ pixel.x + 1, pixel.y });
            if (pixel.y > 0) stack.push_back({ pixel.x, pixel.y - 1 });
            if (pixel.y < imageSize.y - 1) stack.push_back({ pixel.x, pixel.y + 1 });
        }
    }
}

void Dungeon::removeEdgeWalls(sf::Image& image) {
    sf::Color black(0, 0, 0);
    sf::Vector2u imageSize = image.getSize();

    // Make entire border black
    for (unsigned int x = 0; x < imageSize.x; ++x) {
        image.setPixel(sf::Vector2u(x, 0), black);
        image.setPixel(sf::Vector2u(x, imageSize.y - 1), black);
    }
    for (unsigned int y = 0; y < imageSize.y; ++y) {
        image.setPixel(sf::Vector2u(0, y), black);
        image.setPixel(sf::Vector2u(imageSize.x - 1, y), black);
    }
} 

void Dungeon::restoreValidRoomWalls(sf::Image& image, Room* room) {
    sf::Color red(255, 0, 0);

    for (const sf::Vector2i& pixel : room->gridPosition) {
        // Restore horizontal & vertical walls
        if (pixel.x > 0 && image.getPixel(sf::Vector2u(pixel.x - 1, pixel.y)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x - 1, pixel.y), red);
        if (pixel.x < image.getSize().x - 1 && image.getPixel(sf::Vector2u(pixel.x + 1, pixel.y)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x + 1, pixel.y), red);
        if (pixel.y > 0 && image.getPixel(sf::Vector2u(pixel.x, pixel.y - 1)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x, pixel.y - 1), red);
        if (pixel.y < image.getSize().y - 1 && image.getPixel(sf::Vector2u(pixel.x, pixel.y + 1)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x, pixel.y + 1), red);

        // Restore diagonal walls (corners)
        if (pixel.x > 0 && pixel.y > 0 && image.getPixel(sf::Vector2u(pixel.x - 1, pixel.y - 1)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x - 1, pixel.y - 1), red);
        if (pixel.x > 0 && pixel.y < image.getSize().y - 1 && image.getPixel(sf::Vector2u(pixel.x - 1, pixel.y + 1)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x - 1, pixel.y + 1), red);
        if (pixel.x < image.getSize().x - 1 && pixel.y > 0 && image.getPixel(sf::Vector2u(pixel.x + 1, pixel.y - 1)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x + 1, pixel.y - 1), red);
        if (pixel.x < image.getSize().x - 1 && pixel.y < image.getSize().y - 1 && image.getPixel(sf::Vector2u(pixel.x + 1, pixel.y + 1)) == sf::Color(0, 0, 0))
            image.setPixel(sf::Vector2u(pixel.x + 1, pixel.y + 1), red);
    }
}   //CHECKED

void Dungeon::removeInvalidRoomPixels(sf::Image& image, Room* room) {
    sf::Color black(0, 0, 0);

    for (const sf::Vector2i& pixel : room->gridPosition) {
        image.setPixel(sf::Vector2u(pixel.x, pixel.y), black);

        // Remove all adjacent pixels (walls)
        if (pixel.x > 0) image.setPixel(sf::Vector2u(pixel.x - 1, pixel.y), black);
        if (pixel.x < image.getSize().x - 1) image.setPixel(sf::Vector2u(pixel.x + 1, pixel.y), black);
        if (pixel.y > 0) image.setPixel(sf::Vector2u(pixel.x, pixel.y - 1), black);
        if (pixel.y < image.getSize().y - 1) image.setPixel(sf::Vector2u(pixel.x, pixel.y + 1), black);

        // Restore diagonal pixel removal (corners)
        if (pixel.x > 0 && pixel.y > 0) image.setPixel(sf::Vector2u(pixel.x - 1, pixel.y - 1), black);
        if (pixel.x > 0 && pixel.y < image.getSize().y - 1) image.setPixel(sf::Vector2u(pixel.x - 1, pixel.y + 1), black);
        if (pixel.x < image.getSize().x - 1 && pixel.y > 0) image.setPixel(sf::Vector2u(pixel.x + 1, pixel.y - 1), black);
        if (pixel.x < image.getSize().x - 1 && pixel.y < image.getSize().y - 1) image.setPixel(sf::Vector2u(pixel.x + 1, pixel.y + 1), black);
    }
}

void Dungeon::colourRooms(sf::Image& image) {
    sf::Color green(0, 255, 0);      // Start Room
    sf::Color dark_green(0, 100, 0); // End Room
    sf::Color white(255, 255, 255);  // Neutral
    sf::Color yellow(255, 255, 0);   // Hazardous
    sf::Color lightBlue(173, 216, 230); // Beneficial
    sf::Color grey(128, 128, 128);   // Cave

    for (auto& room : validRooms) {
        sf::Color roomColor = white;

        switch (room->type) {
        case RoomType::Start:
            roomColor = green;
            break;
        case RoomType::End:
            roomColor = dark_green;
            break;
        case RoomType::Hazardous:
            roomColor = yellow;
            break;
        case RoomType::Beneficial:
            roomColor = lightBlue;
            break;
        case RoomType::Cave:
            roomColor = grey;
            break;
        default:
            break;
        }

        for (const sf::Vector2i& pixel : room->gridPosition) {
            image.setPixel(sf::Vector2u(pixel.x, pixel.y), roomColor);
        }
    }
}

void Dungeon::convertToCaveAndDrunkardsWalk(sf::Image& image, Room* room) {
    sf::Color red(255, 0, 0);   // Walls  
    sf::Color black(0, 0, 0);   // Empty space  
    sf::Color white(255, 255, 255); // Walkable cave space  
    sf::Color brown(139, 69, 19);  // Doors  

    room->type = RoomType::Cave;

    for (const sf::Vector2i& pixel : room->gridPosition) {
        for (const auto& direction : std::vector<sf::Vector2i>{ {1, 0}, {-1, 0}, {0, 1}, {0, -1} }) {
            sf::Vector2i neighbor = pixel + direction;
            if (image.getPixel(sf::Vector2u(neighbor.x, neighbor.y)) == red) {
                image.setPixel(sf::Vector2u(neighbor.x, neighbor.y), black);
            }
        }
    }

    sf::Vector2i currentPos = room->gridPosition[rand() % room->gridPosition.size()];
    int doorsPlaced = 0;

    while (true) {
        image.setPixel(sf::Vector2u(currentPos.x, currentPos.y), white);
        room->gridPosition.push_back(currentPos);

        sf::Vector2i direction = std::vector<sf::Vector2i>{ {1, 0}, {-1, 0}, {0, 1}, {0, -1} }[rand() % 4];
        sf::Vector2i nextPos = currentPos + direction;

        if (nextPos.x < 2 || nextPos.y < 2 ||
            nextPos.x >= image.getSize().x - 2 || nextPos.y >= image.getSize().y - 2) {
            continue;
        }

        if (image.getPixel(sf::Vector2u(nextPos.x, nextPos.y)) == red) {
            sf::Vector2i doorPosition1 = nextPos;
            sf::Vector2i doorPosition2 = { nextPos.x + direction.x, nextPos.y + direction.y };
            sf::Vector2i beyondDoor = { doorPosition2.x + direction.x, doorPosition2.y + direction.y };

            bool validDoor = false;
            for (const auto& r : validRooms) {
                if (!r->connected) continue;  // Reverting to connectivity check
                for (const sf::Vector2i& pixel : r->gridPosition) {
                    if (pixel == doorPosition1 || pixel == doorPosition2 || pixel == beyondDoor) {
                        validDoor = true;
                        break;
                    }
                }
                if (validDoor) break;
            }

            if (validDoor) {
                image.setPixel(sf::Vector2u(doorPosition1.x, doorPosition1.y), brown);
                image.setPixel(sf::Vector2u(doorPosition2.x, doorPosition2.y), brown);
                room->exits.push_back(new sf::Vector2i(doorPosition1));
                room->exits.push_back(new sf::Vector2i(doorPosition2));

                room->connected = true;
                doorsPlaced++;
                currentPos = room->gridPosition[rand() % room->gridPosition.size()];
                if (doorsPlaced == 2) break;
            }
            else {
                continue;
            }
        }

        currentPos = nextPos;
    }

    // Step 3: Convert adjacent black pixels into walls (INCLUDING diagonal directions)
    for (const sf::Vector2i& pixel : room->gridPosition) {
        for (const auto& direction : std::vector<sf::Vector2i>{
                {1, 0}, {-1, 0}, {0, 1}, {0, -1},  // Cardinal directions
                {1, 1}, {-1, -1}, {1, -1}, {-1, 1} // Diagonal directions
            }) {
            sf::Vector2i neighbor = pixel + direction;
            if (image.getPixel(sf::Vector2u(neighbor.x, neighbor.y)) == black) {
                image.setPixel(sf::Vector2u(neighbor.x, neighbor.y), red);
            }
        }
    }
}

void Dungeon::deadEndsToCaves(sf::Image& image) {
    std::vector<Room*> toConvert;

    for (auto& room : validRooms) {
        if (room->exits.size() < 4) {
            std::cout << "Room has less than 2 exits, converting to cave.\n";
            toConvert.push_back(room.get());
        }
    }

    for (Room* room : toConvert) {
        convertToCaveAndDrunkardsWalk(image, room);
    }
}

void Dungeon::placeDoors(sf::Image& image) {
    sf::Color red(255, 0, 0);
    sf::Color brown(139, 69, 19);

    std::unordered_map<Room*, std::unordered_set<Room*>> placedDoors;

    for (auto& room : validRooms) {
        for (auto& otherRoom : validRooms) {
            if (room.get() == otherRoom.get()) continue;

            bool adjacent = false;
            sf::Vector2i doorPosition1, doorPosition2;

            for (const sf::Vector2i& pixel : room->gridPosition) {
                for (const sf::Vector2i& otherPixel : otherRoom->gridPosition) {
                    if ((std::abs(pixel.x - otherPixel.x) == 3 && pixel.y == otherPixel.y)) {
                        doorPosition1 = { (pixel.x + otherPixel.x) / 2, pixel.y };
                        doorPosition2 = { doorPosition1.x + 1, doorPosition1.y };
                        adjacent = true;
                    }
                    else if ((std::abs(pixel.y - otherPixel.y) == 3 && pixel.x == otherPixel.x)) {
                        doorPosition1 = { pixel.x, (pixel.y + otherPixel.y) / 2 };
                        doorPosition2 = { doorPosition1.x, doorPosition1.y + 1 };
                        adjacent = true;
                    }
                    if (adjacent) break;
                }
                if (adjacent) break;
            }

            if (adjacent) {
                image.setPixel(sf::Vector2u(doorPosition1.x, doorPosition1.y), brown);
                image.setPixel(sf::Vector2u(doorPosition2.x, doorPosition2.y), brown);

                room->exits.push_back(new sf::Vector2i(doorPosition1));
                room->exits.push_back(new sf::Vector2i(doorPosition2));
                otherRoom->exits.push_back(new sf::Vector2i(doorPosition1));
                otherRoom->exits.push_back(new sf::Vector2i(doorPosition2));

                placedDoors[room.get()].insert(otherRoom.get());
                placedDoors[otherRoom.get()].insert(room.get());

                room->connected = true;
                otherRoom->connected = true;
            }
        }
    }
    deadEndsToCaves(image);
}

void Dungeon::classifyRooms() {
    if (validRooms.size() < 2) return;

    startRoom->type = RoomType::Start;
    endRoom->type = RoomType::End;

    // Shuffle rooms to randomize selection
    std::shuffle(validRooms.begin(), validRooms.end(), std::mt19937(std::random_device{}()));

    // Determine proportions of room types
    size_t neutralCount = validRooms.size() * 0.6;
    size_t hazardousCount = validRooms.size() * 0.1;
    size_t beneficialCount = validRooms.size() * 0.3;

    size_t index = 0;

    // Assign room types
    for (auto& room : validRooms) {
        if (room.get() == startRoom || room.get() == endRoom) continue;

        if (index < neutralCount) {
            room->type = RoomType::Neutral;
        }
        else if (index < neutralCount + hazardousCount) {
            room->type = RoomType::Hazardous;
        }
        else {
            room->type = RoomType::Beneficial;
        }
        index++;
    }
}

// Places audio in each room
void Dungeon::popRoomAudio(sf::Vector2f dungeonScale) {
    std::map<RoomType, std::vector<RoomSubType>> roomSubTypes = {
    {RoomType::Beneficial, {RoomSubType::b_Treasure, RoomSubType::b_Rest, RoomSubType::b_Garden}},
    {RoomType::Hazardous, {RoomSubType::h_GasRoom, RoomSubType::h_FlamingRoom}},
    {RoomType::Cave, {RoomSubType::c_EchoCave}},
    {RoomType::Start, {RoomSubType::v_startRoom}},
    {RoomType::End, {RoomSubType::v_endRoom}}
    };

    audioRooms.clear();
    for (auto& room : validRooms) {
        if (room->type == RoomType::Start || room->type == RoomType::End || room->type != RoomType::Neutral) {
            audioRooms.push_back(room.get());
        }
    }

    std::cout << "There are: " <<audioRooms.size() << " audio rooms. " << std::endl;

    float volume;
    float attenuation;


    for (Room* room : audioRooms) {
        // If it isn't neutral
        if (roomSubTypes.find(room->type) != roomSubTypes.end()) {  

            const auto& subTypes = roomSubTypes[room->type];

            // Assign it random subtype
            room->subType = subTypes[rand() % subTypes.size()];
            switch (room->subType)
            {
            case RoomSubType::b_Garden:
                room->soundRatio = 6;
                volume = 30;
                attenuation = 4;
                break;

            case RoomSubType::b_Rest:
                room->soundRatio = 40;
                volume = 100;
                attenuation = 10;
                break;
            case RoomSubType::b_Treasure:
                room->soundRatio = 10;
                volume = 100;
                attenuation = 8;
                break;
            case RoomSubType::h_FlamingRoom:
                room->soundRatio = 7;
                volume = 100;
                attenuation = 17;
                break;
            case RoomSubType::h_GasRoom:
                room->soundRatio = 6;
                volume = 150;
                attenuation = 2;
                break;
            case RoomSubType::c_EchoCave:
                room->soundRatio = 10;
                volume = 50;
                attenuation = 25;
                break;
            case RoomSubType::v_endRoom:
                room->soundRatio = 100;
                volume = 100;
                attenuation = 3;
                break;
            case RoomSubType::v_startRoom:
                room->soundRatio = 100;
                volume = 100;
                attenuation = 4;
                break;
            default:
                break;
            }

        }
        int numSounds = std::max(1, static_cast<int>(room->gridPosition.size() / room->soundRatio));
        std::cout << "Room at [" << room->gridPosition[0].x << ", " << room->gridPosition[0].y << "] gets "
            << numSounds << " sounds. Subtype: " << static_cast<int>(room->subType) << std::endl;
        std::vector<sf::Vector2i> availablePositions = room->gridPosition;
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(availablePositions.begin(), availablePositions.end(), g);
        for (int i = 0; i < numSounds; ++i) {
            sf::Vector2i soundPosition = availablePositions[i % availablePositions.size()];
            if (roomSounds[room->subType].empty()) {
                std::cerr << "Error: No sounds available for room subtype!" << std::endl;
                return;
            }
            std::string selectedSound = roomSounds[room->subType][rand() % roomSounds[room->subType].size()];
            auto buffer = std::make_unique<sf::SoundBuffer>();
            if (buffer->loadFromFile("../Sound_Dungeon/audio/" + selectedSound)) {
                room->soundBuffers.push_back(std::move(buffer)); 
                auto sound = std::make_unique<sf::Sound>((*room->soundBuffers.back()));  
                sf::Vector3f scaledPosition(
                    soundPosition.x * dungeonScale.x,  
                    0.0f,
                    soundPosition.y * dungeonScale.y   
                );
                sound->setPosition(scaledPosition);
                sound->setLooping(true);
                sound->setVolume(volume);
                sound->setAttenuation(attenuation);
                sound->setMinDistance(10);
                room->activeSounds.push_back(std::move(sound));             
            }
            else {
                std::cerr << "Failed to load sound: " << selectedSound << std::endl;
            }
        }
        std::cerr << "Active sounds count in room: " << room->activeSounds.size() << std::endl;
    }
}

// Begins all audio in all rooms
void Dungeon::startUpAudio() {
    for (Room* room : audioRooms)
    {
        for (const auto& sound : room->activeSounds)
        {
            sound->play();
        }
    }
}

// Used when CullDisconnected fails
void Dungeon::regenerateDungeon(sf::Image& image)
{
    validRooms.clear();
    generateDungeon(image); 
}