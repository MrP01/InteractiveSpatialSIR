#include "Box.h"
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string.h>

void PersonBox::initRandomly(double initialKineticEnergy, double initialGravitationalPotential) {
  double approxHeight = initialGravitationalPotential / (PARTICLE_MASS * GRAVITY);
  for (size_t i = 0; i < POPULATION_SIZE; i++) {
    double closestNeighbourDist = 0;
    while (closestNeighbourDist < 0.8) {
      people[i].setPosition(((double)rand() / RAND_MAX) * BOX_WIDTH, ((double)rand() / RAND_MAX) * approxHeight);
      closestNeighbourDist = 0.8 + 1;
      for (size_t j = 0; j < i; j++)
        closestNeighbourDist = std::min(closestNeighbourDist, distanceBetween(i, j));
      std::cout << "." << std::flush;
    }

    std::cout << "Init person at " << people[i].position[0] << ", " << people[i].position[1] << std::endl;
    people[i].setVelocity(((double)rand() / RAND_MAX) - 0.5, ((double)rand() / RAND_MAX) - 0.5);
  }
}

void PersonBox::f(PersonVectors &accelerations) {
  for (size_t i = 0; i < POPULATION_SIZE; i++) {
    accelerations[i][0] = 0;
    accelerations[i][1] = 0;
  }
}

void PersonBox::simulate(size_t timesteps) {
  double after_accelerations[POPULATION_SIZE][2];
  f(after_accelerations);
  for (size_t t = 0; t < timesteps; t++) {
    PersonVectors before_accelerations = after_accelerations;
    memcpy(before_accelerations, after_accelerations, POPULATION_SIZE * 2 * sizeof(double));
    for (size_t i = 0; i < POPULATION_SIZE; i++) {
      people[i].position[0] += TAU * people[i].velocity[0] + square(TAU) / 2 * before_accelerations[i][0];
      people[i].position[1] += TAU * people[i].velocity[1] + square(TAU) / 2 * before_accelerations[i][1];
    }

    f(after_accelerations);
    double totalVelocity = 0;
    for (size_t i = 0; i < POPULATION_SIZE; i++) {
      people[i].velocity[0] += TAU / 2 * (before_accelerations[i][0] + after_accelerations[i][0]);
      people[i].velocity[1] += TAU / 2 * (before_accelerations[i][1] + after_accelerations[i][1]);
      totalVelocity += square(people[i].velocity[0]) + square(people[i].velocity[1]);
    }
    totalMeanVelocity += totalVelocity / POPULATION_SIZE;
    reflectPersons();
    // time += TIME_STEP
  }
}

void PersonBox::reflectPersons() {
  for (size_t i = 0; i < POPULATION_SIZE; i++) {
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

double PersonBox::getKineticEnergy() {
  double energy = 0;
  for (size_t i = 0; i < POPULATION_SIZE; i++)
    energy += square(people[i].velocity[0]) + square(people[i].velocity[1]);
  return PARTICLE_MASS / 2 * energy;
}
double PersonBox::getGravitationalPotential() { return PARTICLE_MASS * GRAVITY; }
double PersonBox::getTotalEnergy() { return getKineticEnergy() + getGravitationalPotential(); }

void PersonBox::computeVelocityHistogram() {
  std::array<double, POPULATION_SIZE> values;
  for (size_t i = 0; i < POPULATION_SIZE; i++)
    values[i] = sqrt(square(people[i].velocity[0]) + square(people[i].velocity[1]));
  const auto [_min, _max] = std::minmax_element(values.begin(), values.end());
  velocityHist.min = *_min;
  velocityHist.max = *_max;
  const double delta = velocityHist.max - velocityHist.min;
  std::fill(std::begin(velocityHist.heights), std::end(velocityHist.heights), 0);
  for (size_t i = 0; i < POPULATION_SIZE; i++) {
    size_t bin = floor((values[i] - velocityHist.min) / delta * VELOCITY_HISTOGRAM_BINS);
    if (bin >= VELOCITY_HISTOGRAM_BINS)
      bin = VELOCITY_HISTOGRAM_BINS - 1;
    velocityHist.heights[bin]++;
  }
  velocityHist.maxHeight = 0;
  for (size_t bin = 0; bin < VELOCITY_HISTOGRAM_BINS; bin++)
    velocityHist.maxHeight = std::max(velocityHist.maxHeight, velocityHist.heights[bin]);
}

void PersonBox::exportToCSV() {
  std::ofstream positionsCsv("/tmp/positions.csv");
  for (size_t i = 0; i < POPULATION_SIZE; i++)
    positionsCsv << people[i].position[0] << ", " << people[i].position[1] << "\n";
  positionsCsv.close();
  std::ofstream velocitiesCsv("/tmp/velocities.csv");
  for (size_t i = 0; i < POPULATION_SIZE; i++)
    velocitiesCsv << people[i].velocity[0] << ", " << people[i].velocity[1] << "\n";
  velocitiesCsv.close();
}
