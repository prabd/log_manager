# project details
EXECUTABLE  = manager
PROJECTFILE = manager.cpp

# compiler to use
CXX         = g++

# list of test drivers (with main()) for development
TESTSOURCES = $(wildcard test*.cpp)

# names of test executables
TESTS       = $(TESTSOURCES:%.cpp=%)

# list of sources used in project
SOURCES     = $(wildcard *.cpp)
SOURCES     := $(filter-out $(TESTSOURCES), $(SOURCES))

# list of objects used in project
OBJECTS     = $(SOURCES:%.cpp=%.o)

# list of flags used
CXXFLAGS = -std=c++1z -Wconversion -Wall -Werror -Wextra -pedantic 

# make release - will compile "all" with $(CXXFLAGS) and the -O3 flag
#                also defines NDEBUG so that asserts will not check
release: CXXFLAGS += -O3 -DNDEBUG
release:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

# make debug - will compile sources with $(CXXFLAGS) and the -g3 flag
#              also defines DEBUG, so "#ifdef DEBUG /*...*/ #endif" works
debug: CXXFLAGS += -g3 -DDEBUG
debug:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_debug

# make profile - will compile "all" with $(CXXFLAGS) and the -pg flag
profile: CXXFLAGS += -pg
profile:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_profile


# Build all executables
all: release debug profile

# rule for creating objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp

# make clean - remove .o files, executables, tarball
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE)_debug $(EXECUTABLE)_profile \
      $(TESTS)
	rm -Rf *.dSYM

default: 
	release

# these targets do not create any files
.PHONY: all release debug profile clean
# disable built-in rules
.SUFFIXES:
