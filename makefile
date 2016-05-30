CXX = g++
CXXFLAGS = -O2 -Wall

all: main

clean:
	rm -f -r *.o
	rm -f main

rebuild: clean all

.PHONY: rebuild clean all

main.o: main.cc main.hpp makefile
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main: main.o makefile
	$(CXX) $(CXXFLAGS) -o $@ $<
