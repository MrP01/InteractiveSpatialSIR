#include "Box.h"
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string.h>

void PersonBox::simulate(size_t timesteps) {
  for (size_t t = 0; t < timesteps; t++) {
    simulateMovement();
    reflectPeople();
    simulateInfections();

#if CITY_INTERACTION
    if ((double)rand() / RAND_MAX < CITY_SWITCH_PROBABILITY) {
      Person *p = &people[rand() % people.size()];
      p->cityIndex = (p->cityIndex + 1) % cities.size();
    }
#endif
    time += TAU;
  }
}

void PersonBox::simulateMovement() {
  size_t S = 0, I = 0, R = 0;
  for (size_t i = 0; i < people.size(); i++) {
    Person *p = &people[i];

    if (CITY_INTERACTION && p->npc) {
      struct City city = cities[p->cityIndex];
      double dx = p->position[0] - city.center[0], dy = p->position[1] - city.center[1];
      double distanceToCity = std::hypot(dx, dy);
      if (distanceToCity > city.radius) { // Person p is outside of the city
        double urbanForce = 420 * distanceToCity / city.radius;
        p->velocity[0] -= urbanForce * dx / distanceToCity * TAU;
        p->velocity[1] -= urbanForce * dy / distanceToCity * TAU;
        // std::cout << "urban force: " << p->velocity[0] << ", " << p->velocity[1] << std::endl;
      } else {
        double totalVelocity = std::hypot(p->velocity[0], p->velocity[1]);
        p->velocity[0] /= totalVelocity * 0.1 + (double)rand() / RAND_MAX - 0.5;
        p->velocity[1] /= totalVelocity * 0.1 + (double)rand() / RAND_MAX - 0.5;
      }
    }

    p->position[0] += TAU * p->velocity[0];
    p->position[1] += TAU * p->velocity[1];

    switch (p->state) {
    case HEALTHY:
      S++;
      break;
    case INFECTED:
      I++;
      break;
    case RECOVERED:
      R++;
      break;
    }
  }
  logs.push_back({time, S, I, R});
}

void PersonBox::simulateInfections() {
  for (auto p = people.begin(); p < people.end(); p++) {
    if (p->infectionTimer > 0) {
      p->infectionTimer -= 0.01;
      if (p->infectionTimer <= 0.03)
        p->state = RECOVERED;
    }

    if (p->state != HEALTHY)
      continue; // Person p is already infected or recovered, no need to check for more
    // q infects p:
    for (auto q : people) {
      if (q.state != INFECTED)
        continue; // only infectious people can infect others
      double distance = std::hypot(p->position[0] - q.position[0], p->position[1] - q.position[1]);
      if (distance < 1e-5)
        continue;

      double infectionProbability = 0.4 * exp(-9 * distance);
      // std::cout << infectionProbability << std::endl;
      if ((double)rand() / RAND_MAX < infectionProbability) {
        p->state = INFECTED;
        p->infectionTimer = INFECTION_TIMER_MAX;
      }
    }
  }
}

void PersonBox::initRandomly() {
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
    }

    person.setVelocity(((double)rand() / RAND_MAX - 0.5) * 2 * INITIAL_MAX_SPEED,
        ((double)rand() / RAND_MAX - 0.5) * 2 * INITIAL_MAX_SPEED);
    person.cityIndex = i % cities.size();
    people.push_back(person);
  }
}

void PersonBox::reflectPeople() {
  for (auto p = people.begin(); p < people.end(); p++) {
    if (p->position[0] < 0) {
      p->position[0] = -p->position[0]; // assumes linear movement in this timestep
      p->velocity[0] = -p->velocity[0];
    } else if (p->position[0] > BOX_WIDTH) {
      p->position[0] = BOX_WIDTH - (p->position[0] - BOX_WIDTH); // assumes linear movement
      p->velocity[0] = -p->velocity[0];
    }
    if (p->position[1] < 0) {
      p->position[1] = -p->position[1]; // assumes linear movement
      p->velocity[1] = -p->velocity[1];
    } else if (p->position[1] > BOX_HEIGHT) {
      p->position[1] = BOX_HEIGHT - (p->position[1] - BOX_HEIGHT);
      p->velocity[1] = -p->velocity[1];
    }
  }
}

void PersonBox::computeVelocityHistogram() {
  std::vector<double> values;
  for (auto p : people)
    values.push_back(sqrt(square(p.velocity[0]) + square(p.velocity[1])));
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
  for (auto p : people)
    positionsCsv << p.position[0] << ", " << p.position[1] << "\n";
  positionsCsv.close();
  std::ofstream velocitiesCsv("/tmp/velocities.csv");
  for (auto p : people)
    velocitiesCsv << p.velocity[0] << ", " << p.velocity[1] << "\n";
  velocitiesCsv.close();

  std::ofstream timeseriesCsv("/tmp/timeseries.csv");
  for (auto log : logs)
    timeseriesCsv << log.time << ", " << log.healthy << ", " << log.infected << ", " << log.recovered << "\n";
  timeseriesCsv.close();
}
