#define INFECTION_TIMER_MAX 20

enum State {
  HEALTHY,
  INFECTED,
};

class Person {
 public:
  double position[2];
  double velocity[2];
  enum State state = HEALTHY;
  double infectionTimer;

  void setPosition(double x, double y) {
    position[0] = x;
    position[1] = y;
  };

  void setVelocity(double x, double y) {
    velocity[0] = x;
    velocity[1] = y;
  };
};
