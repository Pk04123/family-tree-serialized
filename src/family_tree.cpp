#include "family_tree.h"

#include <fstream>
#include <stdexcept>
#include <SQLiteCpp/SQLiteCpp.h>

std::optional<std::chrono::year_month_day> ParseDate(const std::optional<std::string>& str) {
    if (str == std::nullopt) return std::nullopt;

    int year = std::stoi(str->substr(0, 4));
    unsigned month = std::stoi(str->substr(5, 2));
    unsigned day = std::stoi(str->substr(8, 2));

    return std::chrono::year_month_day{
        std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}
    };
}

FamilyTree::FamilyTree(const std::string& filename) : filename_(filename) {
    std::cout << "HERE" << std::endl;
    SQLite::Database db(filename_, SQLite::OPEN_READONLY);
    std::cout << "HERE" << std::endl;
    std::unordered_map<int, Person*> id_to_person;  // needed to map relationships 

    // Load people
    SQLite::Statement people_query(db, "SELECT * FROM people;");
    while (people_query.executeStep()) {
        int id = people_query.getColumn(0);
        std::string name = people_query.getColumn(1);

        auto dob_str = people_query.isColumnNull(2) // std::optional<std::chrono::year_month_day>
            ? std::nullopt : std::optional<std::string>(people_query.getColumn(2).getString());
            // need to convert to string() explicitly, because it may be read as a C-style string,
            // which std::optional needs
        auto dod_str = people_query.isColumnNull(3)
            ? std::nullopt : std::optional<std::string>(people_query.getColumn(3).getString());
        auto location = people_query.isColumnNull(4)
            ? std::nullopt : std::optional<std::string>(people_query.getColumn(4).getString());

        Person* p = new Person(id, name, ParseDate(dob_str), ParseDate(dod_str), location);
        name_to_person_[p->name] = p;
        graph_[p] = std::list<std::pair<Person*, std::string>>();
        id_to_person[id] = p;
    }

    // Load Relationships
    SQLite::Statement rel_query(db, "SELECT * FROM relationships;");
    while (rel_query.executeStep()) {
        Person* p1 = id_to_person.at(rel_query.getColumn(0));
        Person* p2 = id_to_person.at(rel_query.getColumn(1));
        std::string type = rel_query.getColumn(2);

        if (type == "PARENT") {
            graph_[p1].push_back({p2, "child"});
            graph_[p2].push_back({p1, "parent"});
        } else if (type == "SPOUSE") {
            graph_[p1].push_back({p2, "spouse"});
            graph_[p2].push_back({p1, "spouse"});
        }
    }
}

bool FamilyTree::ExistsPerson(const std::string& name) {
    return name_to_person_.contains(name);
}

// void FamilyTree::AddPerson(const std::string& person) {
//     // Only the first person should have no relation
//     if (graph_.empty()) {
//         graph_[person] = std::list<std::pair<std::string, std::string>>();
//     } else {
//         throw std::invalid_argument("Person already exists: " + person);
//     }
// }

std::vector<Person*> FamilyTree::GetChildren(const std::string& person) const {
    std::vector<Person*> children;
    Person* personPtr = name_to_person_.at(person);
    for (const auto& relation : graph_.at(personPtr)) {
        if (relation.second == "child") {
            children.push_back(relation.first);
        }
    }
    return children;
}

std::vector<Person*> FamilyTree::GetSiblings(const std::string& person) const {
    std::vector<Person*> siblings;
    Person* personPtr = name_to_person_.at(person);
    for (const auto& relation : graph_.at(personPtr)) {
        if (relation.second == "parent") {
            // any parent works
            Person* parent = relation.first;
            for (const auto& relation : graph_.at(parent)) {
                if (relation.second == "child" && relation.first != personPtr) {
                    // the child is not the original person and sibling not already added
                    siblings.push_back(relation.first);
                }
            }
            break; // only need to check one parent
        }
    }
    return siblings;
}

Person* FamilyTree::GetSpouse(const std::string& name) const {
    Person* personPtr = name_to_person_.at(name);
    for (const auto& relation : graph_.at(personPtr)) {
        if (relation.second == "spouse") {
            return relation.first; // Assuming one spouse per person
        }
    }
    return nullptr;
}

// void FamilyTree::AddChild(const std::string& parent, const std::string& child) {
//     if (!ExistsPerson(parent)) {
//         throw std::invalid_argument("Parent does not exist: " + parent);
//     }
//     graph_[parent].push_back(std::make_pair(child, "child"));
//     graph_[child].push_back(std::make_pair(parent, "parent"));
//     std::string spouse = GetSpouse(parent);
//     if (!spouse.empty()) {
//         graph_[spouse].push_back(std::make_pair(child, "child"));
//         graph_[child].push_back(std::make_pair(spouse, "parent"));
//     }
// }

// void FamilyTree::AddParent(const std::string& child, const std::string& parent) {
//     if (!ExistsPerson(child)) {
//         throw std::invalid_argument("Child does not exist: " + child);
//     }
//     graph_[child].push_back(std::make_pair(parent, "parent"));
//     graph_[parent].push_back(std::make_pair(child, "child"));
//     // Add spouse separately
// }

// void FamilyTree::AddSpouse(const std::string& person_1, const std::string& person_2) {
//     if (!ExistsPerson(person_1)) {
//         throw std::invalid_argument("Person does not exist: " + person_1);
//     }
//     graph_[person_1].push_back(std::make_pair(person_2, "spouse"));
//     graph_[person_2].push_back(std::make_pair(person_1, "spouse"));
//     // Both should have same children
//     for (const auto& relation : graph_[person_1]) {
//         if (relation.second == "child") {
//             std::string child = relation.first;
//             if (!IsDirectlyRelated(person_2, child, "child")) {
//                 graph_[person_2].push_back(std::make_pair(child, "child"));
//                 graph_[child].push_back(std::make_pair(person_2, "parent"));
//             }
            
//         }
//     }
//     for (const auto& relation : graph_[person_2]) {
//         if (relation.second == "child") {
//             std::string child = relation.first;
//             if (!IsDirectlyRelated(person_1, child, "child")) {
//                 graph_[person_1].push_back(std::make_pair(child, "child"));
//                 graph_[child].push_back(std::make_pair(person_1, "parent"));
//             }
            
//         }
//     }
// }

bool FamilyTree::IsDirectlyRelated(const std::string& person1, const std::string& person2) const {
    Person* person1Ptr = name_to_person_.at(person1);
    Person* person2Ptr = name_to_person_.at(person2);
    for (const auto& rel : graph_.at(person1Ptr)) {
        if (rel.first == person2Ptr) {
            return true;
        }
    }
    return false;
}

bool FamilyTree::IsDirectlyRelated(const std::string& person1, const std::string& person2, const std::string& relation) {
    Person* person1Ptr = name_to_person_.at(person1);
    Person* person2Ptr = name_to_person_.at(person2);
    for (const auto& rel : graph_.at(person1Ptr)) {
        if (rel.first == person2Ptr && rel.second == relation) {
            return true;
        }
    }
    return false;
}



std::vector<Person*> FamilyTree::GetParents(const std::string& person) const {
    std::vector<Person*> parents;
    Person* personPtr = name_to_person_.at(person);
    for (const auto& relation : graph_.at(personPtr)) {
        if (relation.second == "parent") {
            parents.push_back(relation.first);
        }
    }
    return parents;
}

std::vector<Person*> FamilyTree::GetGrandparents(const std::string& person) const {
    std::vector<Person*> grandparents;
    Person* personPtr = name_to_person_.at(person);
    for (const auto& relation : graph_.at(personPtr)) {
        if (relation.second == "parent") {
            Person* parent = relation.first;
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

std::vector<Person*> FamilyTree::GetGrandchildren(const std::string& person) const {
    std::vector<Person*> grandchildren;
    Person* personPtr = name_to_person_.at(person);
    for (const auto& relation : graph_.at(personPtr)) {
        if (relation.second == "child") {
            Person* child = relation.first;
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
    std::vector<Person*> parents = GetParents(person);
    if (parents.empty()) {
        std::cout << "\tNone" << std::endl;
    } else {
        std::cout << parents;
    }

    std::cout << "Children:" << std::endl;
    std::vector<Person*> children = GetChildren(person);
    if (children.empty()) {
        std::cout << "\tNone" << std::endl;
    } else {
        std::cout << children;
    }

    std::cout << "Siblings:" << std::endl;
    std::vector<Person*> siblings = GetSiblings(person);
    if (siblings.empty()) {
        std::cout << "\tNone" << std::endl;
    } else {
        std::cout << siblings;
    }

    std::cout << "Spouse:" << std::endl;
    Person* spouse = GetSpouse(person);
    if (spouse == nullptr) {
        std::cout << "\tNone" << std::endl; 
    } else {
        std::cout << '\t' << spouse->name << std::endl;
    }
}

FamilyTree::~FamilyTree() {
    // @TODO: save the database to the file
    for (const auto& pair : name_to_person_) {
        delete pair.second;
    }
}

std::ostream& operator<<(std::ostream& os, const std::vector<Person*>& vec) {
    for (const auto& elem : vec) {
        os << '\t' << elem->name << std::endl;
    }
    return os;
}