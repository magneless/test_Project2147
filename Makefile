CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 -O2
LDFLAGS := -lpthread -lsqlite3

all: main

main: main.o validation.o placesDB.o
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@

run: main
	./main

format:
	clang-format -i *.cpp *.h

clean:
	rm -rf *.o main
