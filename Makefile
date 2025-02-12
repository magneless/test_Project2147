CXX := g++

FLAGS := -lpthread -lsqlite3

all: build

build: main

main: main.o validation.o
	$(CXX) main.o validation.o -o main $(FLAGS)

main.o: main.cpp
	$(CXX) main.cpp -c

validation.o: validation.cpp
	$(CXX) validation.cpp -c

run: build
	./main

clean:
	rm -rf *.o main
