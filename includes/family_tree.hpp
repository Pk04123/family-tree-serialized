#ifndef FAMILY_TREE_HPP
#define FAMILY_TREE_HPP

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <cereal/types/map.hpp>
#include <cereal/archives/json.hpp>

class FamilyTree {
public:
    FamilyTree() = default;
    FamilyTree(const std::string& filename);
    ~FamilyTree() = default;
    bool ExistsPerson(const std::string& person);
    std::vector<std::string> GetChildren(const std::string& person) const;
    std::vector<std::string> GetSiblings(const std::string& person) const;
    std::vector<std::string> GetParents(const std::string& person) const ;
    std::vector<std::string> GetGrandparents(const std::string& person) const;
    std::vector<std::string> GetGrandchildren(const std::string& person) const;
    std::string GetSpouse(const std::string& person) const;
    void AddPerson(const std::string& person);
    void AddParent(const std::string& child, const std::string& parent);
    void AddChild(const std::string& parent, const std::string& child);
    void AddSpouse(const std::string& person, const std::string& spouse);
    void PrintImmediateFamily(const std::string& person) const;
    bool IsDirectlyRelated(const std::string& person1, const std::string& person2) const;
    bool IsDirectlyRelated(const std::string& person1, const std::string& person2, const std::string& relation);

    template <class Archive>
    void serialize(Archive& archive) { archive(graph_); }
    void SaveTree();
    
private:
    // std::pair is name, relation
    std::map<std::string, std::list<std::pair<std::string, std::string>>> graph_;
};

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec);

#endif