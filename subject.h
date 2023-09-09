#ifndef SUBJECT_H
#define SUBJECT_H

#include <QWidget>
#include "slots.h"

class Subject
{

public:

    Subject(QString Name);
    void add_Slot(std::vector <Slots*> section);
    QString get_name();
    std::vector <Slots*> GetAllSlots(int row);
    int GetSlotSize();
    ~Subject();
private:
    QString m_Name;
    std::vector<std::vector <Slots*>> m_Slots;
};
#endif // SUBJECT_H
