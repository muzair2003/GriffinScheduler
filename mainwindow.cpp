#include "mainwindow.h"
#include "algorithm.h"
#include "subject.h"
#include "ui_mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the vertical header labels as hours and 10-minute intervals from 8 AM to 8 PM
    QStringList timeLabels;
    for (int hour = 8; hour <= 21; hour++) {
        QString hourLabel = QString("%1:00").arg(hour, 2, 10, QChar('0'));
        timeLabels << hourLabel;

        // Add 10-minute intervals within the hour
        for (int minute = 10; minute < 60; minute += 10) {
            QString minuteLabel = "";
            timeLabels << minuteLabel;
        }
    }
    ui->Display->setRowCount(timeLabels.size()); // Adjust the number of rows accordingly
    ui->Display->setColumnCount(7);

    // Set the horizontal header labels as days of the week starting from Monday
    QStringList dayLabels;
    dayLabels << "Monday" << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday" << "Sunday";
    ui->Display->setHorizontalHeaderLabels(dayLabels);

    ui->Display->setVerticalHeaderLabels(timeLabels);
    ui->Display->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->Display->resize(1000, 1000);
    currentPage = 0;
    ui->prevPageButton->setEnabled(false);
    ui->nextPageButton->setEnabled(false);
    // Allow the window to be maximized
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);

    // Set the initial window size (optional)
    this->resize(800, 600); // Adjust the initial size as needed

}


MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::on_SubjectAdd_clicked()
//{

//QString text = ui->SubjectEnter->text();

//// Create a new Subject object with the entered text
//Subject* newSubject = new Subject(text);

//// Add the newSubject to a container (e.g., a vector)
//subjects.push_back(newSubject); // subjects is a std::vector<Subject> defined in your MainWindow class

//ui->ListSubject->addItem(text);

//// Clear the line edit
//ui->SubjectEnter->clear();

////}

//void MainWindow::on_SubjectList_clicked()
//{
//State();
//QString text = ui->ListSubject->currentText();
//Slots* lecSlot = new Slots(LecVars, Slots::Lecture, ui->LecStartTime->time(), ui->LecEndTime->time(),text);
//Slots* labSlot = new Slots(LabVars, Slots::Lab, ui->LabStartTime->time(), ui->LabEndTime->time(),text);
//Slots* semSlot = new Slots(SemVars, Slots::Seminar, ui->SemStartTime->time(), ui->SemEndTime->time(),text);
//std::vector<Slots*> Section={};
//Section.push_back(lecSlot);
//Section.push_back(labSlot);
//Section.push_back(semSlot);


//for (int i=0;i<subjects.size();i++){
//    if(subjects[i]->get_name()==text){
//        subjects[i]->add_Slot(Section);
//        resetUI();
//        Section.clear();
//    }
//}

//}

void MainWindow::InsertSlot(QString subject, std::pair<QTime, QTime> times, std::vector<Slots::Days> days, Slots::Type what) {
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

    // Calculate the number of 10-minute intervals for the slot
    int numIntervals = ((endTime.hour() * 60 + endTime.minute()) - (startTime.hour() * 60 + startTime.minute())) / 10;

    // Iterate through the days
    for (const auto &day : days) {
        int col = dayColumnMapping[day]; // Get the column index based on the day

        // Iterate through the 10-minute intervals
        for (int i = 0; i < numIntervals; ++i) {
            // Calculate the time label based on the current interval
            QTime currentTime = startTime.addSecs(i * 10 * 60); // 10 minutes in seconds

            // Calculate the row index based on the time
            int rowIndex = (currentTime.hour() - 8) * 6 + (currentTime.minute() / 10);

            // Create the QTableWidgetItem with subject and type
            QString slotText = subject + " - " + TypeToString[what]; // Combine subject and type
            QTableWidgetItem *item = new QTableWidgetItem(slotText);

            // Add the item to the table
            ui->Display->setItem(rowIndex, col, item);
        }
    }


    // Iterate through the days
    for (const auto &day : days) {
        int col = dayColumnMapping[day]; // Get the column index based on the day

        // Initialize variables for merging slots
        QString previousSlotText;
        int spanStartRow = -1;
        int spanRowCount = 0;

        // Iterate through the 10-minute intervals
        for (int i = 0; i < numIntervals; ++i) {
            // Calculate the time label based on the current interval
            QTime currentTime = startTime.addSecs(i * 10 * 60); // 10 minutes in seconds

            // Calculate the row index based on the time
            int rowIndex = (currentTime.hour() - 8) * 6 + (currentTime.minute() / 10);

            // Create the QTableWidgetItem with subject and type
            QString slotText = subject + " - " + TypeToString[what]; // Combine subject and type
            QTableWidgetItem *item = new QTableWidgetItem(slotText);
            // Check if the current slot can be merged with the previous one
            if (previousSlotText == slotText && rowIndex == spanStartRow + spanRowCount) {
                // Extend the span
                spanRowCount++;
            } else {
                // If not merged, reset the spanStartRow and spanRowCount
                if (spanRowCount > 0) {
                    // Merge by setting a span
                    ui->Display->setSpan(spanStartRow, col, spanRowCount, 1);
                }
                spanStartRow = rowIndex;
                spanRowCount = 1;
            }

            // Add the item to the table
            ui->Display->setItem(rowIndex, col, item);

            // Update previous slot text
            previousSlotText = slotText;
        }

        // Merge any remaining slots at the end of the column
        if (spanRowCount > 0) {
            ui->Display->setSpan(spanStartRow, col, spanRowCount, 1);
        }
    }
}

void MainWindow::on_AlgorithmButton_clicked()
{

    final = newAlg->Solve(subjects);
    maxPage = final.size()-1;

    ui->PageNumber->setText("Page " + QString::number(currentPage+1));


    ui->Display->clearContents();


    for(int j=0;j<final[currentPage].size();j++){
        if (final[currentPage][j] != nullptr) {
            InsertSlot(final[currentPage][j]->GetSubject(),final[currentPage][j]->GetTime(),final[currentPage][j]->GetDay(),final[currentPage][j]->GetType());
        }
    }


    if(currentPage==0){
        ui->prevPageButton->setEnabled(false);
    }
    else{
        ui->prevPageButton->setEnabled(true);

    }
    if(currentPage == maxPage){
        ui->nextPageButton->setEnabled(false);
    }
    else{
        ui->nextPageButton->setEnabled(true);
    }


}

void MainWindow::on_prevPageButton_clicked()
{
    for (int row = 0; row < ui->Display->rowCount(); ++row) {
        for (int col = 0; col < ui->Display->columnCount(); ++col) {
            ui->Display->setSpan(row, col, 1, 1); // 1x1 span
        }
    }

    ui->Display->clearContents();
    currentPage--;
    if(currentPage==0){
        ui->prevPageButton->setEnabled(false);
    }
    else{
        ui->prevPageButton->setEnabled(true);

    }
    if(currentPage == maxPage){
        ui->nextPageButton->setEnabled(false);
    }
    else{
        ui->nextPageButton->setEnabled(true);

    }


    ui->PageNumber->setText("Page " + QString::number(currentPage+1));

    for(int j=0;j<final[currentPage].size();j++){
        if (final[currentPage][j] != nullptr) {
            InsertSlot(final[currentPage][j]->GetSubject(),final[currentPage][j]->GetTime(),final[currentPage][j]->GetDay(),final[currentPage][j]->GetType());
        }
    }
}

void MainWindow::on_nextPageButton_clicked()
{
    for (int row = 0; row < ui->Display->rowCount(); ++row) {
        for (int col = 0; col < ui->Display->columnCount(); ++col) {
            ui->Display->setSpan(row, col, 1, 1); // 1x1 span
        }
    }
    ui->Display->clearContents();
    currentPage++;


    if(currentPage==0){
        ui->prevPageButton->setEnabled(false);
    }
    else{
        ui->prevPageButton->setEnabled(true);

    }
    if(currentPage == maxPage){
        ui->nextPageButton->setEnabled(false);
    }
    else{
        ui->nextPageButton->setEnabled(true);

    }


    ui->PageNumber->setText("Page " + QString::number(currentPage+1));

    for(int j=0;j<final[currentPage].size();j++){
        if (final[currentPage][j] != nullptr) {
            InsertSlot(final[currentPage][j]->GetSubject(),final[currentPage][j]->GetTime(),final[currentPage][j]->GetDay(),final[currentPage][j]->GetType());
        }
    }
}

void MainWindow::on_PullButton_clicked()
{
    QString text = ui->PullSubject->text();  // Get the text and trim whitespace

    // Create a new QProcess object
    QProcess *process = new QProcess(this);

    // Connect the finished and errorOccurred signals to slots
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);
    connect(process, &QProcess::errorOccurred,
            this, &MainWindow::onProcessError);

    // Start the process with the command and arguments
    process->start("C:\\Users\\muzai\\AppData\\Local\\Programs\\Python\\Python312\\python.exe", QStringList() << "C:\\Users\\muzai\\OneDrive\\Documents\\Schedule\\webscraper.py" << text);

    QMessageBox::information(this, tr("Subject Pull"), tr("Pulling Data"),QMessageBox::NoButton);
    ui->PullButton->setDisabled(true);
    ui->PullButton->setText("Pulling");

    // Optional: if you want to delete the QProcess object when it's done
    connect(process, &QProcess::finished,
            process, &QObject::deleteLater);
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->PullButton->setDisabled(false);
    ui->PullButton->setText("Pull");


    QString text = ui->PullSubject->text();
    QColor color = QColor::fromRgb(QRandomGenerator::global()->generate());
    Subject* newSubject = new Subject(text,color);

    std::ifstream file("course_sections.csv");
    std::string line;
    std::string currentId;

    // Skip the header
    std::getline(file, line);

    // Initialize empty slots
    Slots* lecSlot = nullptr;
    Slots* labSlot = nullptr;
    Slots* semSlot = nullptr;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id, startTime, endTime, daysStr, typeStr;

        std::getline(ss, id, ',');
        if (currentId.empty()) {
            currentId = id;
        } else if (currentId != id) {
            // If the ID changed, add the slots to the subject and reset them
            std::vector<Slots*> Section = { lecSlot, labSlot, semSlot };
            newSubject->add_Slot(Section);
            // Reset the slots for the next subject
            lecSlot = nullptr;
            labSlot = nullptr;
            semSlot = nullptr;
            currentId = id;
        }

        std::getline(ss, startTime, ',');
        std::getline(ss, endTime, ',');
        std::getline(ss, daysStr, ',');
        std::getline(ss, typeStr);

        QTime start = QTime::fromString(QString::fromStdString(startTime), "h:mm AP");
        QTime end = QTime::fromString(QString::fromStdString(endTime), "h:mm AP");

        if (typeStr == "LEC") {
            lecSlot = new Slots(parseDays(daysStr), Slots::Lecture, start, end, text);
        } else if (typeStr == "LAB") {
            labSlot = new Slots(parseDays(daysStr), Slots::Lab, start, end, text);
        } else if (typeStr == "SEM") {
            semSlot = new Slots(parseDays(daysStr), Slots::Seminar, start, end, text);
        }
    }

    // Add the last set of slots for the last subject
    if (!currentId.empty()) {
        std::vector<Slots*> Section = { lecSlot, labSlot, semSlot };
        newSubject->add_Slot(Section);
    }

    // Push back the new subject to the list of subjects
    subjects.push_back(newSubject);

    file.close();

    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        QMessageBox::information(this, tr("Subject Pull"), tr("Subject was pulled successfully."), QMessageBox::Ok);
    } else {
        QMessageBox::critical(this, tr("Subject Pull"), tr("Error: Subject was not pulled."), QMessageBox::Ok);
    }

}


void MainWindow::onProcessError(QProcess::ProcessError error)
{
    // Handle process error here
    QMessageBox::critical(this, tr("Subject Pull"), tr("An error occurred while trying to pull the subject."), QMessageBox::Ok);
    ui->PullButton->setDisabled(false);
    ui->PullButton->setText("Pull");
}

// Function to convert the days string to a vector of Days enums
std::vector<Slots::Days> MainWindow::parseDays(const std::string& days) {
    std::vector<Slots::Days> daysVector;

    // Use a map to associate the day abbreviations with the enum values
    std::map<std::string, Slots::Days> dayMap = {
        {"M", Slots::Days::Monday},
        {"Tu", Slots::Days::Tuesday},
        {"W", Slots::Days::Wednesday},
        {"Th", Slots::Days::Thursday},
        {"F", Slots::Days::Friday}
    };

    // Iterate through the map and check for each day
    for (const auto& dayPair : dayMap) {
        // Find each day in the days string
        if (days.find(dayPair.first) != std::string::npos) {
            daysVector.push_back(dayPair.second);
        }
    }

    return daysVector;
}



//// Function to convert the type string to Type enum
//Slots::Type MainWindow::parseType(const std::string& type) {
//    if (type == "LEC") return Slots::Type::Lecture;
//    if (type == "LAB") return Slots::Type::Lab;
//    if (type == "SEM") return Slots::Type::Seminar;
//    throw std::runtime_error("Unknown type");
//}


