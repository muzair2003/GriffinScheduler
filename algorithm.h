#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QStandardItemModel>
#include <QStandardItem>
#include "subject.h"
#include "slots.h"
#include <vector>

class MainWindow;

class Algorithm
{
public:
    Algorithm(); // Modify the constructor
     std::vector<std::vector<Slots*>> Solve(std::vector<Subject*> all); // Use a vector of pointers to Subject

private:
    void BackTrack(std::vector<Subject*> all, int current_subject_index,int previous_index);
    bool IsValid(std::vector<Slots*> one, std::vector<Slots*> two);
        std::vector<Slots*> currentSchedule;

    std::vector<std::vector<Slots*>> allSchedules;
};

#endif // ALGORITHM_H
