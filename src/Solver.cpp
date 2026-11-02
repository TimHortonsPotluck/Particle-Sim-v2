#include "Solver.h"

#include "Print.h"

Solver::Solver(sf::Vector2u window_size, double grid_size) : window_size{ window_size }, fg{ window_size, grid_size } {}

void Solver::addParticle(sf::Vector2<double> pos, double angle, double radius) {
	particles.emplace_back(pos, angle, radius);
	//Printing::println(pos);
}

void Solver::update() {
	time += frame_dt;
	const double step_dt = getStepDT();
	for (int i = 0; i < sub_steps; i++) {
		fg.clear();
		for (size_t i{ 0 }; i < particles.size(); ++i) {
			fg.insert(i, particles[i].getPos());
		}
		addLJAcc();
		AddLJWallRepellant(0);
		updateObjects(step_dt);
	}
}

void Solver::setSimUpdateRate(int rate) {
	if (rate == 0) {
		Printing::println("You can't do that");
		rate = 1;
	}
	frame_dt = 1 / static_cast<double>(rate);
}

void Solver::setSubStepsCount(int sub_steps) {
	this->sub_steps = sub_steps;
}

void Solver::setPartVel(size_t index, sf::Vector2<double> v) {
	particles[index].setVel(v, getStepDT());
}

const sf::Vector2<double>& Solver::getPartVel(size_t index) {
	return particles[index].getVel(getStepDT());
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

double Solver::getStepDT() const {
	return frame_dt / static_cast<double>(sub_steps);
}

size_t Solver::getNumParticles() const {
	return particles.size();
}

double Solver::getTotalVelSq() const {
	double sum = 0;
	for (const Particle& p : particles) {
		sum += p.getVel(getStepDT()).lengthSquared();
	}
	return sum;
}

void Solver::addGravity() {
	for (Particle& p : particles) {
		p.addAcc(gravity);
	}
}

void Solver::addLJAcc() {
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
					Particle& p2 = particles[j];

					const sf::Vector2<double> disp{ p1Pos - p2.getPos() };
					const double dist_sq{ disp.lengthSquared() };
					if (dist_sq < 1e-12) { // checking if too close
						continue;
					}
					if (dist_sq > lj_cutoff * lj_cutoff) { // don't do anything if these particles are too far apart
						continue;
					}

					//force mag looks like 
					// (12 * lj_depth / dist) * ((lj_r0 / dist)^12 - (lj_r0 / dist)^6)
					// and with direction like
					// (12 * lj_depth * disp / dist_sq) * ((lj_r0 / dist)^12 - (lj_r0 / dist)^6)

					const double ratio_2{ lj_r0 * lj_r0 / dist_sq };
					const double ratio_6{ ratio_2 * ratio_2 * ratio_2 };
					const double ratio_12{ ratio_6 * ratio_6 };

					const sf::Vector2<double> force{ (12. * lj_depth) * (ratio_12 - ratio_6) * (disp / dist_sq) };
					p1.addAcc(force); // assuming masses are 1 for now
					p2.addAcc(-force);
				}
			}
		}
	}
}

void Solver::AddLJWallRepellant(double friction_coeff) {
	// only check particles close to wall if possible
	// can do particles whose grid position has a 0 for grid or column in order to get the top row and left column, but other sides are harder
	// can check the last 2 cells to the bottom and right?
	// then do LJ force but with r0/2 and no attractive component
	sf::Vector2u grid_size = fg.getGridSize();

	// lambda function for the force. just needs to be inserted as the x or y commponent of a vector
	// wall_pos is either the x or y position of the wall, p_pos is either x or y position of particle
	// both should be either x or y, can't be mixed
	auto calcForceMag = [&](double p_pos, double wall_pos, double p_vel) {
		const double dist_sq{ (p_pos - wall_pos) * (p_pos - wall_pos) };
		if (dist_sq > lj_cutoff * lj_cutoff) {
			return 0.;
		}
		//force mag looks like 
		// (12 * lj_depth / dist) * (lj_r0 * .5 / dist)^12
		// and with direction like
		// (12 * lj_depth * disp / dist_sq) * (lj_r0 * .5 / dist)^12)
		const double ratio_2{ lj_r0 * lj_r0 * .25 / dist_sq };
		const double ratio_6{ ratio_2 * ratio_2 * ratio_2 };
		const double ratio_12{ ratio_6 * ratio_6 };
		return ratio_12 * ((12. * lj_depth) * ((p_pos - wall_pos) / dist_sq) - p_vel * friction_coeff); // force mag
	};

	auto applyToBoundaryCell = [&](int col, int row, bool force_dir_x, double wall_pos) {
		const auto& cell = fg.getCellContent(col, row);
		for (size_t i : cell) {
			Particle& p = particles[i];
			if (force_dir_x) {
				p.addAcc({ calcForceMag(p.getPos().x, wall_pos, p.getVel(getStepDT()).x), 0});
			} else {
				p.addAcc({ 0, calcForceMag(p.getPos().y, wall_pos, p.getVel(getStepDT()).y) });
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
		p.setVel({mag * std::cos(a), mag * std::sin(a)}, getStepDT());
	}
}


