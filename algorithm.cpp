#include "algorithm.h"

Algorithm::Algorithm()

{
}
bool Algorithm::IsValid(std::vector<Slots*> one, std::vector<Slots*> two){
    if(one.empty() || two.empty()){
        return true; // If any set is empty, no conflict is possible.
    }
    std::pair<QTime,QTime> time_range_one;
    std::pair<QTime,QTime> time_range_two;
    for(int i = 0; i < one.size(); i++){
        for (int j = 0; j < two.size(); j++){

            if (one[i] != nullptr) {
                std::pair<QTime,QTime> time_range_one = one[i]->GetTime();
            }
            if (two[j] != nullptr) {
                std::pair<QTime,QTime> time_range_two = two[j]->GetTime();
            }


            // First, check if the time ranges overlap
            if(!(time_range_one.second <= time_range_two.first || time_range_one.first >= time_range_two.second)){
                // If they overlap, check for common days
                std::vector<Slots::Days> first = one[i]->GetDay();
                std::vector<Slots::Days> second = two[j]->GetDay();
                std::vector<Slots::Days> common_elements;
                std::set_intersection(
                    first.begin(), first.end(),
                    second.begin(), second.end(),
                    std::back_inserter(common_elements)
                    );
                // If there are common elements (days), then there is a conflict
                if (!common_elements.empty()){
                    return false;
                }
            }
        }
    }
    // If no conflicts are found, return true
    return true;
}

void Algorithm::BackTrack(std::vector<Subject*> all, int current_subject_index,int previous_index) {


    if (current_subject_index >= all.size() && current_schedule.size() == 3* all.size()) {
        all_schedules.push_back(current_schedule);
        return;
    }

    for (int i=0;i<all[current_subject_index]->GetSlotSize();i++){

        std::vector<Slots*> temp = all[current_subject_index]->GetAllSlots(i);
//        if (current_subject_index==0){
//            current_schedule.insert(current_schedule.end(), temp.begin(), temp.end());
//            BackTrack(all, i+1,i);
//    }
        //else{
        if (IsValid(current_schedule,temp)){
            for(int j=0;j<temp.size();j++){
                current_schedule.push_back(temp[j]);
            }
                BackTrack(all,current_subject_index+1,i);

            if(current_schedule.size()> 3*current_subject_index){
                for (int k = 0; k < 3; k++) {
                current_schedule.pop_back();
                }
            }
            //}
        }
}

}



 std::vector<std::vector<Slots*>> Algorithm::Solve(std::vector<Subject*> all){



    // Start the backtracking process with the first subject
    BackTrack(all, 0, 0);
return all_schedules;
}
