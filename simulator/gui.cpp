#include "Server.h"
#include "Simulator.h"
#include <QtCore/QCoreApplication>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  setlocale(LC_NUMERIC, "en_UK.UTF-8");
  srand(time(NULL));

  BoxSimulator simulator;
  simulator.initRandomly();
  simulator.buildUI();
  simulator.resize(1380, 1024);
  simulator.show();
  Server server(1234, &simulator);
  return app.exec();
}
