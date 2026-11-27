#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <format>

struct Person {
    int id;
    std::string name;
    std::optional<std::chrono::year_month_day> dob = std::nullopt;
    std::optional<std::chrono::year_month_day> dod = std::nullopt;
    std::optional<std::string> location = std::nullopt;

    Person() = delete;
    Person(int id, std::string name,
        std::optional<std::chrono::year_month_day> dob, std::optional<std::chrono::year_month_day> dod,
        std::optional<std::string> location);

    void changeName(std::string newName);
    void changeDOB(std::optional<std::chrono::year_month_day> newDOB);
    void changeDOD(std::optional<std::chrono::year_month_day> newDOD);
    void changeLocation(std::optional<std::string> newLocation);

    std::string getDOB() const;
    std::string getDOD() const;
    std::string getLocation() const;

};