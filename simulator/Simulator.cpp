#include "Simulator.h"

void BoxSimulator::buildUI() {
  QChartView *particleView = new QChartView(this);
  {
    particleSeries->setName("Particles");
    renderParticles();

    particleChart->setTitle("Particle Box");
    particleChart->addSeries(particleSeries);
    particleChart->createDefaultAxes();
    particleChart->axes(Qt::Horizontal).first()->setRange(0, BOX_WIDTH);
    particleChart->axes(Qt::Vertical).first()->setRange(0, PLOT_HEIGHT);
    particleChart->setAnimationOptions(QChart::SeriesAnimations);
    particleView->setRenderHint(QPainter::Antialiasing);
    particleView->setChart(particleChart);
    QSizePolicy policy = particleView->sizePolicy();
    policy.setHorizontalStretch(2);
    particleView->setSizePolicy(policy);
  }

  QChartView *energyView = new QChartView(this);
  {
    kineticEnergySeries->setName("Kinetic Energy");
    potentialEnergySeries->setName("Gravitational Potential");
    LJpotentialEnergySeries->setName("LJ Potential");
    totalEnergySeries->setName("Total Energy");
    energyChart->addSeries(kineticEnergySeries);
    energyChart->addSeries(potentialEnergySeries);
    energyChart->addSeries(LJpotentialEnergySeries);
    energyChart->addSeries(totalEnergySeries);
    energyChart->setTitle("Energy development");
    energyChart->createDefaultAxes();

    energyChart->axes(Qt::Horizontal).first()->setRange(0, MEASUREMENTS_IN_ENERGY_PLOT);
    energyChart->axes(Qt::Horizontal)
        .first()
        ->setTitleText(QString("Measurement n / %1 steps").arg(STEPS_PER_MEASUREMENT));
    energyChart->axes(Qt::Vertical).first()->setTitleText("Energy log10(E) / log10(eu)");

    energyView->setRenderHint(QPainter::Antialiasing);
    energyView->setChart(energyChart);
  }

  QChartView *velocityHistView = new QChartView(this);
  {
    for (size_t bin = 0; bin < VELOCITY_HISTOGRAM_BINS; bin++)
      *velocityHistSet << 1;
    QStackedBarSeries *series = new QStackedBarSeries();
    series->append(velocityHistSet);
    velocityHistChart->addSeries(series);
    velocityHistChart->addAxis(new QValueAxis(), Qt::AlignBottom);
    series->attachAxis(new QValueAxis()); // this axis is not shown, only used for scaling
    velocityHistChart->axes(Qt::Horizontal).first()->setRange(0, VELOCITY_HISTOGRAM_BINS);
    QValueAxis *axisY = new QValueAxis();
    velocityHistChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    velocityHistChart->setAnimationOptions(QChart::SeriesAnimations);
    velocityHistView->setRenderHint(QPainter::Antialiasing);
    velocityHistView->setChart(velocityHistChart);
    velocityHistView->setMinimumWidth(450);
  }
  updateHistograms();

  connect(stepBtn, &QPushButton::clicked, [=]() { step(); });
  connect(controlBtn, &QPushButton::clicked, [=]() {
    if (controlBtn->text() == "Start") {
      _timerId = startTimer(10);
      particleView->chart()->setAnimationOptions(QChart::NoAnimation);
      //heightHistChart->setAnimationOptions(QChart::NoAnimation);
      velocityHistChart->setAnimationOptions(QChart::NoAnimation);
      std::cout << "Resetting squared mean velocity measurement" << std::endl;
      _start_step = _step;
      totalMeanVelocity = 0;
      controlBtn->setText("Stop");
    } else {
      killTimer(_timerId);
      double meanVel = totalMeanVelocity / ((_step - _start_step) * TAU);
      std::cout << "Squared Mean velocity result: " << meanVel << std::endl;
      std::cout << "k_B * T / m = " << meanVel / 2 << std::endl;
      controlBtn->setText("Start");
    }
  });
  connect(liftBtn, &QPushButton::clicked, [=]() {
    for (size_t i = 0; i < PARTICLES; i++)
      positions[i][1] += PLOT_HEIGHT / 3;
    renderParticles();
    measure();
  });
  connect(slowDownBtn, &QPushButton::clicked, [=]() {
    for (size_t i = 0; i < PARTICLES; i++) {
      velocities[i][0] = pow(abs(velocities[i][0]), 0.3);
      velocities[i][1] = pow(abs(velocities[i][1]), 0.3);
    }
    measure();
  });
  connect(bringDownBtn, &QPushButton::clicked, [=]() {
    for (size_t i = 0; i < PARTICLES; i++)
      if (positions[i][1] > PLOT_HEIGHT * 0.8)
        positions[i][1] = pow(abs(positions[i][1]), 0.6);
    renderParticles();
    measure();
  });
  connect(reinitBtn, &QPushButton::clicked, [=]() {
    initRandomly(10, PLOT_HEIGHT * GRAVITY * PARTICLE_MASS);
    renderParticles();
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

  auto mainWidget = new QWidget(this);
  auto mainLayout = new QGridLayout(mainWidget);
  mainLayout->addWidget(particleView, 0, 0);
  auto rightChartLayout = new QVBoxLayout();
  rightChartLayout->addWidget(energyView);
  rightChartLayout->addWidget(velocityHistView);
  mainLayout->addLayout(rightChartLayout, 0, 2);
  mainLayout->addWidget(statsLabel, 1, 0, 1, 2);
  auto buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(controlBtn);
  buttonLayout->addWidget(stepBtn);
  buttonLayout->addWidget(liftBtn);
  buttonLayout->addWidget(slowDownBtn);
  buttonLayout->addWidget(bringDownBtn);
  buttonLayout->addWidget(reinitBtn);
  buttonLayout->addWidget(exportBtn);
  buttonLayout->addWidget(themeBox);
  mainLayout->addLayout(buttonLayout, 2, 0, 1, 3);
  setCentralWidget(mainWidget);
  setWindowTitle("Particle Box Simulator");

  QShortcut *closeShortcut = new QShortcut(Qt::CTRL | Qt::Key_W, this);
  QObject::connect(closeShortcut, &QShortcut::activated, this, [=]() { close(); });
}

void BoxSimulator::renderParticles() {
  particleSeries->clear();
  for (size_t i = 0; i < PARTICLES; i++)
    *particleSeries << QPointF(positions[i][0], positions[i][1]);
}

void BoxSimulator::updateHistograms() {

  computeVelocityHistogram();
  velocityHistChart->axes(Qt::Horizontal).first()->setRange(velocityHist.min, velocityHist.max);
  velocityHistChart->axes(Qt::Vertical).first()->setRange(0, (double)velocityHist.maxHeight);
  velocityHistSet->remove(0, VELOCITY_HISTOGRAM_BINS);
  for (size_t bin = 0; bin < VELOCITY_HISTOGRAM_BINS; bin++)
    *velocityHistSet << velocityHist.heights[bin];
}

void BoxSimulator::measure() {
  double E_kin = getKineticEnergy();
  double E_pot = getGravitationalPotential();
  double E_pot_LJ = getLJPotential();
  double E_total = E_kin + E_pot + E_pot_LJ;

  _energyMax = std::max(_energyMax, E_total);
  energyChart->axes(Qt::Vertical).first()->setRange(0, log10(_energyMax) + 1.5);

  double measurement = _step / STEPS_PER_MEASUREMENT;
  *kineticEnergySeries << QPointF(measurement, log10(E_kin));
  *potentialEnergySeries << QPointF(measurement, log10(E_pot));
  *LJpotentialEnergySeries << QPointF(measurement, log10(E_pot_LJ));
  *totalEnergySeries << QPointF(measurement, log10(E_total));
  if (measurement > MEASUREMENTS_IN_ENERGY_PLOT)
    energyChart->axes(Qt::Horizontal).first()->setRange((measurement - MEASUREMENTS_IN_ENERGY_PLOT), measurement);
  updateHistograms();

/*  double max_height = 0;
  for (size_t i = 0; i < PARTICLES; i++)
    max_height = std::max(max_height, positions[i][1]);*/

  statsLabel->setText(QString("t = %1 tu,\t E_kin = %2,\t E_pot = %3,\t E_LJ = %4 eu")
                          .arg(QString::number(_step * TAU * ONE_SECOND, 'E', 3), QString::number(E_kin, 'E', 3),
                               QString::number(E_pot, 'E', 3), QString::number(E_pot_LJ, 'E', 3)));
}

void BoxSimulator::step() {
  simulate(STEPS_PER_FRAME);
  renderParticles();
  _step += STEPS_PER_FRAME;

  if (_step % STEPS_PER_MEASUREMENT == 0)
    measure();
}

void BoxSimulator::timerEvent(QTimerEvent *event) { step(); }

void BoxSimulator::setTheme(QChart::ChartTheme theme) {
  energyChart->setTheme(theme);
  velocityHistChart->setTheme(theme);
  particleChart->setTheme(theme);
};
