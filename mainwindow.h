#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qpushbutton.h"
#include <QMainWindow>
#include "subject.h"
#include<QTableWidgetItem>

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
    void on_LecMondayButton_clicked();
    void on_LecTuesdayButton_clicked();
    void on_LecWednesdayButton_clicked();
    void on_LecThursdayButton_clicked();
    void on_LecFridayButton_clicked();
    void on_LecSaturdayButton_clicked();
    void on_LecSundayButton_clicked();
    void on_LabMondayButton_clicked();
    void on_LabTuesdayButton_clicked();
    void on_LabWednesdayButton_clicked();
    void on_LabThursdayButton_clicked();
    void on_LabFridayButton_clicked();
    void on_LabSaturdayButton_clicked();
    void on_LabSundayButton_clicked();
    void on_SemMondayButton_clicked();
    void on_SemTuesdayButton_clicked();
    void on_SemWednesdayButton_clicked();
    void on_SemThursdayButton_clicked();
    void on_SemFridayButton_clicked();
    void on_SemSaturdayButton_clicked();
    void on_SemSundayButton_clicked();


    void on_SubjectAdd_clicked();

    void on_SubjectList_clicked();
    void State();



    void on_AlgorithmButton_clicked();

    void on_Display_activated(const QModelIndex &index);
    void on_prevPageButton_clicked();

    void on_nextPageButton_clicked();

private:
      Algorithm* newAlg = new Algorithm();
     std::vector<std::vector<Slots*>> final;
    int currentPage;
    int maxPage;
    std::vector<std::pair<QString, std::tuple<QTime, QTime, std::vector<Slots::Days>, Slots::Type>>> currentSlots;
    void InsertSlot(QString subject,std::pair<QTime,QTime> times,std::vector<Slots::Days> days,Slots::Type what);
    void updateTable();
    Ui::MainWindow *ui;
    bool LecMondayToggled;
    bool LecTuesdayToggled;
    bool LecWednesdayToggled;
    bool LecThursdayToggled;
    bool LecFridayToggled;
    bool LecSaturdayToggled;
    bool LecSundayToggled;
    bool LabMondayToggled;
    bool LabTuesdayToggled;
    bool LabWednesdayToggled;
    bool LabThursdayToggled;
    bool LabFridayToggled;
    bool LabSaturdayToggled;
    bool LabSundayToggled;
    bool SemMondayToggled;
    bool SemTuesdayToggled;
    bool SemWednesdayToggled;
    bool SemThursdayToggled;
    bool SemFridayToggled;
    bool SemSaturdayToggled;
    bool SemSundayToggled;
    // Create vectors to hold the variables
    std::vector<Slots::Days> LecVars;
    std::vector<Slots::Days> LabVars;
    std::vector<Slots::Days> SemVars;
    void resetUI();
    void Pressed(bool &state, QPushButton &button);
    std::vector<bool> Days;

    std::vector<Subject*> subjects;
};
#endif // MAINWINDOW_H
