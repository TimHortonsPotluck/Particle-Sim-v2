#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Solver.h"

class Renderer {
public:
	
	Renderer(sf::RenderTarget& target);

	void render(const Solver& solver) const;

private:
	sf::Vector2f window_size;
	sf::RenderTarget& render_target;
};