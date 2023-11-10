#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qpushbutton.h"
#include <QMainWindow>
#include "subject.h"
#include<QTableWidgetItem>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <fstream>
#include <sstream>
#include <vector>
#include <QString>
#include <Qtime>
#include <QRandomGenerator>
#include <QColor>
#include <QTime>


#include <array>


#include <QDialog>
#include "algorithm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:

    void InsertSlot(QString subject,std::pair<QTime,QTime> times,std::vector<Slots::Days> days,Slots::Type what);

    //void on_SubjectAdd_clicked();

    //void on_SubjectList_clicked();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

    void on_AlgorithmButton_clicked();

    void on_prevPageButton_clicked();

    void on_nextPageButton_clicked();

    void on_PullButton_clicked();

private:
      Algorithm* newAlg = new Algorithm();
     std::vector<std::vector<Slots*>> final;
      Slots::Type parseType(const std::string& type) ;
     std::vector<Slots::Days> parseDays(const std::string& days);
    int currentPage;
    int maxPage;
    std::vector<std::pair<QString, std::tuple<QTime, QTime, std::vector<Slots::Days>, Slots::Type>>> currentSlots;
    void updateTable();
    Ui::MainWindow *ui;
    // Create vectors to hold the variables
    std::vector<Slots::Days> LecVars;
    std::vector<Slots::Days> LabVars;
    std::vector<Slots::Days> SemVars;
    void Pressed(bool &state, QPushButton &button);
    std::vector<bool> Days;

    std::vector<Subject*> subjects;
};
#endif // MAINWINDOW_H
