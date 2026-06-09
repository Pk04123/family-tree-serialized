#include "family_tree.hpp"

#include <fstream>
#include <stdexcept>

FamilyTree::FamilyTree(const std::string& filename) {
    std::ifstream ifs{filename};
    if (!ifs.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    cereal::JSONInputArchive archive(ifs);
    archive(*this); 
}

bool FamilyTree::ExistsPerson(const std::string& name) {
    return graph_.contains(name);
}

void FamilyTree::AddPerson(const std::string& person) {
    // Only the first person should have no relation
    if (graph_.empty()) {
        graph_[person] = std::list<std::pair<std::string, std::string>>();
    } else {
        throw std::invalid_argument("Person already exists: " + person);
    }
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

std::string FamilyTree::GetSpouse(const std::string& name) const {
    for (const auto& relation : graph_.at(name)) {
        if (relation.second == "spouse") {
            return relation.first; // Assuming one spouse per person
        }
    }
    return "";
}

void FamilyTree::AddChild(const std::string& parent, const std::string& child) {
    if (!ExistsPerson(parent)) {
        throw std::invalid_argument("Parent does not exist: " + parent);
    }
    graph_[parent].push_back(std::make_pair(child, "child"));
    graph_[child].push_back(std::make_pair(parent, "parent"));
    std::string spouse = GetSpouse(parent);
    if (!spouse.empty()) {
        graph_[spouse].push_back(std::make_pair(child, "child"));
        graph_[child].push_back(std::make_pair(spouse, "parent"));
    }
}

void FamilyTree::AddSpouse(const std::string& person_1, const std::string& person_2) {
    if (!ExistsPerson(person_1)) {
        throw std::invalid_argument("Person does not exist: " + person_1);
    }
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
    for (const auto& rel : graph_.at(person1)) {
        if (rel.first == person2) {
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

void FamilyTree::SaveTree(const std::string& filename) const {
    std::ofstream ofs{filename};
    if (!ofs.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    // Explicit scope block forces the archive to destroy and flush 
    // its data to the file BEFORE ofs closes.
    {
        cereal::JSONOutputArchive archive(ofs);
        archive(*this); // Triggers FamilyTree::serialize
    } 
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec) {
    for (const auto& elem : vec) {
        os << '\t' << elem << std::endl;
    }
    return os;
}