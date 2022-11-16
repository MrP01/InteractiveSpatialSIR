#pragma once

#include "Person.h"
#include <array>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <vector>

// be careful to set numeric values as floats here
#define INITIAL_POP_SIZE 120       // initial population size
#define INITIAL_MAX_SPEED 3        // initial velocity max
#define BOX_WIDTH 16               // width of the infinitely high box
#define BOX_HEIGHT 30              // height of the plot
#define GRAVITY 8.532e1            // 9.81 m/s², actual value in reduced units: 8.532e-05
#define TAU 8.0e-4                 // time step
#define VELOCITY_HISTOGRAM_BINS 16 // similarly, number of bins for the velocity histogram
#define ONE_SECOND 2.1257e-12      // one second in reduced time unit

#define square(x) (x * x)

#define LJ_CUT_DIST_SQ (LJ_CUTOFF_DISTANCE * LJ_CUTOFF_DISTANCE)
#define LJ_SIGMA_SQ (LJ_SIGMA * LJ_SIGMA)

// using PersonVectors = double (&)[POPULATION_SIZE][2];

struct VelocityHistogram {
  double min, max;
  size_t heights[VELOCITY_HISTOGRAM_BINS] = {0};
  size_t maxHeight = 1;
};

class PersonBox {
 public:
  std::vector<Person> people{};
  struct VelocityHistogram velocityHist;
  double totalMeanVelocity = 0;

  double distanceBetween(size_t i, size_t j) {
    return std::hypot(people[i].position[0] - people[j].position[0], people[i].position[1] - people[j].position[1]);
  }

 public:
  PersonBox() = default;
  void initRandomly(double initialKineticEnergy);
  void simulateMovement(size_t timesteps);
  void reflectPeople();
  void computeVelocityHistogram();
  void exportToCSV();
  void simulateInfections();

  size_t countHealthy() {
    size_t n = 0;
    for (auto p : people)
      if (p.state == HEALTHY)
        n++;
    return n;
  };

  size_t countInfected() {
    size_t n = 0;
    for (auto p : people)
      if (p.state == INFECTED)
        n++;
    return n;
  };
};
