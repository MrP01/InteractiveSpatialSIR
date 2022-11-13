#include "Box.h"
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string.h>

void PersonBox::initRandomly(double initialKineticEnergy) {
  // double approxHeight = initialGravitationalPotential / (PARTICLE_MASS * GRAVITY);
  for (size_t i = 0; i < 120; i++) {
    Person person = Person();
    double closestNeighbourDist = 0;
    while (closestNeighbourDist < 0.8) {
      person.setPosition(((double)rand() / RAND_MAX) * BOX_WIDTH, ((double)rand() / RAND_MAX) * BOX_HEIGHT);
      if (i < 1)
        break;
      closestNeighbourDist = 0.8 + 1;
      for (size_t j = 0; j < i; j++) {
        closestNeighbourDist = std::min(closestNeighbourDist,
            std::hypot(person.position[0] - people[j].position[0], person.position[1] - people[j].position[1]));
      }
      // std::cout << "." << std::flush;
    }

    // std::cout << "Init person at " << person.position[0] << ", " << person.position[1] << std::endl;
    person.setVelocity(((double)rand() / RAND_MAX) * 1.6 - 0.8, ((double)rand() / RAND_MAX) * 1.6 - 0.8);
    people.push_back(person);
  }
}

void PersonBox::simulate(size_t timesteps) {
  for (size_t t = 0; t < timesteps; t++) {
    for (size_t i = 0; i < people.size(); i++) {
      people[i].position[0] += TAU * people[i].velocity[0];
      people[i].position[1] += TAU * people[i].velocity[1];
    }
    reflectPersons();
    // time += TIME_STEP
  }
}

void PersonBox::reflectPersons() {
  for (size_t i = 0; i < people.size(); i++) {
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
    } else if (people[i].position[1] > BOX_HEIGHT) {
      people[i].position[1] = BOX_HEIGHT - (people[i].position[1] - BOX_HEIGHT);
      people[i].velocity[1] = -people[i].velocity[1];
    }
  }
}

void PersonBox::computeVelocityHistogram() {
  std::vector<double> values;
  for (size_t i = 0; i < people.size(); i++)
    values.push_back(sqrt(square(people[i].velocity[0]) + square(people[i].velocity[1])));
  const auto [_min, _max] = std::minmax_element(values.begin(), values.end());
  velocityHist.min = *_min;
  velocityHist.max = *_max;
  const double delta = velocityHist.max - velocityHist.min;
  std::fill(std::begin(velocityHist.heights), std::end(velocityHist.heights), 0);
  for (size_t i = 0; i < people.size(); i++) {
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
  for (size_t i = 0; i < people.size(); i++)
    positionsCsv << people[i].position[0] << ", " << people[i].position[1] << "\n";
  positionsCsv.close();
  std::ofstream velocitiesCsv("/tmp/velocities.csv");
  for (size_t i = 0; i < people.size(); i++)
    velocitiesCsv << people[i].velocity[0] << ", " << people[i].velocity[1] << "\n";
  velocitiesCsv.close();
}
