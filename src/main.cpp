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

void fillWithParticles(Solver& solver, int num) {
	// assumes square, using the smaller of the two dimentions, then removes wall_spacing from each side to account for walls
	const double wall_spacing = 25;
	const double len = std::min(solver.getWindowSize().x, solver.getWindowSize().y) - 2 * wall_spacing;
	const double particles_per_side = std::sqrt(num);
	const double space_per_particle = len / particles_per_side;

	for (int i = 0; i < num; i++) {
		solver.addParticle(
			sf::Vector2<double>{
			wall_spacing + space_per_particle * .5 + space_per_particle * std::fmod(i, particles_per_side), 
				wall_spacing + space_per_particle * .5 + space_per_particle * std::floor(i / particles_per_side)},
			0, 10);
	}
}

int main() {

	constexpr int width{ 800 };
	constexpr int height{ 800 };

	sf::ContextSettings settings;
	settings.antiAliasingLevel = 1;

	sf::RenderWindow window( sf::VideoMode( { width, height } ), "Better Particles", sf::Style::Default, sf::State::Windowed, settings);

	constexpr int framerate = 60;
	const int sub_steps{ 1 };

	Solver solver{ window.getSize(), 33. };
	solver.setSimUpdateRate(framerate);
	solver.setSubStepsCount(sub_steps);

	Renderer renderer{ window };

	//solver.addParticle(sf::Vector2<double>{750, 100}, 0, 10);
	//solver.setPartVel(0, {10, 0});
	//for (int i = 0; i < 20; i++) { // square
	//	for (int j = 0; j < 20; j++) {
	//		solver.addParticle(sf::Vector2<double>{width * .5 - 75.*2 + 15. * i, height * .5 - 75.*2 + 15. * j}, 0, 10);
	//	}
	//}
	fillWithParticles(solver, 2000);
	solver.randomizeParticleVels(5);

	auto start_frame_time = std::chrono::high_resolution_clock::now();
	auto end_frame_time = std::chrono::high_resolution_clock::now();
	int frame{ 0 };
	int frame_avg{ 10 };
	double milli_accu{ 0 };
	while ( window.isOpen() ) {
		start_frame_time = std::chrono::high_resolution_clock::now();
		while ( const std::optional event = window.pollEvent() ) {
			if ( event->is<sf::Event::Closed>() )
				window.close();
			if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
					sf::Vector2<double> localPosition = static_cast<sf::Vector2<double>>(sf::Mouse::getPosition(window));
					solver.addParticle(localPosition, 0, 10);
				}
			}
		}

		solver.update();
		window.clear();
		renderer.render(solver);
		window.display();

		end_frame_time = std::chrono::high_resolution_clock::now();
		double millis = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end_frame_time - start_frame_time).count()) / 1000.;
		milli_accu += millis;
		frame++;
		if (frame % frame_avg == 0) {
			println(milli_accu / frame_avg);
			milli_accu = 0;
			//println(solver.getTotalVelSq()/solver.getNumParticles());
		}
		//println(solver.getPartVel(0));
		
	}

	return 0;
}
