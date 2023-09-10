#include "mainwindow.h"
#include "algorithm.h"
#include "subject.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     LecMondayToggled =false;
     LecTuesdayToggled=false;
     LecWednesdayToggled=false;
     LecThursdayToggled=false;
     LecFridayToggled=false;
     LecSaturdayToggled=false;
     LecSundayToggled=false;
      LabMondayToggled=false;
      LabTuesdayToggled=false;
      LabWednesdayToggled=false;
      LabThursdayToggled=false;
      LabFridayToggled=false;
      LabSaturdayToggled=false;
      LabSundayToggled=false;
      SemMondayToggled=false;
      SemTuesdayToggled=false;
      SemWednesdayToggled=false;
      SemThursdayToggled=false;
      SemFridayToggled=false;
      SemSaturdayToggled=false;
      SemSundayToggled=false;


      // Create a QTableWidget with 13 rows and 7 columns
      ui->Display->setRowCount(13);
      ui->Display->setColumnCount(7);

      // Set the horizontal header labels as days of the week starting from Monday
      QStringList dayLabels;
      dayLabels << "Monday" << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday" << "Sunday";
      ui->Display->setHorizontalHeaderLabels(dayLabels);

      // Set the vertical header labels as times from 8 AM to 8 PM
      QStringList timeLabels;
      for (int hour = 8; hour <= 20; hour++) {
          QString timeLabel = QString("%1:00").arg(hour);
          timeLabels << timeLabel;
      }
      ui->Display->setVerticalHeaderLabels(timeLabels);



      currentPage = 0;



ui->prevPageButton->setEnabled(false);

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Pressed(bool &state,QPushButton &button){


// Check the toggle state.
if (state) {
    // If it's true (button was previously toggled), reset the button style to default.
    button.setStyleSheet("");
} else {
    // If it's false (button was not previously toggled), set the background color to green.
    button.setStyleSheet("background-color: green;");
}
// Toggle the state variable.
state = !state;
}



void MainWindow::on_SubjectAdd_clicked()
{

QString text = ui->SubjectEnter->text();

// Create a new Subject object with the entered text
Subject* newSubject = new Subject(text);

// Add the newSubject to a container (e.g., a vector)
subjects.push_back(newSubject); // subjects is a std::vector<Subject> defined in your MainWindow class

ui->ListSubject->addItem(text);

// Clear the line edit
ui->SubjectEnter->clear();

}


void MainWindow::on_SubjectList_clicked()
{
State();
QString text = ui->ListSubject->currentText();
Slots* lecSlot = new Slots(LecVars, Slots::Lecture, ui->LecStartTime->time(), ui->LecEndTime->time(),text);
Slots* labSlot = new Slots(LabVars, Slots::Lab, ui->LabStartTime->time(), ui->LabEndTime->time(),text);
Slots* semSlot = new Slots(SemVars, Slots::Seminar, ui->SemStartTime->time(), ui->SemEndTime->time(),text);
std::vector<Slots*> Section={};
Section.push_back(lecSlot);
Section.push_back(labSlot);
Section.push_back(semSlot);


for (int i=0;i<subjects.size();i++){
    if(subjects[i]->get_name()==text){
        subjects[i]->add_Slot(Section);
        resetUI();
        Section.clear();
    }
}
}


void MainWindow::InsertSlot(QString subject, std::pair<QTime, QTime> times, std::vector<Slots::Days> days, Slots::Type what) {

// Define your day to column mapping
std::map<Slots::Days, int> dayColumnMapping = {
    {Slots::Monday, 0},
    {Slots::Tuesday, 1},
    {Slots::Wednesday, 2},
    {Slots::Thursday, 3},
    {Slots::Friday, 4},
    {Slots::Saturday, 5},
    {Slots::Sunday, 6}
};

std::map<Slots::Type, QString> TypeToString = {
    {Slots::Lecture, "Lecture"},
    {Slots::Lab, "Lab"},
    {Slots::Seminar, "Seminar"}
};

QTime startTime = times.first;
QTime endTime = times.second;

// Iterate through the days
for (const auto &day : days) {
    int col = dayColumnMapping[day]; // Get the column index based on the day

    // Calculate the number of hours for the slot
    int numHours = endTime.hour() - startTime.hour(); // Corrected calculation

    // Add the subject and type to the slot and create the QTableWidgetItem
    for (int i = 0; i < numHours; ++i) {
        QString slotText = subject + " - " + TypeToString[what]; // Combine subject and type
        QTableWidgetItem *item = new QTableWidgetItem(slotText);
        ui->Display->setItem(startTime.hour() + i - 8, col, item); // Adjusted row index
    }
}
}



void MainWindow::on_AlgorithmButton_clicked()
{

final = newAlg->Solve(subjects);
maxPage = final.size()-1;

if(currentPage == maxPage){
    ui->nextPageButton->setEnabled(false);
}
ui->Display->clearContents();


    for(int j=0;j<final[currentPage].size();j++){
        InsertSlot(final[currentPage][j]->GetSubject(),final[currentPage][j]->GetTime(),final[currentPage][j]->GetDay(),final[currentPage][j]->GetType());
    }

}



void MainWindow::on_prevPageButton_clicked()
{
        ui->Display->clearContents();



    for(int j=0;j<final[currentPage].size();j++){
        InsertSlot(final[currentPage][j]->GetSubject(),final[currentPage][j]->GetTime(),final[currentPage][j]->GetDay(),final[currentPage][j]->GetType());
    }

    if(currentPage==0){
        ui->prevPageButton->setEnabled(false);
    }
    currentPage--;
}


void MainWindow::on_nextPageButton_clicked()
{
    ui->Display->clearContents();


    for(int j=0;j<final[currentPage].size();j++){
        InsertSlot(final[currentPage][j]->GetSubject(),final[currentPage][j]->GetTime(),final[currentPage][j]->GetDay(),final[currentPage][j]->GetType());
    }

    if(currentPage == maxPage){
        ui->nextPageButton->setEnabled(false);
    }
    currentPage++;

}



void MainWindow::resetUI() {
// Reset the QTimes to a default value (e.g., 00:00:00)
QTime defaultTime(0, 0, 0);
ui->LecStartTime->setTime(defaultTime);
ui->LabStartTime->setTime(defaultTime);
ui->SemStartTime->setTime(defaultTime);
ui->LecEndTime->setTime(defaultTime);
ui->LabEndTime->setTime(defaultTime);
ui->SemEndTime->setTime(defaultTime);

// Reset the button colors to default
ui->LecMondayButton->setStyleSheet("");
ui->LecTuesdayButton->setStyleSheet("");
ui->LecWednesdayButton->setStyleSheet("");
ui->LecThursdayButton->setStyleSheet("");
ui->LecFridayButton->setStyleSheet("");
ui->LecSaturdayButton->setStyleSheet("");
ui->LecSundayButton->setStyleSheet("");

ui->LabMondayButton->setStyleSheet("");
ui->LabTuesdayButton->setStyleSheet("");
ui->LabWednesdayButton->setStyleSheet("");
ui->LabThursdayButton->setStyleSheet("");
ui->LabFridayButton->setStyleSheet("");
ui->LabSaturdayButton->setStyleSheet("");
ui->LabSundayButton->setStyleSheet("");

ui->SemMondayButton->setStyleSheet("");
ui->SemTuesdayButton->setStyleSheet("");
ui->SemWednesdayButton->setStyleSheet("");
ui->SemThursdayButton->setStyleSheet("");
ui->SemFridayButton->setStyleSheet("");
ui->SemSaturdayButton->setStyleSheet("");
ui->SemSundayButton->setStyleSheet("");
LecMondayToggled =false;
LecTuesdayToggled=false;
LecWednesdayToggled=false;
LecThursdayToggled=false;
LecFridayToggled=false;
LecSaturdayToggled=false;
LecSundayToggled=false;
LabMondayToggled=false;
LabTuesdayToggled=false;
LabWednesdayToggled=false;
LabThursdayToggled=false;
LabFridayToggled=false;
LabSaturdayToggled=false;
LabSundayToggled=false;
SemMondayToggled=false;
SemTuesdayToggled=false;
SemWednesdayToggled=false;
SemThursdayToggled=false;
SemFridayToggled=false;
SemSaturdayToggled=false;
SemSundayToggled=false;
}


void MainWindow::State() {
LecVars.clear();  // Clear the vector to start with a clean slate

if (LecMondayToggled) {
    LecVars.push_back(Slots::Monday);
}
if (LecTuesdayToggled) {
    LecVars.push_back(Slots::Tuesday);
}
if (LecWednesdayToggled) {
    LecVars.push_back(Slots::Wednesday);
}
if (LecThursdayToggled) {
    LecVars.push_back(Slots::Thursday);
}
if (LecFridayToggled) {
    LecVars.push_back(Slots::Friday);
}
if (LecSaturdayToggled) {
    LecVars.push_back(Slots::Saturday);
}
if (LecSundayToggled) {
    LecVars.push_back(Slots::Sunday);
}

// Clear and update LabVars
LabVars.clear();
if (LabMondayToggled) {
    LabVars.push_back(Slots::Monday);
}
if (LabTuesdayToggled) {
    LabVars.push_back(Slots::Tuesday);
}
if (LabWednesdayToggled) {
    LabVars.push_back(Slots::Wednesday);
}
if (LabThursdayToggled) {
    LabVars.push_back(Slots::Thursday);
}
if (LabFridayToggled) {
    LabVars.push_back(Slots::Friday);
}
if (LabSaturdayToggled) {
    LabVars.push_back(Slots::Saturday);
}
if (LabSundayToggled) {
    LabVars.push_back(Slots::Sunday);
}

// Clear and update SemVars
SemVars.clear();
if (SemMondayToggled) {
    SemVars.push_back(Slots::Monday);
}
if (SemTuesdayToggled) {
    SemVars.push_back(Slots::Tuesday);
}
if (SemWednesdayToggled) {
    SemVars.push_back(Slots::Wednesday);
}
if (SemThursdayToggled) {
    SemVars.push_back(Slots::Thursday);
}
if (SemFridayToggled) {
    SemVars.push_back(Slots::Friday);
}
if (SemSaturdayToggled) {
    SemVars.push_back(Slots::Saturday);
}
if (SemSundayToggled) {
    SemVars.push_back(Slots::Sunday);
}
}
void MainWindow::on_LecMondayButton_clicked()
{
    Pressed(LecMondayToggled,*ui->LecMondayButton);
}

void MainWindow::on_LecTuesdayButton_clicked()
{
    Pressed(LecTuesdayToggled,*ui->LecTuesdayButton);
}

void MainWindow::on_LecWednesdayButton_clicked()
{
    Pressed(LecWednesdayToggled,*ui->LecWednesdayButton);
}

void MainWindow::on_LecThursdayButton_clicked()
{
    Pressed(LecThursdayToggled,*ui->LecThursdayButton);
}
void MainWindow::on_LecFridayButton_clicked()
{
    Pressed(LecFridayToggled,*ui->LecFridayButton);
}

void MainWindow::on_LecSaturdayButton_clicked()
{
    Pressed(LecSaturdayToggled,*ui->LecSaturdayButton);
}

void MainWindow::on_LecSundayButton_clicked()
{
    Pressed(LecSundayToggled,*ui->LecSundayButton);
}

void MainWindow::on_LabMondayButton_clicked()
{
    Pressed(LabMondayToggled,*ui->LabMondayButton);
}

void MainWindow::on_LabTuesdayButton_clicked()
{
    Pressed(LabTuesdayToggled,*ui->LabTuesdayButton);
}

void MainWindow::on_LabWednesdayButton_clicked()
{
    Pressed(LabWednesdayToggled,*ui->LabWednesdayButton);
}

void MainWindow::on_LabThursdayButton_clicked()
{
    Pressed(LabThursdayToggled,*ui->LabThursdayButton);
}

void MainWindow::on_LabFridayButton_clicked()
{
    Pressed(LabFridayToggled,*ui->LabFridayButton);
}

void MainWindow::on_LabSaturdayButton_clicked()
{
    Pressed(LabSaturdayToggled,*ui->LabSaturdayButton);
}

void MainWindow::on_LabSundayButton_clicked()
{
    Pressed(LabSundayToggled,*ui->LabSundayButton);
}

void MainWindow::on_SemMondayButton_clicked()
{
    Pressed(SemMondayToggled, *ui->SemMondayButton);
}

void MainWindow::on_SemTuesdayButton_clicked()
{
    Pressed(SemTuesdayToggled, *ui->SemTuesdayButton);
}

void MainWindow::on_SemWednesdayButton_clicked()
{
    Pressed(SemWednesdayToggled, *ui->SemWednesdayButton);
}

void MainWindow::on_SemThursdayButton_clicked()
{
    Pressed(SemThursdayToggled, *ui->SemThursdayButton);
}

void MainWindow::on_SemFridayButton_clicked()
{
    Pressed(SemFridayToggled, *ui->SemFridayButton);
}

void MainWindow::on_SemSaturdayButton_clicked()
{
    Pressed(SemSaturdayToggled, *ui->SemSaturdayButton);
}

void MainWindow::on_SemSundayButton_clicked()
{
    Pressed(SemSundayToggled, *ui->SemSundayButton);
}




void MainWindow::on_Display_activated(const QModelIndex &index)
{

}



