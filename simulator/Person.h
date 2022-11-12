enum State {
  HEALTHY,
  INFECTED,
};

class Person {
 public:
  double position[2];
  double velocity[2];
  enum State state = HEALTHY;
};
