#pragma once

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "person.h"

class FamilyTree {
public:
    FamilyTree() = default;
    FamilyTree(const std::string& filename);
    ~FamilyTree();
    bool ExistsPerson(const std::string& person);
    std::vector<Person*> GetChildren(const std::string& person) const;
    std::vector<Person*> GetSiblings(const std::string& person) const;
    std::vector<Person*> GetParents(const std::string& person) const ;
    std::vector<Person*> GetGrandparents(const std::string& person) const;
    std::vector<Person*> GetGrandchildren(const std::string& person) const;
    Person* GetSpouse(const std::string& person) const;
    // void AddPerson(const std::string& person); @TODO with new Person class
    // void AddParent(const std::string& child, const std::string& parent); -- @TODO with new Person class
    // void AddChild(const std::string& parent, const std::string& child); -- @TODO with new Person class
    // void AddSpouse(const std::string& person, const std::string& spouse); -- @TODO with new Person class
    void PrintImmediateFamily(const std::string& person) const;
    bool IsDirectlyRelated(const std::string& person1, const std::string& person2) const;
    bool IsDirectlyRelated(const std::string& person1, const std::string& person2, const std::string& relation);
    
private:
    std::map<Person*, std::list<std::pair<Person*, std::string>>> graph_;
    std::map<std::string, Person*> name_to_person_;
    std::string filename_;
};

std::ostream& operator<<(std::ostream& os, const std::vector<Person*>& vec);