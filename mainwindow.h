#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPushButton;
class QLineEdit;
class QDial;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupStateMachine();

    Ui::MainWindow *ui;

    // GUI microwave:
    QLineEdit    *m_display;
    QPushButton  *m_clockButton;
    QPushButton  *m_powerButton;
    QPushButton  *m_modeButton;
    QPushButton  *m_defrostButton;
    QDial        *m_dial;
    QPushButton  *m_stopButton;
    QPushButton  *m_startButton;

    QTimer       *m_cookingTimer;
    QTimer       *m_idleClockTimer;  // for updating the clock when idle
    int           m_remainingTime;   // in ms

    // For the clock time
    QTime         m_clockTime;
    int           m_tempHour;


    enum SettingType {
        None,
        ClockHours,
        ClockMinutes,
        Power,
        Duration,
        Mode,
        Weight
    };
    SettingType m_currentSetting;
};

#endif // MAINWINDOW_H
