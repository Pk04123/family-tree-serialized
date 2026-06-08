#include "family_tree.hpp"

#include <fstream>
#include <stdexcept>

FamilyTree::FamilyTree(const std::string& people_file, const std::string& children_file, const std::string& spouse_file) {
    std::ifstream people_fs{people_file};
    std::ifstream children_fs{children_file};
    std::ifstream spouse_fs{spouse_file};
    if (!people_fs.is_open() || !children_fs.is_open() || !spouse_fs.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    // Main file parsing - add all people to graph
    std::string person_name;
    while (people_fs.good()) {
        people_fs >> person_name;
        graph_[person_name] = std::list<std::pair<std::string, std::string>>();
    }

    // Children file parsing - add children relations to graph
    std::string parent_name;
    char throw_away;
    while (children_fs.good()) {
        children_fs >> parent_name;
        children_fs >> throw_away;
        if (children_fs.fail()) {
            break;
        }
        SetChildren(parent_name, children_fs);
    }
     
    // Spouse File Parsing
    std::string person_1;
    std::string person_2;
    while (spouse_fs.good()) {
        spouse_fs >> person_1;
        spouse_fs >> person_2;
        SetSpouse(person_1, person_2);
    }
}

void FamilyTree::SetChildren(const std::string& parent, std::ifstream& children_fs) {
    std::string child;
    while (true) {
        children_fs >> child;
        if (child == ";") {
            break;
        }
        SetChild(parent, child);       
    }
}

void FamilyTree::SetChild(const std::string& parent, const std::string& child) {
    graph_[parent].push_back(std::make_pair(child, "child"));
    graph_[child].push_back(std::make_pair(parent, "parent"));
    std::string spouse = GetSpouse(parent);
    if (!spouse.empty()) {
        graph_[spouse].push_back(std::make_pair(child, "child"));
        graph_[child].push_back(std::make_pair(spouse, "parent"));
    }
}

void FamilyTree::SetSpouse(const std::string& person_1, const std::string& person_2) {
    graph_[person_1].push_back(std::make_pair(person_2, "spouse"));
    graph_[person_2].push_back(std::make_pair(person_1, "spouse"));
    // Both should have same children
    for (const auto& relation : graph_[person_1]) {
        if (relation.second == "child") {
            std::string child = relation.first;
            if (!IsDirectlyRelated(person_2, child, "child")) {
                graph_[person_2].push_back(std::make_pair(child, "child"));
                graph_[child].push_back(std::make_pair(person_2, "parent"));
            }
            
        }
    }
    for (const auto& relation : graph_[person_2]) {
        if (relation.second == "child") {
            std::string child = relation.first;
            if (!IsDirectlyRelated(person_1, child, "child")) {
                graph_[person_1].push_back(std::make_pair(child, "child"));
                graph_[child].push_back(std::make_pair(person_1, "parent"));
            }
            
        }
    }
}

bool FamilyTree::IsDirectlyRelated(const std::string& person1, const std::string& person2) const {
    for (const auto& relation : graph_.at(person1)) {
        if (relation.first == person2) {
            return true;
        }
    }
    return false;
}

bool FamilyTree::IsDirectlyRelated(const std::string& person1, const std::string& person2, const std::string& relation) {
    for (const auto& rel : graph_[person1]) {
        if (rel.first == person2 && rel.second == relation) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> FamilyTree::GetChildren(const std::string& person) const {
    std::vector<std::string> children;
    for (const auto& relation : graph_.at(person)) {
        if (relation.second == "child") {
            children.push_back(relation.first);
        }
    }
    return children;
}

std::vector<std::string> FamilyTree::GetSiblings(const std::string& person) const {
    std::vector<std::string> siblings;
    for (const auto& relation : graph_.at(person)) {
        if (relation.second == "parent") {
            // any parent works
            std::string parent = relation.first;
            for (const auto& relation : graph_.at(parent)) {
                if (relation.second == "child" && relation.first != person) {
                    // the child is not the original person and sibling not already added
                    siblings.push_back(relation.first);
                }
            }
            break; // only need to check one parent
        }
    }
    return siblings;
}

std::vector<std::string> FamilyTree::GetParents(const std::string& person) const {
    std::vector<std::string> parents;
    for (const auto& relation : graph_.at(person)) {
        if (relation.second == "parent") {
            parents.push_back(relation.first);
        }
    }
    return parents;
}

std::vector<std::string> FamilyTree::GetGrandparents(const std::string& person) const {
    std::vector<std::string> grandparents;
    for (const auto& relation : graph_.at(person)) {
        if (relation.second == "parent") {
            std::string parent = relation.first;
            for (const auto& relation : graph_.at(parent)) {
                // Parent's parents for both
                if (relation.second == "parent") {
                    grandparents.push_back(relation.first);
                }
            }
        }
    }
    return grandparents;
}

std::vector<std::string> FamilyTree::GetGrandchildren(const std::string& person) const {
    std::vector<std::string> grandchildren;
    for (const auto& relation : graph_.at(person)) {
        if (relation.second == "child") {
            std::string child = relation.first;
            for (const auto& relation : graph_.at(child)) {
                // Child's children for both
                if (relation.second == "child") {
                    grandchildren.push_back(relation.first);
                }
            }
        }
    }
    return grandchildren;
}

std::string FamilyTree::GetSpouse(const std::string& name) const {
    for (const auto& relation : graph_.at(name)) {
        if (relation.second == "spouse") {
            return relation.first;
        }
    }
    return "";
}

bool FamilyTree::ExistsPerson(const std::string& name) {
    return graph_.contains(name);
    // Error because the linter is not aware of C++20
}

void FamilyTree::PrintImmediateFamily(const std::string& person) const {
    std::cout << "Here" << std::endl;
    std::cout << "Parents:" << std::endl;
    std::vector<std::string> parents = GetParents(person);
    if (parents.empty()) {
        std::cout << "\tNone" << std::endl;
    } else {
        std::cout << parents;
    }

    std::cout << "Children:" << std::endl;
    std::vector<std::string> children = GetChildren(person);
    if (children.empty()) {
        std::cout << "\tNone" << std::endl;
    } else {
        std::cout << children;
    }

    std::cout << "Siblings:" << std::endl;
    std::vector<std::string> siblings = GetSiblings(person);
    if (siblings.empty()) {
        std::cout << "\tNone" << std::endl;
    } else {
        std::cout << siblings;
    }

    std::cout << "Spouse:" << std::endl;
    std::string spouse = GetSpouse(person);
    if (spouse.empty()) {
        std::cout << "\tNone" << std::endl; 
    } else {
        std::cout << '\t' << spouse << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec) {
    for (const auto& elem : vec) {
        os << '\t' << elem << std::endl;
    }
    return os;
}