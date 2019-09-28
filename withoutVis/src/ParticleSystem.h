#pragma once

#include "Tree.h"
#include "Particle.h"

#include <vector>

class ParticleSystem {
protected:
	float timeStep;
	std::vector<Particle> particles;
	Tree* tree;

public:
	ParticleSystem();

	void setTimeStep(float timeStep);

	void add(Particle& p);
	std::vector<Particle*> getNeighbors(float x, float y, float radius) const;
	unsigned size() const;
	Particle& operator[](unsigned i);

	void setupForces();
	void addRepulsionForce(const Particle& particle, float radius, float scale);
	void addRepulsionForce(float x, float y, float radius, float scale);
	void addAttractionForce(const Particle& particle, float radius, float scale);
	void addAttractionForce(float x, float y, float radius, float scale);
	void addForce(const Particle& particle, float radius, float scale);
	void addForce(float x, float y, float radius, float scale);
	void update();


	void drawTree();
	void draw();


};
