#include "Print.h"

namespace Printing {

	void println(const int x) {
		std::cout << x << std::endl;
	}

	void println(const size_t x) {
		std::cout << x << std::endl;
	}

	void println(const float x) {
		std::cout << x << std::endl;
	}

	void println(const double x) {
		std::cout << x << std::endl;
	}

	void println(const std::string& x) {
		std::cout << x << std::endl;
	}

	void println(const std::string& x, const double y) {
		std::cout << x << " " << y << std::endl;
	}

	void println(const std::vector<std::vector<double>>& vec) {
		for (size_t i{ 0 }; i < vec.size(); ++i) {
			for (size_t j{ 0 }; j < vec[0].size() - 1; ++j) {
				std::cout << vec[i][j] << ", ";
			}
			std::cout << vec[i][vec[0].size() - 1] << std::endl;
		}
	}

	void println(const std::vector<double>& vec, int num_rows, int num_cols) {
		for (size_t i{ 0 }; i < num_rows; ++i) {
			for (size_t j{ 0 }; j < num_cols - 1; ++j) {
				//println(i, j);
				std::cout << vec[i * num_cols + j] << ", ";
			} // (numrows - 1) * numcols + num_cols - 2
			std::cout << vec[i * num_cols + num_cols - 1] << std::endl;
		}
	}

	void println(const sf::Vector2<double> v) {
		std::cout << v.x << ", " << v.y << std::endl;
	}

}