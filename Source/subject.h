#ifndef SUBJECT_H
#define SUBJECT_H

#include <QWidget>
#include "slots.h"

class Subject
{

public:

    Subject(QString Name,QColor color);
    void add_Slot(std::vector <Slots*> section);
    QString get_name();
    QColor get_color();
    std::vector <Slots*> GetAllSlots(int row);
    int GetSlotSize();
    ~Subject();
private:
    QString m_Name;
    QColor m_color;
    std::vector<std::vector <Slots*>> m_Slots;
};
#endif // SUBJECT_H
