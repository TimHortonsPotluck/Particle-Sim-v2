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

	void setSimDT(double dt);

	void setPartVel(size_t index, sf::Vector2<double> v);
	const sf::Vector2<double>& getPartVel(size_t index);
	const std::vector<Particle>& getParticles() const;

	const sf::Vector2u& getWindowSize() const;

	double getTime() const;
	double getDT() const;
	int getFrameCount() const;
	size_t getNumParticles() const;

	double getTotalVelSq() const;
	double getAvgClosestDist(int grid_cells) const;

	void addGravity();
	void addLJAcc();
	void addLJWallRepellant();
	void addLinearDamping();

	void updateObjects(double dt);

	void randomizeParticleVels(double max);
	
	void setLJR0(double r0);
	void setLJDepth(double depth);
	void setLJCutoff(double cutoff);
	void setLJWallFriction(double friction);
	void setLinearDamping(double friction);

	void buildVerletList();

private:
	double time{ 0 };
	double dt{ 0 };
	int frame_count{ 0 };

	sf::Vector2u window_size;
	std::vector<Particle> particles;

	const sf::Vector2<double> gravity{ 0, .1 };

	double lj_r0{ 10 };
	double lj_depth{ 1 };
	double lj_cutoff{ 33 }; // depends on r0 and how much accuracy you want. look at desmos
	// look at your desmos graph file under jacksonpi99@gmail.com called "lennard jones in terms of force"
	// found here hopefully https://www.desmos.com/calculator/nhxygnqf6e
	// for more info on LJ stuff
	double lj_wall_friction{ 0 };

	double lin_damp_friction{ 0 };
	

	double lj_r0_sq{ 10 * 10 };
	double lj_depth_12{ 12 * 1 };
	double lj_cutoff_sq{ 33 * 33 };

	FixedGrid fg;
	std::vector<std::vector<size_t>> verlet_list;
	double verlet_skin{ 5 };

};