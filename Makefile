CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I. -Iinclude -Ierror -Iutil
LDFLAGS = 

# Find all source files
# Using shell find to recursively find all .cpp files in src and main.cpp
SRCS = $(shell find src -name "*.cpp") main.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = RCompiler

.PHONY: all build run clean

all: build

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: build
	@./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

