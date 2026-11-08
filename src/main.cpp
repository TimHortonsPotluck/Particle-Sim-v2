#include <vector>
#include <array>
#include <chrono>
#include <SFML/Graphics.hpp>


#include "Solver.h"
#include "Particle.h"
#include "Renderer.h"
#include "Print.h"
#include "Random.h"

using namespace Printing;

void fillWithParticlesSquare(Solver& solver, int num, double radius) {
	// assumes square, using the smaller of the two dimentions, then removes wall_spacing from each side to account for walls
	const double wall_spacing = radius * 1.5;
	const double len = std::min(solver.getWindowSize().x, solver.getWindowSize().y) - 2 * wall_spacing;
	const double particles_per_side = std::sqrt(num);
	const double space_per_particle = len / particles_per_side;

	for (int i = 0; i < num; i++) {
		solver.addParticle(
			sf::Vector2<double>{
			wall_spacing + space_per_particle * std::fmod(i, particles_per_side), 
				wall_spacing + space_per_particle * std::floor(i / particles_per_side)},
			0, radius);
	}
}

void fillWithParticlesFull(Solver& solver, int num, double radius) {
	// this seems to work fine
	const double wall_spacing = radius * 1.5;
	const double len_x = solver.getWindowSize().x - 2 * wall_spacing;
	const double len_y = solver.getWindowSize().y - 2 * wall_spacing;
	const double area_per_particle = len_x * len_y / num;
	const double space_per_particle = std::sqrt(area_per_particle);
	const double particles_per_row = len_x / space_per_particle;
	//println(space_per_particle);
	for (int i = 0; i < num; i++) {
		solver.addParticle(
			sf::Vector2<double>{
			wall_spacing + space_per_particle * std::fmod(i, particles_per_row),
				wall_spacing + space_per_particle * std::floor(i / particles_per_row)},
			0, radius);
	}
}

void makeParticleTriagularPacking(Solver& solver, double x, double y, int num, double spacing, double radius, double angle) {
	const int particles_per_side = static_cast<int>(std::sqrt(num));
	int row = 0;
	for (int i = 0; i < num; i++) {
		row = static_cast<int>(std::floor(i / particles_per_side));
		const sf::Vector2<double> raw_pos{(row % 2) * .5 * spacing + spacing * (i % particles_per_side),
				spacing * .5 * std::sqrt(3) * std::floor(i / particles_per_side) };
		solver.addParticle(
			raw_pos.rotatedBy(sf::degrees(angle)) + sf::Vector2<double>{x, y},
			0, radius);
	}
}

int main() {

	constexpr int width{ 800 };
	constexpr int height{ 800 };

	sf::ContextSettings settings;
	settings.antiAliasingLevel = 1;

	sf::RenderWindow window( sf::VideoMode( { width, height } ), "Better Particles", sf::Style::Default, sf::State::Windowed, settings);

	const double sim_dt = .01;
	const double particle_radius = 5; // just for drawing purposes, probably want same as lj_r0
	const double lj_r0 = 5;
	const double lj_depth = 2;
	const double lj_cutoff = 17; // check solver.h for more on these

	Solver solver{ window.getSize(), lj_cutoff }; // the grid size should be at least the LJ cutoff
	solver.setSimDT(sim_dt);
	solver.setLJR0(lj_r0);
	solver.setLJDepth(lj_depth);
	solver.setLJCutoff(lj_cutoff);
	solver.setLJWallFriction(0);
	solver.setLinearDamping(.001);

	Renderer renderer{ window };

	//solver.addParticle(sf::Vector2<double>{100, 100}, 0, particle_radius);
	//solver.addParticle(sf::Vector2<double>{120, 100}, 0, particle_radius);
	//solver.addParticle(sf::Vector2<double>{150, 100}, 0, particle_radius);
	//solver.addParticle(sf::Vector2<double>{200, 100}, 0, particle_radius);
	//solver.setPartVel(0, {10, 0});
	//for (int i = 0; i < 20; i++) { // square
	//	for (int j = 0; j < 20; j++) {
	//		solver.addParticle(sf::Vector2<double>{width * .5 - 75.*2 + 15. * i, height * .5 - 75.*2 + 15. * j}, 0, 10);
	//	}
	//}
	//fillWithParticlesSquare(solver, 10000, particle_radius);
	fillWithParticlesFull(solver, 10000, particle_radius);
	//makeParticleTriagularPacking(solver, 10, 10, 10000, 5, particle_radius, 0);
	//makeParticleTriagularPacking(solver, 50, 50, 500, 10, particle_radius, 0);
	//makeParticleTriagularPacking(solver, 420, 50, 500, 10, particle_radius, 45);
	//makeParticleTriagularPacking(solver, 350, 270, 500, 10, particle_radius, 95);
	solver.randomizeParticleVels(5);

	auto start_frame_time = std::chrono::high_resolution_clock::now();
	auto end_frame_time = std::chrono::high_resolution_clock::now();
	int frame{ 0 };
	int frame_avg{ 100 };
	double milli_accu{ 0 };

	int render_every = 10; // num frames

	while ( window.isOpen() ) {
		start_frame_time = std::chrono::high_resolution_clock::now();
		while ( const std::optional event = window.pollEvent() ) {
			if ( event->is<sf::Event::Closed>() )
				window.close();
			if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseButtonPressed->button == sf::Mouse::Button::Right) {
					sf::Vector2<double> localPosition = static_cast<sf::Vector2<double>>(sf::Mouse::getPosition(window));
					solver.addParticle(localPosition, 0, particle_radius);
					solver.setPartVel(solver.getNumParticles()-1, { 50, 0 });
				}
				if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
					sf::Vector2<double> localPosition = static_cast<sf::Vector2<double>>(sf::Mouse::getPosition(window));
					solver.addParticle(localPosition, 0, particle_radius);
				}
			}
		}

		solver.update();

		if (frame % render_every == 0) {
			window.clear();
			renderer.render(solver);
			window.display();
		}

		end_frame_time = std::chrono::high_resolution_clock::now();
		double millis = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end_frame_time - start_frame_time).count()) / 1000.;
		milli_accu += millis;
		frame++;
		if (frame % frame_avg == 0) {
			println("avg millis:", milli_accu / frame_avg);
			milli_accu = 0;
			//println("mean vel sq:", solver.getTotalVelSq() / solver.getNumParticles());
			//println("avg closest dist:", solver.getAvgClosestDist(1));
		}
		//println(solver.getPartVel(0));
		
		
	}

	return 0;
}
