#include "family_tree.h"

#include <fstream>
#include <stdexcept>
#include <SQLiteCpp/SQLiteCpp.h>

std::optional<std::chrono::year_month_day> FamilyTree::ParseDate(const std::optional<std::string>& str) const {
    if (str == std::nullopt || str->empty()) return std::nullopt;

    // need to dereference for std::optional
    if (str->size() != 10 || (*str)[4] != '-' || (*str)[7] != '-') {
        return std::nullopt; // Invalid format
    }

    for (size_t i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if ((*str)[i] < '0' || (*str)[i] > '9') return std::nullopt;
    }

    int year = std::stoi(str->substr(0, 4));
    unsigned month = std::stoi(str->substr(5, 2));
    unsigned day = std::stoi(str->substr(8, 2));

    std::chrono::year_month_day ymd{
        std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}
    };

    return ymd.ok() ? std::optional<std::chrono::year_month_day>{ymd} : std::nullopt;
    //  ymd.ok() checks if date is valid
}

FamilyTree::FamilyTree(const std::string& filename) : filename_(filename) {
    SQLite::Database db(filename_, SQLite::OPEN_READONLY);
    std::unordered_map<int, Person*> id_to_person;  // needed for mapping relationships later

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

        if (id >= next_id_) {
            next_id_ = id + 1; // ensure next_id_ is always greater than the max id in the database
        }
    }

    // Load Relationships
    SQLite::Statement rel_query(db, "SELECT * FROM relationships;");
    while (rel_query.executeStep()) {
        Person* p1 = id_to_person.at(rel_query.getColumn(0)); // get the pointer based on id
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

bool FamilyTree::ExistsPerson(const std::string& name) const{
    return name_to_person_.contains(name);
}

void FamilyTree::AddPerson(const std::string& name, const std::string& dob,
    const std::string& dod, const std::string& location) {

    if (name.empty()) {
        throw std::invalid_argument("Person name cannot be empty.");
    }
    if (name_to_person_.contains(name)) {
        throw std::invalid_argument("Person already exists: " + name);
    }

    auto dob_opt = dob.empty() ? std::nullopt : std::optional<std::string>(dob);    // make into optional string
    auto dod_opt = dod.empty() ? std::nullopt : std::optional<std::string>(dod);
    auto location_opt = location.empty() ? std::nullopt : std::optional<std::string>(location);

    auto parsed_dob = ParseDate(dob_opt);
    auto parsed_dod = ParseDate(dod_opt);

    if (!dob.empty() && parsed_dob == std::nullopt) {   // nullopt when non-empty means parsing failed
        throw std::invalid_argument("Invalid DOB (expected YYYY-MM-DD): " + dob);
    }
    if (!dod.empty() && parsed_dod == std::nullopt) {
        throw std::invalid_argument("Invalid DOD (expected YYYY-MM-DD): " + dod);
    }

    Person* p = new Person(next_id_++, name, parsed_dob, parsed_dod, location_opt);
    name_to_person_[name] = p;
    graph_[p] = std::list<std::pair<Person*, std::string>>();

    if (deleted_person_ids_.contains(p->id)) {
        deleted_person_ids_.erase(p->id); // if previously deleted, now it's new again
    } else {
        new_person_ids_.insert(p->id);
    }
}

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
    if (!ExistsPerson(name)) {
        throw std::invalid_argument("Person does not exist: " + name);
    }

    Person* personPtr = name_to_person_.at(name);
    for (const auto& relation : graph_.at(personPtr)) {
        if (relation.second == "spouse") {
            return relation.first; // Assuming one spouse per person
        }
    }
    return nullptr; // No spouse found
}

void FamilyTree::AddParent(const std::string& child, const std::string& parent) {
    if (!ExistsPerson(parent)) {
        throw std::invalid_argument("Parent does not exist: " + parent);
    }
    if (!ExistsPerson(child)) {
        throw std::invalid_argument("Child does not exist: " + child);
    }

    Person* parentPtr = name_to_person_[parent];
    Person* childPtr = name_to_person_[child];
    graph_[parentPtr].push_back(std::make_pair(childPtr, "child"));
    graph_[childPtr].push_back(std::make_pair(parentPtr, "parent"));

    Person* spouse = GetSpouse(parent);
    bool added_spouse_edge = false;
    if (spouse != nullptr && !IsDirectlyRelated(spouse->name, child, "child")) {     // don't want to double add
        graph_[spouse].push_back(std::make_pair(childPtr, "child"));
        graph_[childPtr].push_back(std::make_pair(spouse, "parent"));
        added_spouse_edge = true;
    }

    // SQL only stores PARENT edges
    auto parent_edge = std::make_tuple(parentPtr->id, childPtr->id, std::string("PARENT"));
    if (deleted_relationships_.contains(parent_edge)) {
        deleted_relationships_.erase(parent_edge);
    } else {
        new_relationships_.insert(parent_edge);
    }

    if (added_spouse_edge) {
        auto spouse_edge = std::make_tuple(spouse->id, childPtr->id, std::string("PARENT"));
        if (deleted_relationships_.contains(spouse_edge)) {
            deleted_relationships_.erase(spouse_edge);
        } else {
            new_relationships_.insert(spouse_edge);
        }
    }
}

void FamilyTree::AddChild(const std::string& parent, const std::string& child) {
    AddParent(child, parent); // Reuse AddParent since SQLite stores "PARENT" edges
}

void FamilyTree::AddSpouse(const std::string& person1, const std::string& person2) {
    if (!ExistsPerson(person1)) {
        throw std::invalid_argument("Person does not exist: " + person1);
    }
    if (!ExistsPerson(person2)) {
        throw std::invalid_argument("Person does not exist: " + person2);
    }
    if (IsDirectlyRelated(person1, person2, "spouse")) {
        return;
    }

    Person* p1 = name_to_person_[person1];
    Person* p2 = name_to_person_[person2];

    graph_[p1].push_back(std::make_pair(p2, "spouse"));
    graph_[p2].push_back(std::make_pair(p1, "spouse"));

    std::vector<std::tuple<int, int, std::string>> added_parent_edges;
    // to keep track of children added

    for (Person* child : GetChildren(person1)) {
        if (!IsDirectlyRelated(person2, child->name, "child")) {
            graph_[p2].push_back(std::make_pair(child, "child"));
            graph_[child].push_back(std::make_pair(p2, "parent"));
            added_parent_edges.push_back(std::make_tuple(p1->id, child->id, "PARENT"));
        }
    }

    for (Person* child : GetChildren(person2)) {
        if (!IsDirectlyRelated(person1, child->name, "child")) {
            graph_[p1].push_back(std::make_pair(child, "child"));
            graph_[child].push_back(std::make_pair(p1, "parent"));
            added_parent_edges.emplace_back(std::make_tuple(p1->id, child->id, "PARENT"));
        }
    }

    // min and max provide consistency to what is being added, and prevent adding in a duplicate direction
    auto [min_id, max_id] = std::minmax(p1->id, p2->id);
    auto spouse_edge = std::make_tuple(min_id, max_id, std::string("SPOUSE"));
    if (deleted_relationships_.contains(spouse_edge)) {
        deleted_relationships_.erase(spouse_edge);
    } else {
        new_relationships_.insert(spouse_edge);
    }

    for (const auto& edge : added_parent_edges) {
        if (deleted_relationships_.contains(edge)) {
            deleted_relationships_.erase(edge);
        } else {
            new_relationships_.insert(edge);
        }
    }
}

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
    try {
        SQLite::Database db(filename_, SQLite::OPEN_READWRITE);
        db.exec("PRAGMA foreign_keys = ON;"); // is needed for automatically deleting relationships when a person is deleted from the main table
        SQLite::Transaction transaction(db);

         // 1. Delete standalone removed relationships
        if (!deleted_relationships_.empty()) {
            SQLite::Statement del_single_rel(db,
                "DELETE FROM relationships "
                "WHERE (person1_id = ? AND person2_id = ? AND type = ?); ");
            for (const auto& [id1, id2, type] : deleted_relationships_) {
                del_single_rel.bind(1, id1); // like python's .format()
                del_single_rel.bind(2, id2);
                del_single_rel.bind(3, type);
                del_single_rel.bind(4, id2);
                del_single_rel.bind(5, id1);
                del_single_rel.exec();      // run the command
                del_single_rel.reset();
            }
        }

        // 2. Delete removed people and any relationships involving them
        if (!deleted_person_ids_.empty()) {
            SQLite::Statement del_rel(db,
                "DELETE FROM relationships WHERE person1_id = ? OR person2_id = ?;");
            SQLite::Statement del_person(db,
                "DELETE FROM people WHERE id = ?;");
            for (int id : deleted_person_ids_) {
                // first delete all relationships
                del_rel.bind(1, id);
                del_rel.bind(2, id);
                del_rel.exec();
                del_rel.reset();
                // and then the person
                del_person.bind(1, id);
                del_person.exec();
                del_person.reset();
            }
        }
        // 3. Insert new people (must be done before relationship inserts, since they have been done one people newly inserted)
        if (!new_person_ids_.empty()) {
            SQLite::Statement ins_person(db,
                "INSERT INTO people VALUES (?, ?, ?, ?, ?);");
            for (const auto& [name, p] : name_to_person_) {     // for all people
                if (!new_person_ids_.contains(p->id)) {         // if they are not new, skip
                    continue;
                }
                ins_person.bind(1, p->id);
                ins_person.bind(2, p->name);
                p->dob != std::nullopt ? ins_person.bind(3, p->getDOB()) : ins_person.bind(3, nullptr);
                p->dod != std::nullopt ? ins_person.bind(4, p->getDOD()) : ins_person.bind(4, nullptr);
                p->location != std::nullopt ? ins_person.bind(5, p->getLocation()) : ins_person.bind(5, nullptr);
                ins_person.exec();
                ins_person.reset();
            }
        }

        // 4. Insert new relationships
        if (!new_relationships_.empty()) {
            SQLite::Statement ins_rel(db,
            "INSERT INTO relationships VALUES (?, ?, ?);");
            for (const auto& [id1, id2, type] : new_relationships_) {
                ins_rel.bind(1, id1);
                ins_rel.bind(2, id2);
                ins_rel.bind(3, type);
                ins_rel.exec();
                ins_rel.reset();
                }
            }

        transaction.commit();

    } catch (const std::exception& e) {
        // don't crash, since we need to free memory
        std::cerr << "Failed to save family tree to SQLite: " << e.what() << std::endl;
    }

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