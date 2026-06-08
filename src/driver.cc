#include "family_tree.hpp"

int main() {
    std::cout << "Family tree starting..." << std::endl;
    FamilyTree ft("./people_file.txt", "./children_file.txt", "./spouse_file.txt");
    // Dadi's family

    // std::vector<std::string> children = ft.GetChildren("Renu");
    // std::cout << "Children of Renu:" << std::endl;
    // for (const std::string& child : children) {
    //     std::cout << child << std::endl;
    // }

    // std::vector<std::string> grandparents = ft.GetGrandparents("Pranet");
    // std::cout << "Grandparents of Pranet:" << std::endl;
    // for (const std::string& grandparent : grandparents) {
    //     std::cout <ç< grandparent << std::endl;
    // }

    // std::vector<std::string> grandchildren = ft.GetGrandchildren("Bhimlavati");
    // std::cout << "Grandchildren of Bhimlavati:" << std::endl;
    // for (const std::string& grandchild : grandchildren) {
    //     std::cout << grandchild << std::endl;
    // }

    // std::vector<std::string> parents = ft.GetParents("Tanisha");
    // std::cout << "Parents of Tanisha:" << std::endl;
    // for (const std::string& parent : parents) {
    //     std::cout << parent << std::endl;
    // }

    // std::string spouse = ft.GetSpouse("Ruchi");
    // if (!spouse.empty()) {
    //     std::cout << "Spouse of Ruchi: " << spouse << std::endl;
    // } else {
    //     std::cout << "Ruchi has no spouse recorded." << std::endl;
    // }

    // std::vector<std::string> siblings = ft.GetSiblings("Vibhu");
    // std::cout << "Siblings of Vibhu:" << std::endl;
    // for (const std::string& sibling : siblings) {
    //     std::cout << sibling << std::endl;
    // }   

    // Nani's family

    // std::vector<std::string> children = ft.GetChildren("Daddy");
    // std::cout << "Children of Daddy:" << std::endl;
    // std::cout << children;

    // std::vector<std::string> grandparents = ft.GetGrandparents("Pranet");
    // std::cout << "Grandparents of Pranet:" << std::endl;
    // std::cout << grandparents;

    // std::vector<std::string> grandchildren = ft.GetGrandchildren("BMK");
    // std::cout << "Grandchildren of BMK:" << std::endl;
    // std::cout << grandchildren;

    // std::vector<std::string> parents = ft.GetParents("Sachin");
    // std::cout << "Parents of Sachin:" << std::endl;
    // std::cout << parents;

    // std::string spouse = ft.GetSpouse("Vinita");
    // if (!spouse.empty()) {
    //     std::cout << "Spouse of Vinita: " << spouse << std::endl;
    // } else {
    //     std::cout << "Ruchi has no spouse recorded." << std::endl;
    // }

    // std::vector<std::string> siblings = ft.GetSiblings("Appu");
    // std::cout << "Siblings of Appu:" << std::endl;
    // std::cout << siblings;

    ft.PrintImmediateFamily("Suvra");
    std::cout << "Here" << std::endl;
}