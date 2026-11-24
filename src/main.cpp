#include "family_tree.h"

int main() {
    std::cout << "Family tree starting..." << std::endl;
    FamilyTree ft("data/data.db"); // Load existing family tree from db
 
    // Dadi's family

    std::vector<Person*> children = ft.GetChildren("Renuka Devi");
    std::cout << "Children of Renuka Devi:" << std::endl;
    for (const Person* child : children) {
        std::cout << child->name << std::endl;
    }

    std::vector<Person*> grandparents = ft.GetGrandparents("Pranet Kumar");
    std::cout << "Grandparents of Pranet Kumar:" << std::endl;
    for (const Person* grandparent : grandparents) {
        std::cout << grandparent->name << std::endl;
    }

    Person* spouse = ft.GetSpouse("Siddharth Kumar");
    if (spouse != nullptr) {
        std::cout << "Spouse of Siddharth Kumar: " << spouse->name << std::endl;
    } else {
        std::cout << "Siddharth Kumar has no spouse recorded." << std::endl;
    }

    std::cout << "Done" << std::endl;
}