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

std::string Person::getDOB() const {
    if (dob == std::nullopt) {
        return "DOB does not exist";
    }
    // Format the date as YYYY-MM-DD (that's why 04, 02, 02)
    return std::format("{:04}-{:02}-{:02}", static_cast<int>(dob->year()), static_cast<unsigned>(dob->month()), static_cast<unsigned>(dob->day()));
}

std::string Person::getDOD() const {
    if (dod == std::nullopt) {
        return "DOD does not exist";
    }
    return std::format("{:04}-{:02}-{:02}", static_cast<int>(dod->year()), static_cast<unsigned>(dod->month()), static_cast<unsigned>(dod->day()));
}

std::string Person::getLocation() const {
    if (location == std::nullopt) {
        return "Location does not exist";
    }
    return *location;
}