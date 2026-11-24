CXX=clang++
CXX_FLAGS=-std=c++20 -Iinclude -Ithird_party/SQLiteCpp/include -Wall -Wextra -g
LDFLAGS=-lsqlite3	# linker flag

SQLITECPP_SRCS=$(wildcard third_party/SQLiteCpp/src/*.cpp)	# wildcard finds all .cpp files (for disk)
SQLITECPP_OBJS=$(SQLITECPP_SRCS:.cpp=.o)	# generates all .o targets 

exec: bin/exec

clean:
	rm -rf bin/exec src/*.o $(SQLITECPP_OBJS)

bin/exec: src/main.o src/family_tree.o src/person.o $(SQLITECPP_OBJS)
	mkdir -p bin
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LDFLAGS)		# $^ is all the dependencies

src/main.o: src/main.cpp include/family_tree.h include/person.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@				# $< is the first dependency (the .cpp file)

src/family_tree.o: src/family_tree.cpp include/family_tree.h include/person.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@

src/person.o: src/person.cpp include/person.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@

third_party/SQLiteCpp/src/%.o: third_party/SQLiteCpp/src/%.cpp		# % is a wildcard for the filename (for makefile)
	$(CXX) $(CXX_FLAGS) -w -c $< -o $@

.PHONY: exec clean