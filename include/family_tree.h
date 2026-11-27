#pragma once

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include "person.h"

class FamilyTree {
public:
    FamilyTree() = default;
    FamilyTree(const std::string& filename);
    ~FamilyTree();
    bool ExistsPerson(const std::string& person) const;
    std::vector<Person*> GetChildren(const std::string& person) const;
    std::vector<Person*> GetSiblings(const std::string& person) const;
    std::vector<Person*> GetParents(const std::string& person) const ;
    std::vector<Person*> GetGrandparents(const std::string& person) const;
    std::vector<Person*> GetGrandchildren(const std::string& person) const;
    Person* GetSpouse(const std::string& person) const;
    void AddPerson(const std::string& name, const std::string& dob,
        const std::string& dod, const std::string& location);
    void AddParent(const std::string& child, const std::string& parent); 
    void AddChild(const std::string& parent, const std::string& child);
    void AddSpouse(const std::string& person, const std::string& spouse);
    void PrintImmediateFamily(const std::string& person) const;
    bool IsDirectlyRelated(const std::string& person1, const std::string& person2) const;
    bool IsDirectlyRelated(const std::string& person1, const std::string& person2, const std::string& relation);
    
private:
    std::map<Person*, std::list<std::pair<Person*, std::string>>> graph_; // Person, (other-person, relationship)
    std::map<std::string, Person*> name_to_person_;
    std::string filename_;
    int next_id_ = 1;

    std::unordered_set<int> new_person_ids_;    // unordered since OK for int and faster for int
    std::unordered_set<int> deleted_person_ids_;
    std::set<std::tuple<int, int, std::string>> new_relationships_; // not unordered since cannot hash tuple
    std::set<std::tuple<int, int, std::string>> deleted_relationships_;

    std::optional<std::chrono::year_month_day> ParseDate(const std::optional<std::string>& date_str) const;
};

std::ostream& operator<<(std::ostream& os, const std::vector<Person*>& vec);