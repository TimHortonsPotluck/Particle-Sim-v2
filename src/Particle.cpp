#include "Particle.h"
#include "Print.h"

Particle::Particle(sf::Vector2<double> pos, double angle, double radius) : pos{ pos }, angle{ angle }, last_pos{ pos }, last_angle{ angle }, acc{ sf::Vector2<double>(0, 0) }, ang_acc{ 0 }, radius{ radius }, color{ sf::Color::White }, id { generateID() } {}

void Particle::update(double dt) {
	const sf::Vector2<double> next_pos = 2. * pos - last_pos + acc * dt * dt;
	last_pos = pos;
	pos = next_pos;
	acc = { 0, 0 };

	const double next_angle = 2. * angle - last_angle + ang_acc * dt * dt;
	last_angle = angle;
	angle = next_angle;
	ang_acc = 0;
}

void Particle::addAcc(sf::Vector2<double> a) {
	acc += a;
}

void Particle::setVel(sf::Vector2<double> v, double dt) {
	last_pos = pos - v * dt;
}

void Particle::addVel(sf::Vector2<double> v, double dt) {
	last_pos -= v * dt;
}

sf::Vector2<double> Particle::getVel(double dt) const {
	return (pos - last_pos) / dt;
}

void Particle::addAngAcc(double a_a) {
	ang_acc += a_a;
}

void Particle::setAngVel(double a_v, double dt) {
	last_angle = angle - a_v * dt;
}

void Particle::addAngVel(double a_v, double dt) {
	last_angle -= a_v * dt;
}

double Particle::getAngVel(double dt) const {
	return (angle - last_angle) / dt;
}

const double Particle::getVelMagSq(double dt) const {
	const sf::Vector2<double> vel = getVel(dt);
	return vel.x * vel.x + vel.y * vel.y;
}

const std::size_t Particle::getID() {
	return id;
}

const sf::Vector2<double> Particle::getPos() const {
	return pos;
}

const sf::Vector2<double> Particle::getLastPos() const {
	return last_pos;
}

const double Particle::getAngle() const {
	return angle;
}

const double Particle::getLastAngle() const {
	return last_angle;
}

inline std::size_t Particle::generateID() {
	static std::size_t static_id{ 0 };
	return static_id++;
}


