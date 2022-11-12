#include "Box.h"
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string.h>

void ParticleBox::initRandomly(double initialKineticEnergy, double initialGravitationalPotential) {
  double approxHeight = initialGravitationalPotential / (PARTICLE_MASS * GRAVITY);
  for (size_t i = 0; i < PARTICLES; i++) {
    double closestNeighbourDist = 0;
    while (closestNeighbourDist < 0.8 * LJ_SIGMA) {
      people[i].setPosition(((double)rand() / RAND_MAX) * BOX_WIDTH, ((double)rand() / RAND_MAX) * approxHeight);
      closestNeighbourDist = LJ_SIGMA;
      for (size_t j = 0; j < i; j++)
        closestNeighbourDist = std::min(closestNeighbourDist, distanceBetween(i, j));
      std::cout << "." << std::flush;
    }

    std::cout << "Init particle at " << people[i].position[0] << ", " << people[i].position[1] << std::endl;
    people[i].setVelocity(((double)rand() / RAND_MAX) - 0.5, ((double)rand() / RAND_MAX) - 0.5);
  }
}

void ParticleBox::f(ParticleVectors &accelerations) {
  for (size_t i = 0; i < PARTICLES; i++) {
    double force_x = 0, force_y = 0;
    for (size_t j = 0; j < PARTICLES; j++) {
      if (i == j)
        continue;
      double dx = people[i].position[0] - people[j].position[0], dy = people[i].position[1] - people[j].position[1];
      double r_squared = dx * dx + dy * dy;
      if (r_squared < LJ_CUT_DIST_SQ)
        r_squared = LJ_CUT_DIST_SQ;
      double factor = (2 * std::pow(LJ_SIGMA_SQ / r_squared, 6) - std::pow(LJ_SIGMA_SQ / r_squared, 3)) / r_squared;
      force_x += dx * factor;
      force_y += dy * factor;
      // std::cout << "Distance_squared:" << r_squared << std::endl;
    }
    // std::cout << "Force: " << 24 * LJ_EPSILON * force_x << ", " << 24 * LJ_EPSILON * force_y - GRAVITY << std::endl;
    accelerations[i][0] = 24 * LJ_EPSILON * force_x / PARTICLE_MASS;
    accelerations[i][1] = 24 * LJ_EPSILON * force_y / PARTICLE_MASS - GRAVITY;
  }
}

void ParticleBox::simulate(size_t timesteps) {
  double after_accelerations[PARTICLES][2];
  f(after_accelerations);
  for (size_t t = 0; t < timesteps; t++) {
    ParticleVectors before_accelerations = after_accelerations;
    memcpy(before_accelerations, after_accelerations, PARTICLES * 2 * sizeof(double));
    for (size_t i = 0; i < PARTICLES; i++) {
      people[i].position[0] += TAU * people[i].velocity[0] + square(TAU) / 2 * before_accelerations[i][0];
      people[i].position[1] += TAU * people[i].velocity[1] + square(TAU) / 2 * before_accelerations[i][1];
    }

    f(after_accelerations);
    double totalVelocity = 0;
    for (size_t i = 0; i < PARTICLES; i++) {
      people[i].velocity[0] += TAU / 2 * (before_accelerations[i][0] + after_accelerations[i][0]);
      people[i].velocity[1] += TAU / 2 * (before_accelerations[i][1] + after_accelerations[i][1]);
      totalVelocity += square(people[i].velocity[0]) + square(people[i].velocity[1]);
    }
    totalMeanVelocity += totalVelocity / PARTICLES;
    reflectParticles();
    // time += TIME_STEP
  }
}

void ParticleBox::reflectParticles() {
  for (size_t i = 0; i < PARTICLES; i++) {
    if (people[i].position[0] < 0) {
      people[i].position[0] = -people[i].position[0]; // assumes linear movement in this timestep
      people[i].velocity[0] = -people[i].velocity[0];
    } else if (people[i].position[0] > BOX_WIDTH) {
      people[i].position[0] = BOX_WIDTH - (people[i].position[0] - BOX_WIDTH); // assumes linear movement
      people[i].velocity[0] = -people[i].velocity[0];
    }
    if (people[i].position[1] < 0) {
      people[i].position[1] = -people[i].position[1]; // assumes linear movement
      people[i].velocity[1] = -people[i].velocity[1];
    }
  }
}

double ParticleBox::getKineticEnergy() {
  double energy = 0;
  for (size_t i = 0; i < PARTICLES; i++)
    energy += square(people[i].velocity[0]) + square(people[i].velocity[1]);
  return PARTICLE_MASS / 2 * energy;
}
double ParticleBox::getGravitationalPotential() { return PARTICLE_MASS * GRAVITY; }
double ParticleBox::getLJPotential() {
  double energy = 0;
  for (size_t i = 0; i < PARTICLES; i++) {
    for (size_t j = 0; j < PARTICLES; j++) {
      if (i == j)
        continue;
      double dx = people[i].position[0] - people[j].position[0], dy = people[i].position[1] - people[j].position[1];
      double r_squared = dx * dx + dy * dy;
      if (r_squared < LJ_CUT_DIST_SQ)
        r_squared = LJ_CUT_DIST_SQ;
      energy += abs(std::pow(LJ_SIGMA_SQ / r_squared, 6) - std::pow(LJ_SIGMA_SQ / r_squared, 3));
      // std::cout << r_squared << ", " << energy << std::endl;
    }
  }
  return 4 * LJ_EPSILON * energy;
}
double ParticleBox::getTotalEnergy() { return getKineticEnergy() + getGravitationalPotential() + getLJPotential(); }

void ParticleBox::computeVelocityHistogram() {
  std::array<double, PARTICLES> values;
  for (size_t i = 0; i < PARTICLES; i++)
    values[i] = sqrt(square(people[i].velocity[0]) + square(people[i].velocity[1]));
  const auto [_min, _max] = std::minmax_element(values.begin(), values.end());
  velocityHist.min = *_min;
  velocityHist.max = *_max;
  const double delta = velocityHist.max - velocityHist.min;
  std::fill(std::begin(velocityHist.heights), std::end(velocityHist.heights), 0);
  for (size_t i = 0; i < PARTICLES; i++) {
    size_t bin = floor((values[i] - velocityHist.min) / delta * VELOCITY_HISTOGRAM_BINS);
    if (bin >= VELOCITY_HISTOGRAM_BINS)
      bin = VELOCITY_HISTOGRAM_BINS - 1;
    velocityHist.heights[bin]++;
  }
  velocityHist.maxHeight = 0;
  for (size_t bin = 0; bin < VELOCITY_HISTOGRAM_BINS; bin++)
    velocityHist.maxHeight = std::max(velocityHist.maxHeight, velocityHist.heights[bin]);
}

void ParticleBox::exportToCSV() {
  std::ofstream positionsCsv("/tmp/positions.csv");
  for (size_t i = 0; i < PARTICLES; i++)
    positionsCsv << people[i].position[0] << ", " << people[i].position[1] << "\n";
  positionsCsv.close();
  std::ofstream velocitiesCsv("/tmp/velocities.csv");
  for (size_t i = 0; i < PARTICLES; i++)
    velocitiesCsv << people[i].velocity[0] << ", " << people[i].velocity[1] << "\n";
  velocitiesCsv.close();
}
