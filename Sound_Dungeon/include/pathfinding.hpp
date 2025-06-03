#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "room.hpp"

class Pathfinding {
public:
    // **Find the closest room's center to a target position**
    static sf::Vector2i findClosestRoom(const sf::Vector2i& target, const std::vector<std::unique_ptr<Room>>& validRooms);

    // **Find path using A* algorithm**
    static std::vector<sf::Vector2i> findPath(sf::Image& image, const sf::Vector2i& start, const sf::Vector2i& goal);

private:
    // **Heuristic function (Manhattan distance)**
    static int manhattanDistance(const sf::Vector2i& a, const sf::Vector2i& b);

    // **Node structure for A* pathfinding**
    struct Node {
        sf::Vector2i position;
        int gCost; // Cost from start
        int hCost; // Estimated cost to target (heuristic)
        int fCost() const { return gCost + hCost; }
        Node* parent;

        Node(sf::Vector2i pos, int g, int h, Node* p = nullptr)
            : position(pos), gCost(g), hCost(h), parent(p) {
        }
    };
};