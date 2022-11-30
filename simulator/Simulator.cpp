#include "Simulator.h"

void BoxSimulator::buildUI() {
  QChartView *personView = new QChartView(this);
  {
    healthySeries->setName("Healthy");
    infectedSeries->setName("Infected");
    recoveredSeries->setName("Recovered");
    renderPeople();
    connect(healthySeries, &QScatterSeries::clicked, this, &BoxSimulator::infectPerson);

    // personChart->setTitle("Person Box");
    personChart->addSeries(healthySeries);
    personChart->addSeries(infectedSeries);
    personChart->addSeries(recoveredSeries);
    personChart->createDefaultAxes();
    personChart->axes(Qt::Horizontal).first()->setRange(0, BOX_WIDTH);
    personChart->axes(Qt::Vertical).first()->setRange(0, BOX_HEIGHT);
    personChart->setAnimationOptions(QChart::NoAnimation);
    personView->setRenderHint(QPainter::Antialiasing);
    personView->setChart(personChart);
    QSizePolicy policy = personView->sizePolicy();
    policy.setHorizontalStretch(2);
    personView->setSizePolicy(policy);
  }

  QChartView *energyView = new QChartView(this);
  {
    numHealthySeries->setName("Healthy");
    numInfectedSeries->setName("Infected");
    numRecoveredSeries->setName("Recovered");
    energyChart->addSeries(numHealthySeries);
    energyChart->addSeries(numInfectedSeries);
    energyChart->addSeries(numRecoveredSeries);
    // energyChart->setTitle("SIR development");
    energyChart->createDefaultAxes();

    energyChart->axes(Qt::Horizontal).first()->setRange(0, MEASUREMENTS_IN_ENERGY_PLOT);
    energyChart->axes(Qt::Horizontal).first()->setTitleText("Time");
    energyChart->axes(Qt::Vertical).first()->setTitleText("Number of people");

    energyView->setRenderHint(QPainter::Antialiasing);
    energyView->setMaximumHeight(300);
    // energyView->setMinimumWidth(400);
    energyView->setChart(energyChart);
  }

  // QChartView *velocityHistView = new QChartView(this);
  // {
  //   for (size_t bin = 0; bin < VELOCITY_HISTOGRAM_BINS; bin++)
  //     *velocityHistSet << 1;
  //   QStackedBarSeries *series = new QStackedBarSeries();
  //   series->append(velocityHistSet);
  //   velocityHistChart->addSeries(series);
  //   velocityHistChart->addAxis(new QValueAxis(), Qt::AlignBottom);
  //   series->attachAxis(new QValueAxis()); // this axis is not shown, only used for scaling
  //   velocityHistChart->axes(Qt::Horizontal).first()->setRange(0, VELOCITY_HISTOGRAM_BINS);
  //   QValueAxis *axisY = new QValueAxis();
  //   velocityHistChart->addAxis(axisY, Qt::AlignLeft);
  //   series->attachAxis(axisY);
  //   velocityHistChart->setAnimationOptions(QChart::SeriesAnimations);
  //   velocityHistView->setRenderHint(QPainter::Antialiasing);
  //   velocityHistView->setChart(velocityHistChart);
  //   velocityHistView->setMinimumWidth(450);
  // }
  // updateHistograms();

  connect(stepBtn, &QPushButton::clicked, [=]() { step(); });
  connect(controlBtn, &QPushButton::clicked, [=]() {
    if (controlBtn->text() == "Start") {
      _timerId = startTimer(10);
      // personView->chart()->setAnimationOptions(QChart::NoAnimation);
      velocityHistChart->setAnimationOptions(QChart::NoAnimation);
      std::cout << "Resetting squared mean velocity measurement" << std::endl;
      _start_step = _step;
      totalMeanVelocity = 0;
      controlBtn->setText("Stop");
    } else {
      killTimer(_timerId);
      double meanVel = totalMeanVelocity / ((_step - _start_step) * TAU);
      std::cout << "Squared Mean velocity result: " << meanVel << std::endl;
      // std::cout << "k_B * T / m = " << meanVel / 2 << std::endl;
      controlBtn->setText("Start");
    }
  });
  connect(liftBtn, &QPushButton::clicked, [=]() {
    for (size_t i = 0; i < people.size(); i++)
      people[i].position[1] += BOX_HEIGHT / 3;
    renderPeople();
    measure();
  });
  connect(slowDownBtn, &QPushButton::clicked, [=]() {
    for (size_t i = 0; i < people.size(); i++) {
      people[i].velocity[0] = pow(abs(people[i].velocity[0]), 0.3);
      people[i].velocity[1] = pow(abs(people[i].velocity[1]), 0.3);
    }
    measure();
  });
  connect(bringDownBtn, &QPushButton::clicked, [=]() {
    for (size_t i = 0; i < people.size(); i++)
      if (people[i].position[1] > BOX_HEIGHT * 0.8)
        people[i].position[1] = pow(abs(people[i].position[1]), 0.6);
    renderPeople();
    measure();
  });
  connect(reinitBtn, &QPushButton::clicked, [=]() {
    initRandomly();
    renderPeople();
    measure();
  });
  connect(exportBtn, &QPushButton::clicked, [=]() { exportToCSV(); });

  QComboBox *themeBox = new QComboBox();
  themeBox->addItem("Light");
  themeBox->addItem("Dark");
  themeBox->addItem("Cerulean Blue");
  themeBox->addItem("Brown Sand");
  themeBox->addItem("Icy Blue");
  connect(themeBox, &QComboBox::currentIndexChanged, [=]() {
    std::cout << themeBox->currentIndex();
    switch (themeBox->currentIndex()) {
    case 0:
      setTheme(QChart::ChartThemeLight);
      break;
    case 1:
      setTheme(QChart::ChartThemeDark);
      break;
    case 2:
      setTheme(QChart::ChartThemeBlueCerulean);
      break;
    case 3:
      setTheme(QChart::ChartThemeBrownSand);
      break;
    case 4:
      setTheme(QChart::ChartThemeBlueIcy);
      break;
    default:
      break;
    }
  });
  themeBox->setMaximumWidth(200);
  setTheme(QChart::ChartThemeLight);

  auto mainWidget = new QWidget(this);
  auto mainLayout = new QGridLayout(mainWidget);
  auto buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(controlBtn);
  buttonLayout->addWidget(stepBtn);
  buttonLayout->addWidget(liftBtn);
  buttonLayout->addWidget(slowDownBtn);
  buttonLayout->addWidget(bringDownBtn);
  buttonLayout->addWidget(reinitBtn);
  buttonLayout->addWidget(exportBtn);
  auto settingsLayout = new QVBoxLayout();
  settingsLayout->addWidget(autoScroll);
  settingsLayout->addWidget(themeBox);
  mainLayout->addLayout(buttonLayout, 0, 0, 1, 2);
  mainLayout->addWidget(personView, 1, 0, 1, 2);
  mainLayout->addWidget(energyView, 2, 0);
  mainLayout->addLayout(settingsLayout, 2, 1);
  mainLayout->addWidget(statsLabel, 3, 0);
  setCentralWidget(mainWidget);
  setWindowTitle("Person Box Simulator");

  QShortcut *closeShortcut = new QShortcut(Qt::CTRL | Qt::Key_W, this);
  QObject::connect(closeShortcut, &QShortcut::activated, this, [=]() { close(); });

  _timerId = startTimer(10);
}

void BoxSimulator::renderPeople() {
  healthySeries->clear();
  infectedSeries->clear();
  recoveredSeries->clear();
  for (size_t i = 0; i < people.size(); i++)
    if (people[i].state == HEALTHY)
      *healthySeries << QPointF(people[i].position[0], people[i].position[1]);
    else if (people[i].state == INFECTED)
      *infectedSeries << QPointF(people[i].position[0], people[i].position[1]);
    else
      *recoveredSeries << QPointF(people[i].position[0], people[i].position[1]);
}

void BoxSimulator::measure() {
  energyChart->axes(Qt::Vertical).first()->setRange(0, (double)people.size());

  double measurement = _step / STEPS_PER_MEASUREMENT;
  *numHealthySeries << QPointF(measurement, healthySeries->count());
  *numInfectedSeries << QPointF(measurement, infectedSeries->count());
  *numRecoveredSeries << QPointF(measurement, recoveredSeries->count());
  if (measurement > MEASUREMENTS_IN_ENERGY_PLOT)
    if (autoScroll->isChecked())
      energyChart->axes(Qt::Horizontal).first()->setRange(measurement - MEASUREMENTS_IN_ENERGY_PLOT, measurement);
    else
      energyChart->axes(Qt::Horizontal).first()->setRange(0, measurement);

  // statsLabel->setText(QString("t = %1 tu,\t E_kin = %2,\t E_pot = %3");
}

void BoxSimulator::step() {
  simulate(STEPS_PER_FRAME);
  renderPeople();
  _step += STEPS_PER_FRAME;
  if (_step % STEPS_PER_MEASUREMENT == 0)
    measure();
}

void BoxSimulator::timerEvent(QTimerEvent *event) { step(); }

void BoxSimulator::setTheme(QChart::ChartTheme theme) {
  energyChart->setTheme(theme);
  velocityHistChart->setTheme(theme);
  personChart->setTheme(theme);

  healthySeries->setColor(Qt::green);
  infectedSeries->setColor(Qt::red);
  recoveredSeries->setColor(Qt::blue);
  numHealthySeries->setColor(Qt::green);
  numInfectedSeries->setColor(Qt::red);
  numRecoveredSeries->setColor(Qt::blue);
}

void BoxSimulator::infectPerson(const QPointF &point) {
  personChart->setAnimationOptions(QChart::NoAnimation);
  std::cout << point.x() << ", " << point.y() << std::endl;
  for (size_t i = 0; i < people.size(); i++) {
    if (abs(people[i].position[0] - point.x()) < 1e-6 && abs(people[i].position[1] - point.y()) < 1e-6) {
      std::cout << "Found: " << people[i].position[0] << ", " << people[i].position[1] << std::endl;
      people[i].state = State::INFECTED;
      people[i].infectionTimer = INFECTION_TIMER_MAX;
      break;
    }
  }
  // QTimer *timer = new QTimer(this);
  // connect(timer, &QTimer::timeout, [=] { renderPeople(); });
  // timer->start(100);
}
