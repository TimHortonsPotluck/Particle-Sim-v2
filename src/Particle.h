#pragma once
#include <SFML/Graphics.hpp>


class Particle {
public:

	double radius;
	sf::Color color;

	Particle(sf::Vector2<double> pos, double angle, double radius);

	void update(double dt);

	void addAcc(sf::Vector2<double> a);
	void setVel(sf::Vector2<double> v, double dt);
	void addVel(sf::Vector2<double> v, double dt);
	sf::Vector2<double> getVel(double dt) const;
	void addAngAcc(double a_a);
	void setAngVel(double a_v, double dt);
	void addAngVel(double a_v, double dt);
	double getAngVel(double dt) const;
	const double getVelMagSq(double dt) const;
	const std::size_t getID();

	const sf::Vector2<double> getPos() const;
	const sf::Vector2<double> getLastPos() const;
	const double getAngle() const;
	const double getLastAngle() const;

private:
	sf::Vector2<double> pos;
	double angle;
	sf::Vector2<double> last_pos;
	double last_angle;
	sf::Vector2<double> acc;
	double ang_acc;

    const std::size_t id;
	std::size_t generateID();
};

