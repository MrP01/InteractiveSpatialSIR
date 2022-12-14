#pragma once

#include "Box.h"
#include "SimulatorThread.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QTimer>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QValueAxis>

#define STEPS_PER_FRAME 1        // number of timesteps per frame
#define FRAMES_PER_MEASUREMENT 2 // how often we measure
#define STEPS_PER_MEASUREMENT (STEPS_PER_FRAME * FRAMES_PER_MEASUREMENT)
#define MEASUREMENTS_IN_ENERGY_PLOT 600

class BoxSimulator : public PersonBox, public QMainWindow {
 private:
  QChart *personChart = new QChart();
  QScatterSeries *healthySeries = new QScatterSeries();
  QScatterSeries *infectedSeries = new QScatterSeries();
  QScatterSeries *recoveredSeries = new QScatterSeries();

  QChart *energyChart = new QChart();
  QLineSeries *numHealthySeries = new QLineSeries();
  QLineSeries *numInfectedSeries = new QLineSeries();
  QLineSeries *numRecoveredSeries = new QLineSeries();

  QChart *velocityHistChart = new QChart();
  QBarSet *velocityHistSet = new QBarSet("Velocity");

  QPushButton *stepBtn = new QPushButton("Step");
  QPushButton *controlBtn = new QPushButton("Stop");
  QPushButton *liftBtn = new QPushButton("Lift Up");
  QPushButton *slowDownBtn = new QPushButton("Slow Down");
  QPushButton *bringDownBtn = new QPushButton("Bring Down");
  QPushButton *reinitBtn = new QPushButton("Add more");
  QPushButton *exportBtn = new QPushButton("Export");
  QLabel *statsLabel = new QLabel(QString("Persons in a box of width %1").arg(BOX_WIDTH));
  QCheckBox *autoScroll = new QCheckBox("Auto-Scroll");

  size_t _step = 0;
  size_t _start_step = 0;
  double _energyMax = 0;
  size_t _timerId;
  void renderPeople();
  void updateHistograms();
  void measure();
  void step();
  void timerEvent(QTimerEvent *event);
  void infectPerson(const QPointF &point);

  // very important:
  void setTheme(QChart::ChartTheme theme);

 public:
  BoxSimulator() = default;
  void buildUI();
};
