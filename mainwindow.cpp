#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QDial>
#include <QVBoxLayout>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1) Create the widgets
    m_display       = new QLineEdit(this);
    m_clockButton   = new QPushButton("Clock", this);
    m_powerButton   = new QPushButton("Power", this);
    m_modeButton    = new QPushButton("Mode", this);
    m_defrostButton = new QPushButton("Defrost", this);
    m_dial          = new QDial(this);
    m_stopButton    = new QPushButton("Stop", this);
    m_startButton   = new QPushButton("Start", this);

    // Optional: Some initial settings
    m_display->setReadOnly(true);
    m_display->setText("Ready");
    m_display->setAlignment(Qt::AlignCenter);
    // For the dial, you might want some range or initial value:
    // m_dial->setRange(0, 100);
    // m_dial->setValue(50);

    // 2) Create and set the layout for the central widget
    QVBoxLayout *mainLayout = new QVBoxLayout;
    ui->centralwidget->setLayout(mainLayout);

    // 3) Add the display at the top
    mainLayout->addWidget(m_display);

    // 4) Create a grid layout to arrange the buttons and dial
    QGridLayout *gridLayout = new QGridLayout;
    mainLayout->addLayout(gridLayout);

    // Row 0: Clock, Power
    gridLayout->addWidget(m_clockButton, 0, 0);
    gridLayout->addWidget(m_powerButton, 0, 1);

    // Row 1: Mode, Defrost
    gridLayout->addWidget(m_modeButton,    1, 0);
    gridLayout->addWidget(m_defrostButton, 1, 1);

    // Row 2: just the Dial, centered across two columns
    gridLayout->addWidget(m_dial, 2, 0, 1, 2, Qt::AlignHCenter);

    // Row 3: Stop, Start
    gridLayout->addWidget(m_stopButton,  3, 0);
    gridLayout->addWidget(m_startButton, 3, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
