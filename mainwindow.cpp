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
    QFont fnt;
    // Set the vertical header labels as hours and 10-minute intervals from 8 AM to 8 PM
    QStringList timeLabels;
    for (int hour = 8; hour <= 19; hour++) { // Loop from 8 AM to 8 PM
        // Convert 24-hour time to 12-hour time and append AM or PM
        int displayHour = (hour > 12) ? (hour - 12) : hour;
        QString amPm = (hour < 12) ? "AM" : "PM";
        timeLabels << QString::number(displayHour); // Hour on one row
        timeLabels << amPm;

        // Add 10-minute intervals within the hour
        for (int minute = 10; minute < 50; minute += 10) {
            timeLabels << "";
        }
    }
    fnt.setPointSize(5);

    ui->Display->setFont(fnt);
    ui->Display->setRowCount(timeLabels.size()); // Adjust the number of rows accordingly
    ui->Display->setColumnCount(7);
    QStringList dayLabels;
    dayLabels << "Monday" << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday" << "Sunday";
    ui->Display->setHorizontalHeaderLabels(dayLabels);
    ui->Display->setVerticalHeaderLabels(timeLabels);
    currentPage = 0;
    ui->prevPageButton->setEnabled(false);
    ui->nextPageButton->setEnabled(false);
    ui->Display->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QHeaderView *verticalHeader = ui->Display->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader->setDefaultSectionSize(1);
    QFont font;
    font.setPointSize(15);
    ui->Display->horizontalHeader()->setFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InsertSlot(QString subject, std::pair<QTime, QTime> times, std::vector<Slots::Days> days, Slots::Type what,QString section,QColor subjectColor) {
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

    QFont mergedCellFont;
    mergedCellFont.setPointSize(15);
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
            int rowIndex = ((currentTime.hour() - 8) * 6) + (currentTime.minute() / 10)+1;

            QString slotText = subject + " - " + section + " - " + TypeToString[what];
            QTableWidgetItem *item = ui->Display->item(rowIndex, col);
            if (!item) {
                item = new QTableWidgetItem(slotText);
                ui->Display->setItem(rowIndex, col, item);
            }
            item->setFont(mergedCellFont);
            // Check for the need to merge
            if (previousSlotText == slotText && rowIndex == spanStartRow + spanRowCount) {
                spanRowCount++;
            } else {
                if (spanRowCount > 1 && mergedRowsInColumn[col].count(spanStartRow) == 0) {
                    ui->Display->setSpan(spanStartRow, col, spanRowCount, 1);
                    // Mark these rows as merged
                    for (int r = spanStartRow; r < spanStartRow + spanRowCount; ++r) {
                        QTableWidgetItem* mergedItem = ui->Display->item(r, col);
                        if (mergedItem) {
                            mergedItem->setFont(mergedCellFont);
                        }

                    }
                }
                spanStartRow = rowIndex;
                spanRowCount = 1;
                item->setBackground(subjectColor);
            }

            previousSlotText = slotText;
        }

        // Merge the last span if necessary
        if (spanRowCount > 1 && mergedRowsInColumn[col].count(spanStartRow) == 0) {
            ui->Display->setSpan(spanStartRow, col, spanRowCount, 1);
        }
    }
}
void MainWindow::show_slots()
{

    ui->PageNumber->setText("Page " + QString::number(currentPage+1));
    ui->Display->clearContents();


    for(int j=0;j<final[currentPage].size();j++){
        if (final[currentPage][j] != nullptr) {
            InsertSlot(final[currentPage][j]->GetSubject(),final[currentPage][j]->GetTime(),final[currentPage][j]->GetDay(),final[currentPage][j]->GetType(),final[currentPage][j]->GetSection(),final[currentPage][j]->GetColor());
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
void MainWindow::on_AlgorithmButton_clicked()
{
    currentPage = 0;
    Algorithm* newAlg = new Algorithm();
    final = newAlg->Solve(subjects);
    ui->Display->clearContents();
    if(final.size()>0){
        maxPage = final.size()-1;
        show_slots();
    }
    else{
        QMessageBox::critical(this, tr("Schedule Generate"), tr("There are no possible schedules without conflicts."), QMessageBox::Ok);
    }


}

void MainWindow::on_prevPageButton_clicked()
{
    for (int row = 0; row < ui->Display->rowCount(); ++row) {
        for (int col = 0; col < ui->Display->columnCount(); ++col) {
            ui->Display->setSpan(row, col, 1, 1); // 1x1 span
        }
    }
    currentPage--;
    show_slots();
}

void MainWindow::on_nextPageButton_clicked()
{
    for (int row = 0; row < ui->Display->rowCount(); ++row) {
        for (int col = 0; col < ui->Display->columnCount(); ++col) {
            ui->Display->setSpan(row, col, 1, 1); // 1x1 span
        }
    }
    currentPage++;
    show_slots();
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
        ui->PullButton->setDisabled(true);
        ui->PullButton->setText("Pulling");

        // Optional: if you want to delete the QProcess object when it's done
        connect(process, &QProcess::finished,
                process, &QObject::deleteLater);
    }
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{

    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {


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
            lecSlot = new Slots(parseDays(daysStr), Slots::Lecture, start, end, text,QString::fromStdString(currentId),color);
        } else if (typeStr == "LAB") {
            labSlot = new Slots(parseDays(daysStr), Slots::Lab, start, end, text,QString::fromStdString(currentId),color);
        } else if (typeStr == "SEM") {
            semSlot = new Slots(parseDays(daysStr), Slots::Seminar, start, end, text,QString::fromStdString(currentId),color);
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

    QMessageBox::information(this, tr("Subject Pull"), tr("Subject was pulled successfully."), QMessageBox::Ok);
    } else {
        QMessageBox::critical(this, tr("Subject Pull"), tr("Error: Subject was not pulled."), QMessageBox::Ok);
    }
    ui->PullButton->setDisabled(false);
    ui->PullButton->setText("Pull");
}

void MainWindow::onProcessError(QProcess::ProcessError error)
{
    // Handle process error here
    QMessageBox::critical(this, tr("Subject Pull"), tr("An error occurred while trying to pull the subject."), QMessageBox::Ok);
    ui->PullButton->setDisabled(false);
    ui->PullButton->setText("Pull");
}

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
