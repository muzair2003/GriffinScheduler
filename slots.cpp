#include "slots.h"


Slots::Slots(std::vector<Days> day, Type hell, QTime Start, QTime End, QString sub,QString section):
    m_Day(day),m_hell(hell),m_Start(Start), m_End(End),m_sub(sub),m_section(section)
{

}
std::vector<Slots::Days> Slots::GetDay(){
    return m_Day;
}

std::pair<QTime, QTime> Slots::GetTime() {
    return std::make_pair(m_Start, m_End);
}
QString Slots::GetSubject() {
    return m_sub;
}

Slots::Type Slots::GetType(){
    return m_hell;
}
QString Slots::GetSection(){
    return m_section;
}




