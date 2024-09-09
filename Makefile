# 指向 GoogleTest 的根目錄
GTEST_DIR = ./googletest

# 編譯器
CXX = g++

CXXFLAGS = -std=c++11 -Wall -Wextra -I$(GTEST_DIR)/include

CPPFLAGS += -I"C:\googletest-main\googletest\include\gtest"
LDFLAGS += -L"C:\googletest-main\googletest\src"

SRC_DIRS = device TextUI LogicSimulator

TARGET = HW1.exe

SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del device\\*.o
	del TextUI\\*.o
	del LogicSimulator\\*.o
	del $(TARGET).exe

.PHONY: all clean

# For building GoogleTest
gtest-all.o : $(GTEST_DIR)/src/gtest-all.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

gtest_main.o : $(GTEST_DIR)/src/gtest_main.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

gtest.a : gtest-all.o
	ar rcs $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	ar rcs $@ $^

# Test build
test_logic_simulator.o : LogicSimulator/test_logic_simulator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_logic_simulator : test_logic_simulator.o gtest_main.a
	$(CXX) $(CXXFLAGS) -lpthread $^ -o $@
