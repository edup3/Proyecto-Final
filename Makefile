# Makefile simple para gsea
CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Wall -Wextra
CXXFLAGS2 = -std=c++17 -pthread -Wall -Wextra
TARGET = gsea

all: $(TARGET)

SRCS = $(wildcard src/*.cpp)

$(TARGET): src/gsea.cpp
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET).exe

debug: $(TARGET)/debug

$(TARGET)/debug: src/gsea.cpp
	$(CXX) -g $(CXXFLAGS2) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.exe
