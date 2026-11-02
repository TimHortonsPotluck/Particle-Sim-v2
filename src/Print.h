#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>

namespace Printing {

	void println(const int x);

	void println(const size_t x);

	void println(const float x);

	void println(const double x);

	void println(const std::string& x);

	void println(const std::vector<std::vector<double>>& vec);

	void println(const std::vector<double>& vec, int num_rows, int num_cols);

	void println(const sf::Vector2<double> v);

}