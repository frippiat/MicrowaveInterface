#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QDial>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStateMachine>
#include <QState>
#include <QSignalTransition>
#include <QTimer>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cookingTimer(new QTimer(this))
    , m_idleClockTimer(new QTimer(this))
    , m_remainingTime(0)
    , m_clockTime(QTime::currentTime())
    , m_tempHour(0)
    , m_currentSetting(None)
{
    ui->setupUi(this);

    //Widgets
        m_display       = new QLineEdit(this);
        m_clockButton   = new QPushButton("Clock", this);
        m_powerButton   = new QPushButton("Power", this);
        m_modeButton    = new QPushButton("Mode", this);
        m_defrostButton = new QPushButton("Defrost", this);
        m_dial          = new QDial(this);
        m_stopButton    = new QPushButton("Stop", this);
        m_startButton   = new QPushButton("Start", this);

        //Display
        m_display->setReadOnly(true);
        m_display->setAlignment(Qt::AlignCenter);
        m_display->setText(m_clockTime.toString("hh:mm"));
        m_display->setStyleSheet("font-size: 28px; font-weight: bold; color: #333;");

        // buttons style
        QString btnStyle = "font-size: 20px; padding: 10px 20px; "
                           "background-color: #f0f0f0; border: 2px solid #555; border-radius: 8px;";
        m_clockButton->setStyleSheet(btnStyle);
        m_powerButton->setStyleSheet(btnStyle);
        m_modeButton->setStyleSheet(btnStyle);
        m_defrostButton->setStyleSheet(btnStyle);
        m_startButton->setStyleSheet(btnStyle);
        m_stopButton->setStyleSheet(btnStyle);

        //dial style
        m_dial->setStyleSheet("QDial { background-color: #ddd; }");

        //central widget layout
        QVBoxLayout *mainLayout = new QVBoxLayout;
        ui->centralwidget->setLayout(mainLayout);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);
        mainLayout->addWidget(m_display);
        QGridLayout *gridLayout = new QGridLayout;
        gridLayout->setHorizontalSpacing(10);
        gridLayout->setVerticalSpacing(10);
        mainLayout->addLayout(gridLayout);
    gridLayout->addWidget(m_clockButton, 0, 0);
    gridLayout->addWidget(m_powerButton, 0, 1);
    gridLayout->addWidget(m_modeButton,    1, 0);
    gridLayout->addWidget(m_defrostButton, 1, 1);
    gridLayout->addWidget(m_dial, 2, 0, 1, 2, Qt::AlignHCenter);
    gridLayout->addWidget(m_stopButton,  3, 0);
    gridLayout->addWidget(m_startButton, 3, 1);

    //dial signal valueChanged
    connect(m_dial, &QDial::valueChanged, this, [this](int value) {
        switch(m_currentSetting) {
            case ClockHours:
                m_tempHour = value;
                m_display->setText("Set Hour: " + QString::number(value));
                break;
            case ClockMinutes:
                m_display->setText("Set Minute: " + QString::number(value));
                break;
            case Power:
                m_display->setText("Set Power: " + QString::number(value) + "%");
                break;
            case Duration:
                m_display->setText("Set Duration: " + QString::number(value) + " sec");
                break;
            case Mode:
                m_display->setText("Set Mode: " + QString::number(value));
                break;
            case Weight:
                m_display->setText("Set Weight: " + QString::number(value*10) + " g");
                break;
            default:
                break;
        }
    });

    //Idle clock timer
    m_idleClockTimer->setInterval(1000);
    connect(m_idleClockTimer, &QTimer::timeout, this, [this]() {
        // Advance the clock by one second
        m_clockTime = m_clockTime.addSecs(1);
        m_display->setText(m_clockTime.toString("hh:mm"));
    });

    setupStateMachine();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStateMachine()
{
    //cooking timer
    m_cookingTimer->setInterval(1000);
    m_cookingTimer->setSingleShot(false);

    QStateMachine *machine = new QStateMachine(this);

    //Idle State
    QState *idleState = new QState();
    connect(idleState, &QState::entered, this, [this]() {
        m_display->setText(m_clockTime.toString("hh:mm"));
        m_currentSetting = None;
        m_clockButton->setEnabled(true);
        m_powerButton->setEnabled(true);
        m_modeButton->setEnabled(true);
        m_defrostButton->setEnabled(true);
        m_startButton->setEnabled(true);
        m_dial->setEnabled(false);
        m_idleClockTimer->start();
    });
    connect(idleState, &QState::exited, this, [this]() {
        m_idleClockTimer->stop();
    });

    //Clock Sequence States
    QState *clockSetHours = new QState();
    connect(clockSetHours, &QState::entered, this, [this]() {
        m_currentSetting = ClockHours;
        m_dial->setRange(0, 23);
        m_dial->setValue(m_clockTime.hour());
        m_tempHour = m_clockTime.hour();
        m_dial->setEnabled(true);
        m_display->setText("Set Hour: " + QString::number(m_dial->value()));
        m_clockButton->setEnabled(true);
        m_powerButton->setEnabled(false);
        m_modeButton->setEnabled(false);
        m_defrostButton->setEnabled(false);
        m_startButton->setEnabled(false);
    });
    QState *clockSetMinutes = new QState();
    connect(clockSetMinutes, &QState::entered, this, [this]() {
        m_currentSetting = ClockMinutes;
        m_dial->setRange(0, 59);
        m_dial->setValue(m_clockTime.minute());
        m_dial->setEnabled(true);
        m_display->setText("Set Minute: " + QString::number(m_dial->value()));
        m_clockButton->setEnabled(true);
    });
    // update the clock time When leaving clockSetMinutes
    clockSetMinutes->addTransition(m_clockButton, SIGNAL(clicked()), idleState);
    QObject::connect(clockSetMinutes, &QState::exited, this, [this]() {
        //Update m_clockTime
        m_clockTime = QTime(m_tempHour, m_dial->value(), 0);
    });
    clockSetHours->addTransition(m_clockButton, SIGNAL(clicked()), clockSetMinutes);
    clockSetHours->addTransition(m_stopButton, SIGNAL(clicked()), idleState);
    clockSetMinutes->addTransition(m_stopButton, SIGNAL(clicked()), idleState);

    //Power Sequence State
    QState *setupSetPower = new QState();
    connect(setupSetPower, &QState::entered, this, [this]() {
        m_currentSetting = Power;
        m_dial->setRange(0, 100);
        m_dial->setEnabled(true);
        m_dial->setValue(m_dial->value());  //

        m_display->setText("Set Power: " + QString::number(m_dial->value()) + "%");
        m_powerButton->setEnabled(true);
        m_clockButton->setEnabled(false);
        m_modeButton->setEnabled(false);
        m_defrostButton->setEnabled(false);
        m_startButton->setEnabled(false);
    });
    QState *setupSetDuration = new QState();
    connect(setupSetDuration, &QState::entered, this, [this]() {
        m_currentSetting = Duration;
        m_dial->setRange(0, 300);
        m_dial->setEnabled(true);
        m_display->setText("Set Duration: " + QString::number(m_dial->value()) + " sec");
        m_startButton->setEnabled(true);
        m_clockButton->setEnabled(false);
        m_powerButton->setEnabled(false);
        m_modeButton->setEnabled(false);
        m_defrostButton->setEnabled(false);
    });
    setupSetPower->addTransition(m_powerButton, SIGNAL(clicked()), setupSetDuration);
    setupSetPower->addTransition(m_stopButton, SIGNAL(clicked()), idleState);
    setupSetDuration->addTransition(m_stopButton, SIGNAL(clicked()), idleState);
    setupSetDuration->addTransition(m_startButton, SIGNAL(clicked()), nullptr); // to be set to cooking

    //Mode Sequence State
    QState *setupSetMode = new QState();
    connect(setupSetMode, &QState::entered, this, [this]() {
        m_currentSetting = Mode;
        m_dial->setRange(0, 2);
        m_dial->setEnabled(true);
        m_display->setText("Set Mode: " + QString::number(m_dial->value()));
        m_modeButton->setEnabled(true);
        m_clockButton->setEnabled(false);
        m_powerButton->setEnabled(false);
        m_defrostButton->setEnabled(false);
        m_startButton->setEnabled(false);
    });
    setupSetMode->addTransition(m_modeButton, SIGNAL(clicked()), setupSetDuration);
    setupSetMode->addTransition(m_stopButton, SIGNAL(clicked()), idleState);

    // Defrost Sequence State
    QState *setupSetWeight = new QState();
    connect(setupSetWeight, &QState::entered, this, [this]() {
        m_currentSetting = Weight;
        m_dial->setRange(0, 400);
        m_dial->setEnabled(true);
        m_display->setText("Set Weight: " + QString::number(10*m_dial->value()) + " g");
        m_defrostButton->setEnabled(true);
        m_clockButton->setEnabled(false);
        m_powerButton->setEnabled(false);
        m_modeButton->setEnabled(false);
        m_startButton->setEnabled(false);
    });
    setupSetWeight->addTransition(m_defrostButton, SIGNAL(clicked()), setupSetDuration); // to be set to cooking
    setupSetWeight->addTransition(m_stopButton, SIGNAL(clicked()), idleState);

    // Cooking State
    QState *cookingState = new QState();
    connect(cookingState, &QState::entered, this, [this]() {
        m_currentSetting = None;
        if (m_dial->isEnabled()) {
                 m_remainingTime = m_dial->value() * 1000;
            }
        else {
                 m_remainingTime = 60000; // default to 60 seconds.
            }
        m_display->setText("Cooking: " + QString::number(m_remainingTime/1000) + " sec remaining");
        m_dial->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_startButton->setEnabled(true);
        m_cookingTimer->start();
    });
    cookingState->addTransition(m_stopButton, SIGNAL(clicked()), idleState);
    connect(cookingState, &QState::exited, this, [this]() {
        m_cookingTimer->stop();
    });

    connect(m_cookingTimer, &QTimer::timeout, this, [this]() {
        m_remainingTime -= 1000;
        if(m_remainingTime <= 0) {
            m_cookingTimer->stop();
            emit m_stopButton->clicked();
        } else {
            m_display->setText("Cooking: " + QString::number(m_remainingTime/1000) + " sec remaining");
        }
    });
    connect(m_startButton, &QPushButton::clicked, this, [this]() {
        if(m_cookingTimer->isActive()){
            m_remainingTime += 60000;
            m_display->setText("Cooking: " + QString::number(m_remainingTime/1000) + " sec remaining");
        }
    });

    // Transitions from Idle
    idleState->addTransition(m_clockButton, SIGNAL(clicked()), clockSetHours);
    idleState->addTransition(m_powerButton, SIGNAL(clicked()), setupSetPower);
    idleState->addTransition(m_modeButton, SIGNAL(clicked()), setupSetMode);
    idleState->addTransition(m_defrostButton, SIGNAL(clicked()), setupSetWeight);
    idleState->addTransition(m_startButton, SIGNAL(clicked()), cookingState);

    // transitions from setup states to Cooking State
    setupSetDuration->addTransition(m_startButton, SIGNAL(clicked()), cookingState);
    setupSetWeight->addTransition(m_startButton, SIGNAL(clicked()), cookingState);

    machine->addState(idleState);
    machine->addState(clockSetHours);
    machine->addState(clockSetMinutes);
    machine->addState(setupSetPower);
    machine->addState(setupSetMode);
    machine->addState(setupSetWeight);
    machine->addState(setupSetDuration);
    machine->addState(cookingState);

    machine->setInitialState(idleState);
    machine->start();
}

