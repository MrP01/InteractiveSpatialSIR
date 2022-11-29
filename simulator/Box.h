#pragma once

#include "Person.h"
#include <array>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <vector>

// be careful to set numeric values as floats here
#define INITIAL_POP_SIZE 120          // initial population size
#define INITIAL_MAX_SPEED 5           // initial velocity max
#define BOX_WIDTH 32                  // width of the infinitely high box
#define BOX_HEIGHT 20                 // height of the plot
#define GRAVITY 8.532e1               // 9.81 m/s², actual value in reduced units: 8.532e-05
#define TAU 4.0e-3                    // time step
#define VELOCITY_HISTOGRAM_BINS 16    // similarly, number of bins for the velocity histogram
#define ONE_SECOND 2.1257e-12         // one second in reduced time unit
#define CITY_INTERACTION 1            // cities active or not
#define CITY_SWITCH_PROBABILITY 0.004 // probability that a person commutes

#define square(x) (x * x)

struct VelocityHistogram {
  double min, max;
  size_t heights[VELOCITY_HISTOGRAM_BINS] = {0};
  size_t maxHeight = 1;
};

struct City {
  double center[2];
  double radius;
};

struct Log {
  double time;
  size_t healthy;
  size_t infected;
  size_t recovered;
};

class PersonBox {
 public:
  std::vector<Person> people{};
  std::vector<struct City> cities{{{4, 2}, 6}, {{27, 9}, 6}, {{10, 18}, 7}};
  struct VelocityHistogram velocityHist;
  double time = 0;
  double totalMeanVelocity = 0;
  std::vector<struct Log> logs{};

  double distanceBetween(size_t i, size_t j) {
    return std::hypot(people[i].position[0] - people[j].position[0], people[i].position[1] - people[j].position[1]);
  }

 public:
  PersonBox() = default;
  void initRandomly();
  void simulate(size_t timesteps);
  void reflectPeople();
  void computeVelocityHistogram();
  void exportToCSV();
  void simulateMovement();
  void simulateInfections();
};
