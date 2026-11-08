#include <chrono>
#include "Solver.h"

#include "Print.h"

Solver::Solver(sf::Vector2u window_size, double grid_size) : window_size{ window_size }, fg{ window_size, grid_size } {}

void Solver::addParticle(sf::Vector2<double> pos, double angle, double radius) {
	particles.emplace_back(pos, angle, radius);
}

void Solver::update() {
	//auto grid_start = std::chrono::high_resolution_clock::now();
	fg.clear();
	for (size_t i{ 0 }; i < particles.size(); ++i) {
		fg.insert(i, particles[i].getPos());
	}
	//if (frame_count % 10 == 0) { // not doing this right now
	//	fg.clear();
	//	for (size_t i{ 0 }; i < particles.size(); ++i) {
	//		fg.insert(i, particles[i].getPos());
	//	}
	//	buildVerletList();
	//}

	//auto grid_end_LJ_start = std::chrono::high_resolution_clock::now();
	addLJAcc();
	//auto LJ_end_wall_start = std::chrono::high_resolution_clock::now();
	addLJWallRepellant();
	//auto wall_end_damp_start = std::chrono::high_resolution_clock::now();
	addLinearDamping();
	//auto damp_end_update_start = std::chrono::high_resolution_clock::now();
	updateObjects(getDT());
	//auto update_end = std::chrono::high_resolution_clock::now();
	//double grid_time = std::chrono::duration_cast<std::chrono::microseconds>(grid_end_LJ_start - grid_start).count();
	//double LJ_time = std::chrono::duration_cast<std::chrono::microseconds>(LJ_end_wall_start - grid_end_LJ_start).count();
	//double wall_time = std::chrono::duration_cast<std::chrono::microseconds>(wall_end_damp_start - LJ_end_wall_start).count();
	//double damp_time = std::chrono::duration_cast<std::chrono::microseconds>(damp_end_update_start - wall_end_damp_start).count();
	//double update_time = std::chrono::duration_cast<std::chrono::microseconds>(update_end - damp_end_update_start).count();
	//Printing::println("grid time:", grid_time);
	//Printing::println("lj time:", LJ_time);
	//Printing::println("wall time:", wall_time);
	//Printing::println("damp time:", damp_time);
	//Printing::println("update time:", update_time);
	time += dt;
	frame_count++;
}

void Solver::setSimDT(double dt) {
	if (dt == 0) {
		dt = .0001;
	}
	this->dt = dt;
}

void Solver::setPartVel(size_t index, sf::Vector2<double> v) {
	particles[index].setVel(v, dt);
}

const sf::Vector2<double>& Solver::getPartVel(size_t index) {
	return particles[index].getVel(dt);
}

const std::vector<Particle>& Solver::getParticles() const {
	return particles;
}

const sf::Vector2u& Solver::getWindowSize() const {
	return window_size;
}

double Solver::getTime() const {
	return time;
}

double Solver::getDT() const {
	return dt;
}

int Solver::getFrameCount() const {
	return frame_count;
}

size_t Solver::getNumParticles() const {
	return particles.size();
}

double Solver::getTotalVelSq() const {
	double sum = 0;
	for (const Particle& p : particles) {
		sum += p.getVel(dt).lengthSquared();
	}
	return sum;
}

double Solver::getAvgClosestDist(int grid_cells) const { // only checks cells in the fixed grid from -grid_cells to +grid_cells in both x and y
	double sum_of_closests = 0;
	for (size_t i{ 0 }; i < particles.size(); ++i) {
		const Particle& p1 = particles[i];
		const sf::Vector2<double> p1Pos = p1.getPos();
		double closest_dist_sq = window_size.lengthSquared(); // certainly smaller than this
		sf::Vector2u grid_pos = fg.getGridLocAtPos(p1Pos);
		for (int dx = -grid_cells; dx <= grid_cells; dx++) { // search 
			for (int dy = -grid_cells; dy <= grid_cells; dy++) {
				if (!fg.isValidCell(dx + grid_pos.x, dy + grid_pos.y)) {
					continue;
				}
				const auto& cell = fg.getCellContent(dx + grid_pos.x, dy + grid_pos.y);
				for (size_t j : cell) {
					if (j == i) {
						continue;
					}
					const Particle& p2 = particles[j];

					const sf::Vector2<double> disp{ p1Pos - p2.getPos() };
					const double dist_sq{ disp.lengthSquared() };
					if (dist_sq < closest_dist_sq) {
						closest_dist_sq = dist_sq;
					}
				}
			}
		}
		sum_of_closests += std::sqrt(closest_dist_sq);
	}
	return sum_of_closests / particles.size();
}

void Solver::addGravity() {
	for (Particle& p : particles) {
		p.addAcc(gravity);
	}
}

void Solver::addLJAcc() {
	//for (size_t i{ 0 }; i < particles.size(); ++i) { // this is for the verlet list calculation. maybe not doing this for now
	//	Particle& p1 = particles[i];
	//	const sf::Vector2<double> p1Pos = p1.getPos();
	//	for (size_t j : verlet_list[i]) {
	//		Particle& p2 = particles[j];
	//		const sf::Vector2<double> disp{ p1Pos - p2.getPos() };
	//		const double dist_sq{ disp.lengthSquared() };
	//		if (dist_sq < 1e-12) { // checking if too close
	//			continue;
	//		}
	//		if (dist_sq > lj_cutoff_sq) { // don't do anything if these particles are too far apart
	//			continue;
	//		}
	//		//++interactions;
	//		const double inv_dist_sq{ 1 / dist_sq };
	//		//force mag looks like 
	//		// (12 * lj_depth / dist) * ((lj_r0 / dist)^12 - (lj_r0 / dist)^6)
	//		// and with direction like
	//		// (12 * lj_depth * disp / dist_sq) * ((lj_r0 / dist)^12 - (lj_r0 / dist)^6)
	//		const double ratio_2{ lj_r0_sq * inv_dist_sq };
	//		const double ratio_6{ ratio_2 * ratio_2 * ratio_2 };
	//		const double ratio_12{ ratio_6 * ratio_6 };
	//		const double scale{ lj_depth_12 * (ratio_12 - ratio_6) * inv_dist_sq };
	//		p1.addAcc(scale * disp); // assuming masses are 1 for now
	//		p2.addAcc(-scale * disp);
	//		//auto end = std::chrono::high_resolution_clock::now();
	//		//time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
	//	}
	//}
	
	//size_t attempted_interactions{ 0 };
	//size_t interactions{ 0 };
	for (size_t i{ 0 }; i < particles.size(); ++i) {
		Particle& p1 = particles[i];
		const sf::Vector2<double> p1Pos = p1.getPos();
		sf::Vector2u grid_pos = fg.getGridLocAtPos(p1Pos);
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (!fg.isValidCell(dx + grid_pos.x, dy + grid_pos.y)) {
					continue;
				}
				const auto& cell = fg.getCellContent(dx + grid_pos.x, dy + grid_pos.y);
				for (size_t j : cell) {
					if (j <= i) {
						continue;
					}
					//auto start = std::chrono::high_resolution_clock::now();
					Particle& p2 = particles[j];
					//++attempted_interactions;
					const sf::Vector2<double> disp{ p1Pos - p2.getPos() };
					const double dist_sq{ disp.lengthSquared() };
					if (dist_sq < 1e-12) { // checking if too close
						continue;
					}
					if (dist_sq > lj_cutoff_sq) { // don't do anything if these particles are too far apart
						continue;
					}
					//++interactions;
					const double inv_dist_sq{ 1 / dist_sq };

					//force mag looks like 
					// (12 * lj_depth / dist) * ((lj_r0 / dist)^12 - (lj_r0 / dist)^6)
					// and with direction like
					// (12 * lj_depth * disp / dist_sq) * ((lj_r0 / dist)^12 - (lj_r0 / dist)^6)

					const double ratio_2{ lj_r0_sq * inv_dist_sq };
					const double ratio_6{ ratio_2 * ratio_2 * ratio_2 };
					const double ratio_12{ ratio_6 * ratio_6 };

					const double scale{ lj_depth_12 * (ratio_12 - ratio_6) * inv_dist_sq };

					p1.addAcc(scale * disp); // assuming masses are 1 for now
					p2.addAcc(-scale * disp);
					//auto end = std::chrono::high_resolution_clock::now();
					//time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
				}
			}
		}
	}
	//Printing::println("attempted interactions:", attempted_interactions);
	//Printing::println("interactions:", interactions);
	//Printing::println(time / 1000000);
}

void Solver::addLinearDamping() { // linear
	for (Particle& p : particles) {
		p.addAcc(-p.getVel(dt) * lin_damp_friction);
	}
}

void Solver::addLJWallRepellant() {
	// only check particles close to wall if possible
	// can do particles whose grid position has a 0 for grid or column in order to get the top row and left column, but other sides are harder
	// can check the last 2 cells to the bottom and right?
	// then do LJ force but with no attractive component
	sf::Vector2u grid_size = fg.getGridSize();

	// lambda function for the force. just needs to be inserted as the x or y commponent of a vector
	// wall_pos is either the x or y position of the wall, p_pos is either x or y position of particle
	// both should be either x or y, can't be mixed
	auto calcForceMag = [&](double p_pos, double wall_pos, double p_vel) {
		const double dist_sq{ (p_pos - wall_pos) * (p_pos - wall_pos) };
		if (dist_sq > lj_cutoff_sq) {
			return 0.;
		}
		const double inv_dist_sq{1 / dist_sq};
		//force mag looks like 
		// (12 * lj_depth / dist) * (lj_r0 / dist)^12
		// and with direction like
		// (12 * lj_depth * disp / dist_sq) * (lj_r0 / dist)^12)
		const double ratio_2{ lj_r0_sq * inv_dist_sq };
		const double ratio_6{ ratio_2 * ratio_2 * ratio_2 };
		const double ratio_12{ ratio_6 * ratio_6 };
		return ratio_12 * (lj_depth_12 * ((p_pos - wall_pos) * inv_dist_sq) - p_vel * lj_wall_friction); // force mag
	};

	auto applyToBoundaryCell = [&](int col, int row, bool force_dir_x, double wall_pos) {
		const auto& cell = fg.getCellContent(col, row);
		for (size_t i : cell) {
			Particle& p = particles[i];
			if (force_dir_x) {
				p.addAcc({ calcForceMag(p.getPos().x, wall_pos, p.getVel(dt).x), 0});
			} else {
				p.addAcc({ 0, calcForceMag(p.getPos().y, wall_pos, p.getVel(dt).y) });
			}
		}
	};

	for (int col = 0; col < grid_size.x; col++) {
		applyToBoundaryCell(col, 0, false, 0); // top row
		applyToBoundaryCell(col, grid_size.y - 1, false, window_size.y); // second row from bottom
		applyToBoundaryCell(col, grid_size.y - 2, false, window_size.y); // bottom row
	}

	for (int row = 0; row < grid_size.y; row++) {
		applyToBoundaryCell(0, row, true, 0); // left col
		applyToBoundaryCell(grid_size.x - 1, row, true, window_size.x); // second col from right
		applyToBoundaryCell(grid_size.x - 2, row, true, window_size.x); // right col
	}
}

void Solver::updateObjects(double dt) {
	for (Particle& p : particles) {
		p.update(dt);
	}
}

void Solver::randomizeParticleVels(double max) {
	for (Particle& p : particles) {
		const double a = Random::get(0., 2 * 3.1415926535);
		const double mag = Random::get(0., max);
		p.setVel({mag * std::cos(a), mag * std::sin(a)}, dt);
	}
}

void Solver::setLJR0(double r0) {
	lj_r0 = r0;
	lj_r0_sq = r0 * r0;
}

void Solver::setLJDepth(double depth) {
	lj_depth = depth;
	lj_depth_12 = 12. * depth;
}

void Solver::setLJCutoff(double cutoff) {
	lj_cutoff = cutoff;
	lj_cutoff_sq = cutoff * cutoff;
}

void Solver::setLJWallFriction(double friction) {
	lj_wall_friction = friction;
}

void Solver::setLinearDamping(double friction) {
	lin_damp_friction = friction;
}

void Solver::buildVerletList() {
	verlet_list.clear();
	verlet_list.resize(particles.size());
	const double neighbor_radius = fg.getCellSize() + verlet_skin;
	const double neighbor_radius_sq = neighbor_radius * neighbor_radius;
	const int cell_search_radius = static_cast<int>(std::ceil(neighbor_radius / fg.getCellSize()));

	for (size_t i{ 0 }; i < particles.size(); ++i) {
		const sf::Vector2<double> pos_i = particles[i].getPos();
		const sf::Vector2u grid_pos = fg.getGridLocAtPos(pos_i);
		for (int dx = -cell_search_radius; dx <= cell_search_radius; dx++) {
			for (int dy = -cell_search_radius; dy <= cell_search_radius; dy++) {
				if (!fg.isValidCell(grid_pos.x + dx, grid_pos.y + dy)) {
					continue;
				}

				const std::vector<size_t>& cell = fg.getCellContent(grid_pos.x + dx, grid_pos.y + dy);
				for (size_t j : cell) {
					if (j <= i) {
						continue;
					}
					const sf::Vector2<double> disp{ particles[j].getPos() - pos_i };
					const double dist_sq{ disp.lengthSquared() };
					if (dist_sq < neighbor_radius_sq) {
						verlet_list[i].push_back(j);
					}
				}
			}
		}
	}
}

