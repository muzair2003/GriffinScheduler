#include "mainwindow.h"
#include "algorithm.h"
#include "subject.h"
#include "ui_mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <set>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the vertical header labels as hours and 10-minute intervals from 8 AM to 8 PM
    QStringList timeLabels;
    for (int hour = 8; hour <= 20; hour++) { // Loop from 8 AM to 8 PM
        // Convert 24-hour time to 12-hour time and append AM or PM
        int displayHour = (hour > 12) ? (hour - 12) : hour;
        QString amPm = (hour < 12) ? "AM" : "PM";
        QString hourLabel = QString("%1:00 %2").arg(displayHour, 2, 10, QChar('0')).arg(amPm);
        timeLabels << hourLabel;

        // Add 10-minute intervals within the hour
        for (int minute = 10; minute < 60; minute += 10) {
            QString minuteLabel = QString("");
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
    //ui->Display->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    currentPage = 0;
    ui->prevPageButton->setEnabled(false);
    ui->nextPageButton->setEnabled(false);
    // Allow the window to be maximized
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);

    // Set the initial window size (optional)
    //    int availableHeight = ui->Display->height(); // Get the available height for the QTableWidget
    //    int numRows = ui->Display->rowCount(); // Get the number of rows
    //    int rowHeight = availableHeight / numRows;
    //    int totalWidth = ui->Display->width();  // Get the total width of the QTableWidget
    //    int numColumns = ui->Display->columnCount();  // Get the number of columns
    //    int columnWidth = totalWidth / numColumns;  // Calculate the width for each column

    //    for (int i = 0; i < numRows; ++i) {
    //        ui->Display->setRowHeight(i, rowHeight); // Set the height for each row
    //    }
    //    for (int i = 0; i < numColumns; ++i) {
    //        ui->Display->setColumnWidth(i, columnWidth);
    //    }


    // this->showFullScreen();
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
    // Mapping from days to column indices
    std::map<Slots::Days, int> dayColumnMapping = {
        {Slots::Monday, 0}, {Slots::Tuesday, 1}, {Slots::Wednesday, 2},
        {Slots::Thursday, 3}, {Slots::Friday, 4}, {Slots::Saturday, 5}, {Slots::Sunday, 6}
    };

    // Mapping from slot types to string representations
    std::map<Slots::Type, QString> TypeToString = {
        {Slots::Lecture, "Lecture"}, {Slots::Lab, "Lab"}, {Slots::Seminar, "Seminar"}
    };

    QTime startTime = times.first;
    QTime endTime = times.second;

    // Validate time range
    if (startTime >= endTime) {
        qDebug() << "Invalid time range";
        return;
    }

    // Calculate the number of 10-minute intervals for the slot, rounding up
    int numIntervals = (endTime.hour() * 60 + endTime.minute() - startTime.hour() * 60 - startTime.minute() + 9) / 10;

    // Check if numIntervals is zero or negative
    if (numIntervals <= 0) {
        qDebug() << "No intervals to display";
        return;
    }

    // Track merged rows for each column to avoid overlapping spans
    std::map<int, std::set<int>> mergedRowsInColumn;

    for (const auto& day : days) {
        // Validate day
        if (dayColumnMapping.find(day) == dayColumnMapping.end()) {
            qDebug() << "Invalid day";
            continue;
        }

        int col = dayColumnMapping[day];
        QString previousSlotText;
        int spanStartRow = -1;
        int spanRowCount = 0;

        for (int i = 0; i < numIntervals; ++i) {
            QTime currentTime = startTime.addSecs(i * 600); // 10 minutes in seconds
            int rowIndex = (currentTime.hour() - 8) * 6 + (currentTime.minute() / 10);

            QString slotText = subject + " - " + TypeToString[what];
            QTableWidgetItem *item = ui->Display->item(rowIndex, col);
            if (!item) {
                item = new QTableWidgetItem(slotText);
                ui->Display->setItem(rowIndex, col, item);
            }

            // Check for the need to merge
            if (previousSlotText == slotText && rowIndex == spanStartRow + spanRowCount) {
                spanRowCount++;
            } else {
                if (spanRowCount > 1 && mergedRowsInColumn[col].count(spanStartRow) == 0) {
                    ui->Display->setSpan(spanStartRow, col, spanRowCount, 1);
                    // Mark these rows as merged
                    for (int r = spanStartRow; r < spanStartRow + spanRowCount; ++r) {
                        mergedRowsInColumn[col].insert(r);
                    }
                }
                spanStartRow = rowIndex;
                spanRowCount = 1;
            }

            previousSlotText = slotText;
        }

        // Merge the last span if necessary
        if (spanRowCount > 1 && mergedRowsInColumn[col].count(spanStartRow) == 0) {
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
    text.replace(" ", "_");
    std::string filename = text.toStdString() + ".csv";

    if (std::filesystem::exists(filename)) {
        // The file already exists, so you can go straight to processing the file
        // Call your processing finished functions here
        onProcessFinished(0, QProcess::NormalExit); // Example call, adjust according to your actual function
    } else {
        // Create a new QProcess object
        QProcess *process = new QProcess(this);

        // Connect the finished and errorOccurred signals to slots
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::onProcessFinished);
        connect(process, &QProcess::errorOccurred,
                this, &MainWindow::onProcessError);
        text.replace("_", " ");
        // Start the process with the command and arguments
        QString appPath = QCoreApplication::applicationDirPath();
        QString scriptPath = appPath + "/webscraper.py";  // Assuming the script is in the same directory as the executable
        process->start("python", QStringList() << scriptPath << text);

        QMessageBox::information(this, tr("Subject Pull"), tr("Pulling Data"),QMessageBox::NoButton);
        ui->PullButton->setDisabled(true);
        ui->PullButton->setText("Pulling");

        // Optional: if you want to delete the QProcess object when it's done
        connect(process, &QProcess::finished,
                process, &QObject::deleteLater);
    }
}


void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->PullButton->setDisabled(false);
    ui->PullButton->setText("Pull");


    QString text = ui->PullSubject->text();
    QColor color = QColor::fromRgb(QRandomGenerator::global()->generate());
    Subject* newSubject = new Subject(text,color);

    text.replace(" ", "_");
    std::string filename = text.toStdString() + ".csv";  // Convert to std::string and append .csv
    std::ifstream file(filename);
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
        text.replace("_", " ");
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

