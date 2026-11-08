#include <cmath>
#include "Renderer.h"

Renderer::Renderer(sf::RenderTarget& target) : render_target{ target } {
	window_size = static_cast<sf::Vector2f>(render_target.getSize());
}

void Renderer::render(const Solver& solver) const {
	sf::CircleShape circle(1.f);
	circle.setPointCount(8);
	circle.setOrigin({ 1.f, 1.f });
	
	for (const Particle& p : solver.getParticles()) {
		//const double vel = std::sqrt(p.getVelMagSq(solver.getDT()));
		//const int vel_color = static_cast<int>(std::max(0, 255 - static_cast<int>(vel * (255. / 5.))));
		//circle.setFillColor(sf::Color::Color(255, vel_color, vel_color, 255));
		circle.setFillColor(p.color);
		circle.setPosition(static_cast<sf::Vector2f>(p.getPos()));
		circle.setScale({static_cast<float>(p.radius) * .5f, static_cast<float>(p.radius) * .5f }); // .5 so it's the radius
		render_target.draw(circle);
	}
}

