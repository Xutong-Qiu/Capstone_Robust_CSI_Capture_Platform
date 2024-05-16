CXX = g++ 
CXXFLAGS = -std=c++11 
LDFLAGS = -L/usr/lib
LDLIBS = -lssh

# Targets
all: read_output ssh_execute

read_output: read_output.cpp constants.h
	$(CXX) $(CXXFLAGS) -o read_output read_output.cpp $(LDFLAGS) $(LDLIBS)

ssh_execute: ssh_execute.cpp constants.h
	$(CXX) $(CXXFLAGS) -o ssh_execute ssh_execute.cpp $(LDFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	rm -f read_output ssh_execute
