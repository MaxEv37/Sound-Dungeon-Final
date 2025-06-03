#include "../include/game.hpp"

int main() 
{
	const int WIDTH = 800;
	const int HEIGHT = 600;

	sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "Dungeon Renderer");

	Game game(window);	

	game.run(false);

	return 0;
}