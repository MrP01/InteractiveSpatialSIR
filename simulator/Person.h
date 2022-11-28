#include <stdlib.h>

#define INFECTION_TIMER_MAX 12
#define INFECTION_TIMER_INFECTIOUS 8

enum State {
  HEALTHY,
  INFECTED,
  RECOVERED,
};

class Person {
 public:
  double position[2];
  double velocity[2];
  enum State state = HEALTHY;
  double infectionTimer = 0;
  size_t cityIndex = 0;

  void setPosition(double x, double y) {
    position[0] = x;
    position[1] = y;
  };

  void setVelocity(double x, double y) {
    velocity[0] = x;
    velocity[1] = y;
  };

  void moveToAnotherCity();
};
