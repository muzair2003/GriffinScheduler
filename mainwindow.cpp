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
    QFont cellFont;
    cellFont.setPointSize(1);
    ui->Display->verticalHeader()->setFont(cellFont);
    QHeaderView *verticalHeader = ui->Display->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader->setDefaultSectionSize(1);


    QStringList timeLabels;
    for (int hour = 8; hour <= 19; hour++) {
        int displayHour = (hour > 12) ? (hour - 12) : hour;
        QString amPm = (hour < 12) ? "AM" : "PM";
        timeLabels << QString::number(displayHour) + amPm; // Newline character added

        for (int minute = 10; minute < 60; minute += 10) {
            timeLabels << "";
        }
    }

    ui->Display->setRowCount(timeLabels.size());
    ui->Display->setColumnCount(8); // 7 days + 1 for time column
    QStringList dayLabels = {"Time", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    ui->Display->setHorizontalHeaderLabels(dayLabels);
    int mergeSize = 6; // Number of rows to merge
    for (int row = 0; row < 72; row += mergeSize) {
        // Merge cells from 'row' to 'row + mergeSize - 1' in the first column
        ui->Display->setSpan(row, 0, mergeSize, 1);
    }
    // Populate the first column with time labels
    for (int i = 0; i < timeLabels.size(); ++i) {
        QTableWidgetItem *newItem = new QTableWidgetItem(timeLabels[i]);
        ui->Display->setItem(i, 0, newItem);
    }

    currentPage = 0;
    ui->prevPageButton->setEnabled(false);
    ui->nextPageButton->setEnabled(false);

    // Set the interactive mode for all columns
    ui->Display->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // Set the width of the time column (column 0) to 5
    ui->Display->setColumnWidth(0, 50);

    // Set the stretch mode for the rest of the columns
    for (int i = 1; i < ui->Display->columnCount(); ++i) {
        ui->Display->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    // Hide the vertical header
    ui->Display->verticalHeader()->setVisible(false);
    for (int row = 0; row < ui->Display->rowCount(); row += mergeSize) {
        ui->Display->item(row, 0)->setTextAlignment(Qt::AlignTop | Qt::AlignRight);
    }

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InsertSlot(QString subject, std::pair<QTime, QTime> times, std::vector<Slots::Days> days, Slots::Type what,QString section,QColor subjectColor) {
    // Mapping from days to column indices
    std::map<Slots::Days, int> dayColumnMapping = {
        {Slots::Monday, 1}, {Slots::Tuesday, 2}, {Slots::Wednesday, 3},
        {Slots::Thursday, 4}, {Slots::Friday, 5}, {Slots::Saturday, 6}, {Slots::Sunday, 7}
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
    QStringList timeLabels;
    for (int hour = 8; hour <= 19; hour++) {
        int displayHour = (hour > 12) ? (hour - 12) : hour;
        QString amPm = (hour < 12) ? "AM" : "PM";
        timeLabels << QString::number(displayHour) + amPm; // Newline character added

        for (int minute = 10; minute < 60; minute += 10) {
            timeLabels << "";
        }
    }

    int mergeSize = 6; // Number of rows to merge
    for (int row = 0; row < 72; row += mergeSize) {
        // Merge cells from 'row' to 'row + mergeSize - 1' in the first column
        ui->Display->setSpan(row, 0, mergeSize, 1);
    }
    // Populate the first column with time labels
    for (int i = 0; i < timeLabels.size(); ++i) {
        QTableWidgetItem *newItem = new QTableWidgetItem(timeLabels[i]);
        ui->Display->setItem(i, 0, newItem);
    }
    // Set the interactive mode for all columns
    ui->Display->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // Set the width of the time column (column 0) to 5
    ui->Display->setColumnWidth(0, 50);

    // Set the stretch mode for the rest of the columns
    for (int i = 1; i < ui->Display->columnCount(); ++i) {
        ui->Display->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    // Hide the vertical header
    ui->Display->verticalHeader()->setVisible(false);
    for (int row = 0; row < ui->Display->rowCount(); row += mergeSize) {
        ui->Display->item(row, 0)->setTextAlignment(Qt::AlignTop | Qt::AlignRight);
    }

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
