#ifndef SUBJECT_SLOTS_H
#define SUBJECT_SLOTS_H

#include <QWidget>
#include "slots.h"
class subject_slots
{
private:
    Slots m_Lec;
    Slots m_Lab;
    Slots m_Sem;
public:
public:
    subject_slots(Slots Lec);
    subject_slots(Slots Lec,Slots Lab);
    subject_slots(Slots Lec,Slots Lab, Slots Sem);
};

#endif // SUBJECT_SLOTS_H
