# Family Tree

A graph-based project to model and understand extended family relationships.

## Description

I’ve always had trouble understanding my extended family — second cousins, my dad’s cousins, and the complex naming system used in Indian families where each relationship has a unique term.  

Instead of asking someone in my family every time whose birthday it is or how I should address them, I decided to build a program that models my family tree using the skills I learned in **CS128 @ UIUC**.

This project represents family relationships as a graph structure and allows querying relationships through code (see the `.hpp` files for available query functions).

## Features
- Graph-based representation of family relationships  
- String-based person structure  
- Query system for relations  
- Extensible design for future features  

## What I Used
- Language: C++  
- Concepts: Graphs, data structures, BFS  
- Tools: Git, GitHub, VSCode, Debugger  

## Usage
- When creating a new family tree, use the default constructor
- When adding the first person to an empty family tree, use the AddPerson() function
- To add people further, use one of the mutator ("setter") functions declared in the header (.hpp) file
- Relations can be accessed via accessors ("getters") defined in the header (.hpp) file.  
- In the terminal, type 'make exec' to compile and './bin/exec' to run 'driver.cc'
- End the program with the SaveTree() function to write the contents of a graph to a JSON file
- To edit and load a graph saved to a JSON file, use the parameterized constructor (with the filename as input)

## Roadmap
- Add visualization of the family tree (graph rendering)  
- Create a `Person` class abstraction  
- Add CLI-based interactive querying  
- Add relationship name mapping (Indian kinship terms)  

## Status
In development
