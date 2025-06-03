#include "../include/pathfinding.hpp"
#include <algorithm>
#include <iostream>

sf::Vector2i Pathfinding::findClosestRoom(const sf::Vector2i& target, const std::vector<std::unique_ptr<Room>>& validRooms) {
    sf::Vector2i closestCenter;
    int closestDistance = std::numeric_limits<int>::max();
    for (const auto& room : validRooms) {
        sf::Vector2i center(0, 0);
        for (const sf::Vector2i& pixel : room->gridPosition) {
            center += pixel;
        }
        center.x /= static_cast<int>(room->gridPosition.size());
        center.y /= static_cast<int>(room->gridPosition.size());
        int distance = manhattanDistance(center, target);
        if (distance < closestDistance) {
            closestDistance = distance;
            closestCenter = center;
        }
    }
    return closestCenter;
}

// **Manhattan Distance Heuristic**
int Pathfinding::manhattanDistance(const sf::Vector2i& a, const sf::Vector2i& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

// **A* Pathfinding Algorithm**
std::vector<sf::Vector2i> Pathfinding::findPath(sf::Image& image, const sf::Vector2i& start, const sf::Vector2i& goal) {
    std::vector<sf::Vector2i> path;
    std::vector<Node*> openSet;
    std::vector<Node*> closedSet;
    std::vector<sf::Vector2i> visitedPositions;
    openSet.push_back(new Node(start, 0, manhattanDistance(start, goal)));
    sf::Vector2u imageSize = image.getSize();  //  Get image size for bounds checking
    while (!openSet.empty()) {
        std::sort(openSet.begin(), openSet.end(), [](Node* a, Node* b) { return a->fCost() < b->fCost(); });
        Node* current = openSet.front();
        openSet.erase(openSet.begin());
        if (std::find(visitedPositions.begin(), visitedPositions.end(), current->position) != visitedPositions.end()) {
            continue;
        }
        visitedPositions.push_back(current->position);
        closedSet.push_back(current);
        if (current->position == goal) {
            while (current) {
                path.push_back(current->position);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            for (Node* node : openSet) delete node;
            for (Node* node : closedSet) delete node;
            return path;
        }
        std::vector<sf::Vector2i> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
        for (const sf::Vector2i& direction : directions) {
            sf::Vector2i neighborPos = current->position + direction;
            if (neighborPos.x < 0 || neighborPos.y < 0 || neighborPos.x >= (int)imageSize.x || neighborPos.y >= (int)imageSize.y) {
                continue;
            }
            if (std::find(visitedPositions.begin(), visitedPositions.end(), neighborPos) != visitedPositions.end()) {
                continue;
            }
            sf::Color pixelColor = image.getPixel(sf::Vector2u(neighborPos.x, neighborPos.y));
            if (pixelColor == sf::Color::Black) continue;
            int movementCost = (pixelColor == sf::Color::Red) ? 1000 : 1;
            Node* neighbor = new Node(neighborPos, current->gCost + movementCost, manhattanDistance(neighborPos, goal), current);
            openSet.push_back(neighbor);
        }
    }
    for (Node* node : openSet) delete node;
    for (Node* node : closedSet) delete node;
    return {};
}