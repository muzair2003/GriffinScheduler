#include "subject.h"

Subject::Subject(QString Name):
    m_Name(Name)
{

}

void Subject::add_Slot(std::vector <Slots*> section){

    m_Slots.push_back(section);

}

QString Subject::get_name(){
    return m_Name;
}

std::vector <Slots*> Subject::GetAllSlots(int row){
    if (row >= 0 && row < m_Slots.size()) {
        return m_Slots[row];
    } else {
        // Handle an out-of-bounds request, e.g., by returning an empty vector or throwing an exception.
        // Here, we'll return an empty vector as an example.
        return std::vector<Slots*>();
    }
}

int Subject::GetSlotSize(){
        return m_Slots.size();
}
Subject::~Subject(){

}
