#include "Box.h"
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string.h>

void PersonBox::simulateMovement(size_t timesteps) {
  for (size_t t = 0; t < timesteps; t++) {
    for (size_t i = 0; i < people.size(); i++) {
      people[i].position[0] += TAU * people[i].velocity[0];
      people[i].position[1] += TAU * people[i].velocity[1];
    }
    reflectPeople();
  }
}

void PersonBox::simulateInfections() {
  for (size_t i = 0; i < people.size(); i++) {
    people[i].infectionTimer -= 0.01;
    if (people[i].infectionTimer < 0) {
      people[i].state = HEALTHY;
    }

    // q infects p
    for (auto q : people) {
      if (q.state == HEALTHY || q.infectionTimer > INFECTION_TIMER_INFECTIOUS)
        continue; // only infectious people can infect others
      double distance = std::hypot(people[i].position[0] - q.position[0], people[i].position[1] - q.position[1]);
      if (distance < 1e-5)
        continue;

      double infectionProbability = 0.8 * exp(-6 * distance);
      // std::cout << infectionProbability << std::endl;
      if ((double)rand() / RAND_MAX < infectionProbability) {
        people[i].state = INFECTED;
        people[i].infectionTimer = INFECTION_TIMER_MAX;
      }
    }
  }
}

void PersonBox::initRandomly(double initialKineticEnergy) {
  for (size_t i = 0; i < INITIAL_POP_SIZE; i++) {
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
    person.setVelocity(((double)rand() / RAND_MAX - 0.5) * 2 * INITIAL_MAX_SPEED,
        ((double)rand() / RAND_MAX - 0.5) * 2 * INITIAL_MAX_SPEED);
    people.push_back(person);
  }
}

void PersonBox::reflectPeople() {
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
