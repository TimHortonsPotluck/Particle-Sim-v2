#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Particle.h"


class FixedGrid {

public:
	FixedGrid(sf::Vector2u world_size, double cell_size);

	void clear();
	void insert(size_t p_index, const sf::Vector2<double>& pos);
	const std::vector<size_t>& getCellContent(int x, int y) const;
	const std::vector<size_t>& getCellContent(int index) const;
	std::vector<size_t> getNearbyParticles(const sf::Vector2<double>& pos) const;
	int getIndexAtPos(const sf::Vector2<double>& pos) const;
	sf::Vector2u getGridLocAtPos(const sf::Vector2<double>& pos) const;
	bool isValidCell(int x, int y) const;
	sf::Vector2u getGridSize() const;

private:
	int cols;
	int rows;
	double cell_size;

	std::vector<std::vector<size_t>> cells;

	int getCellIndex(int x, int y) const;
	

};