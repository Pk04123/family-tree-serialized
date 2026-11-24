#include "person.h"

Person::Person(int id, std::string name,
    std::optional<std::chrono::year_month_day> dob, std::optional<std::chrono::year_month_day> dod,
    std::optional<std::string> location)
    : id(id), name(name), dob(dob), dod(dod), location(location) {}

void Person::changeName(std::string newName) {
    name = newName;
}

void Person::changeDOB(std::optional<std::chrono::year_month_day> newDOB) {
    dob = newDOB;
}

void Person::changeDOD(std::optional<std::chrono::year_month_day> newDOD) {
    dod = newDOD;
}

void Person::changeLocation(std::optional<std::string> newLocation) {
    location = newLocation;
}