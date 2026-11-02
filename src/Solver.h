#pragma once
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "FixedGrid.h"
#include "Particle.h"
#include "Random.h"

class Solver {
public:
	Solver(sf::Vector2u window_size, double grid_size);

	void addParticle(sf::Vector2<double> pos, double angle, double radius);
	
	void update();

	void setSimUpdateRate(int rate);
	void setSubStepsCount(int sub_steps);

	void setPartVel(size_t index, sf::Vector2<double> v);
	const sf::Vector2<double>& getPartVel(size_t index);
	const std::vector<Particle>& getParticles() const;

	const sf::Vector2u& getWindowSize() const;

	double getTime() const;
	double getStepDT() const;
	size_t getNumParticles() const;

	double getTotalVelSq() const;

	void addGravity();
	void addLJAcc();
	void AddLJWallRepellant(double friction_coeff);

	void updateObjects(double dt);

	void randomizeParticleVels(double max);

private:
	int sub_steps{ 1 };
	double time{ 0 };
	double frame_dt{ 0 };

	sf::Vector2u window_size;
	std::vector<Particle> particles;

	const sf::Vector2<double> gravity{ 0, .1 };

	const double lj_r0{ 10 };
	const double lj_depth{ 1 };
	const double lj_cutoff{ 33 }; // depends on r0 and how much accuracy you want. look at desmos
	// look at your desmos graph file under jacksonpi99@gmail.com called "lennard jones in terms of force"
	// found here hopefully https://www.desmos.com/calculator/nhxygnqf6e
	// for more info on LJ stuff

	FixedGrid fg;

};