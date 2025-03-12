#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPushButton;
class QLineEdit;
class QDial;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // GUI elements for the microwave:
    QLineEdit    *m_display;
    QPushButton  *m_clockButton;
    QPushButton  *m_powerButton;
    QPushButton  *m_modeButton;
    QPushButton  *m_defrostButton;
    QDial        *m_dial;
    QPushButton  *m_stopButton;
    QPushButton  *m_startButton;
};

#endif // MAINWINDOW_H
