#ifndef SLOTS_H
#define SLOTS_H

#include <Qtime>
#include <vector>
#include <QWidget>

class Slots
{


public:
    enum Type { Lecture, Lab, Seminar};
    enum Days { Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday };
    Slots(std::vector<Days> day,Type hell,QTime Start,QTime End,QString subject);
    std::vector<Slots::Days> GetDay();
    std::pair<QTime, QTime> GetTime();
    QString GetSubject();
    Slots::Type GetType();
private:
    std::vector<Days> m_Day;
    Type m_hell;
    QTime m_Start;
    QTime m_End;
    QString m_sub;
};

#endif // SLOTS_H
