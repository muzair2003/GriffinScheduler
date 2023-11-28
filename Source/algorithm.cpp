#include "algorithm.h"

Algorithm::Algorithm() {}

// Checks if two sets of slots have any conflicts
bool Algorithm::IsValid(std::vector<Slots*> setOne, std::vector<Slots*> setTwo) {
    // If any set is empty, no conflict is possible
    if (setOne.empty() || setTwo.empty()) {
        return true;
    }

    for (Slots* slotOne : setOne) {
        for (Slots* slotTwo : setTwo) {
            // Ensure neither slot is null
            if (slotOne != nullptr && slotTwo != nullptr) {
                // Find common days
                std::vector<Slots::Days> daysOne = slotOne->GetDay();
                std::vector<Slots::Days> daysTwo = slotTwo->GetDay();
                std::vector<Slots::Days> commonDays;
                std::sort(daysOne.begin(), daysOne.end());
                std::sort(daysTwo.begin(), daysTwo.end());
                std::set_intersection(
                    daysOne.begin(), daysOne.end(),
                    daysTwo.begin(), daysTwo.end(),
                    std::back_inserter(commonDays)
                    );

                // Continue to the next pair if there are no common days
                if (commonDays.empty()) {
                    continue;
                }

                // Check for time overlap
                std::pair<QTime, QTime> timeOne = slotOne->GetTime();
                std::pair<QTime, QTime> timeTwo = slotTwo->GetTime();

                // If time ranges overlap, there is a conflict
                if (timeOne.first < timeTwo.second && timeTwo.first < timeOne.second) {
                    return false;
                }
            }
        }
    }

    // No conflicts found
    return true;
}

void Algorithm::BackTrack(std::vector<Subject*> subjects, int currentIndex, int previousIndex) {
    if (currentIndex >= subjects.size() && currentSchedule.size() == 3 * subjects.size()) {
        allSchedules.push_back(currentSchedule);
        return;
    }

    for (int i = 0; i < subjects[currentIndex]->GetSlotSize(); i++) {
        std::vector<Slots*> tempSlots = subjects[currentIndex]->GetAllSlots(i);
        if (IsValid(currentSchedule, tempSlots)) {
            currentSchedule.insert(currentSchedule.end(), tempSlots.begin(), tempSlots.end());
            BackTrack(subjects, currentIndex + 1, i);

            // Remove last added slots if they exceed expected size
            if (currentSchedule.size() > 3 * currentIndex) {
                for (int k = 0; k < 3; k++) {
                    currentSchedule.pop_back();
                }
            }
        }
    }
}

std::vector<std::vector<Slots*>> Algorithm::Solve(std::vector<Subject*> subjects) {
    BackTrack(subjects, 0, 0);
    return allSchedules;
}
