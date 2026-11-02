#include "FixedGrid.h"
#include "Print.h"

FixedGrid::FixedGrid(sf::Vector2u world_size, double cell_size) : cell_size{ cell_size }  {
	cols = static_cast<int>(std::ceil(static_cast<double>(world_size.x) / cell_size)); 
	rows = static_cast<int>(std::ceil(static_cast<double>(world_size.y) / cell_size));
	cells.resize(cols * rows); 
}

void FixedGrid::clear() {
	for (auto& cell : cells) {
		cell.clear();
	}
}

void FixedGrid::insert(size_t p_index, const sf::Vector2<double>& pos) {
	int col = static_cast<int>(pos.x / cell_size);
	int row = static_cast<int>(pos.y / cell_size);
	if (!isValidCell(col, row)) {
		Printing::println("not a valid cell");
		return;
	}
	cells[getCellIndex(col, row)].push_back(p_index);
}

const std::vector<size_t>& FixedGrid::getCellContent(int x, int y) const {
	if (!isValidCell(x, y)) {
		Printing::println("not a valid row and column");
		return std::vector<size_t>{};
	}
	return cells[getCellIndex(x, y)];
}

const std::vector<size_t>& FixedGrid::getCellContent(int index) const {
	if (index < 0 || index > cols * rows) {
		Printing::println("not a valid index");
		return std::vector<size_t>{};
	}
	return cells[index];
}

std::vector<size_t> FixedGrid::getNearbyParticles(const sf::Vector2<double>& pos) const {
	std::vector<std::size_t> contents;
	int col = static_cast<int>(pos.x / cell_size);
	int row = static_cast<int>(pos.y / cell_size);
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			int cell_x = i + col;
			int cell_y = j + row;
			if (!isValidCell(cell_x, cell_y)) {
				continue;
			}
			const auto& cell = cells[getCellIndex(cell_x, cell_y)];
			contents.insert(contents.end(), cell.begin(), cell.end());
		}
	}
	return contents;
}

int FixedGrid::getIndexAtPos(const sf::Vector2<double>& pos) const {
	int col = static_cast<int>(pos.x / cell_size);
	int row = static_cast<int>(pos.y / cell_size);
	return getCellIndex(col, row);
}

sf::Vector2u FixedGrid::getGridLocAtPos(const sf::Vector2<double>& pos) const {
	int col = static_cast<int>(pos.x / cell_size);
	int row = static_cast<int>(pos.y / cell_size);
	return sf::Vector2u(col, row);
}

int FixedGrid::getCellIndex(int x, int y) const {
	return y * rows + x;
}

bool FixedGrid::isValidCell(int x, int y) const {
	return (x >= 0 && x < cols && y >= 0 && y < rows);
}

sf::Vector2u FixedGrid::getGridSize() const {
	return sf::Vector2u(cols, rows);
}
