CXX := g++
CXXFLAGS := -std=c++17 -Wall -O2

TARGET := lab_1_1
SRC := lab_1_1.cpp
OBJ := $(SRC:.cpp=.o)
INPUT ?= hello.txt
KEY ?= key.txt
MODE ?= -C

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(INPUT) $(KEY) $(MODE)

clean: 
	rm -f $(TARGET) $(OBJ)

.PHONY: all run clean
